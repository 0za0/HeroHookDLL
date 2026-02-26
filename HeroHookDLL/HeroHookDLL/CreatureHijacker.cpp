#include "CreatureHijacker.h"
#include "GameManip.h"
#include "../imGUI/imgui.h"
#include <cmath>
#include <cstring>

// ============================================================================
// CreatureHijacker implementation
//
// Uses direct memory reads/writes to the game's Creature structures.
// All pointer dereferences are guarded with IsBadReadPtr/IsBadWritePtr.
//
// The hijacked creature has its "dead" byte set to 1, which suppresses AI
// processing and movement but keeps rendering active. Each frame we overwrite
// TerrChar.Mtx with a Y-rotation + translation matrix built from the network
// position, and zero the velocity to prevent physics drift.
// ============================================================================

using namespace CreatureHijacker::Offsets;

// --- Module state ---
static uintptr_t s_hijackedAddr = 0;      // Address of hijacked creature
static uint8_t   s_origDead = 0;           // Original dead flag to restore on release
static bool      s_active = false;
static std::vector<CreatureHijacker::CreatureInfo> s_lastScan; // Cached scan results
static bool      s_showDiag = false;       // Show diagnostic memory probe
static int       s_writeCount = 0;         // Number of SetTransform calls since hijack

// Helper: safe read a pointer-sized value
static uintptr_t SafeReadPtr(uintptr_t addr) {
    if (IsBadReadPtr((void*)addr, sizeof(uintptr_t)))
        return 0;
    return *(uintptr_t*)addr;
}

// Helper: safe read a float
static float SafeReadFloat(uintptr_t addr) {
    if (IsBadReadPtr((void*)addr, sizeof(float)))
        return 0.0f;
    return *(float*)addr;
}

// Helper: safe read a byte
static uint8_t SafeReadByte(uintptr_t addr) {
    if (IsBadReadPtr((void*)addr, 1))
        return 0;
    return *(uint8_t*)addr;
}

// Helper: safe write a float
static bool SafeWriteFloat(uintptr_t addr, float val) {
    if (IsBadWritePtr((void*)addr, sizeof(float)))
        return false;
    *(float*)addr = val;
    return true;
}

// Helper: safe write a byte
static bool SafeWriteByte(uintptr_t addr, uint8_t val) {
    if (IsBadWritePtr((void*)addr, 1))
        return false;
    *(uint8_t*)addr = val;
    return true;
}

// Helper: safe write 64 bytes (matrix)
static bool SafeWriteMatrix(uintptr_t addr, const float* data) {
    if (IsBadWritePtr((void*)addr, 64))
        return false;
    memcpy((void*)addr, data, 64);
    return true;
}

// Get the CreatureManager pointer
static uintptr_t GetCreatureManager() {
    uintptr_t exeBase = (uintptr_t)ExeBaseAddress;
    uintptr_t mgrPtrAddr = exeBase + CREATURE_MANAGER;
    return SafeReadPtr(mgrPtrAddr);
}

namespace CreatureHijacker {

std::vector<CreatureInfo> ScanCreatures() {
    std::vector<CreatureInfo> result;

    uintptr_t mgr = GetCreatureManager();
    if (!mgr) return result;

    uintptr_t curr = SafeReadPtr(mgr + MGR_LIST_HEAD);
    int safety = 0; // prevent infinite loop on corrupted list

    while (curr && safety < 100) {
        safety++;

        // Basic validity: check if the memory is readable
        if (IsBadReadPtr((void*)curr, 0x700)) break;

        int ok = (int)SafeReadPtr(curr + CR_OK);
        if (ok != 1) {
            curr = SafeReadPtr(curr + CR_NEXT);
            continue;
        }

        CreatureInfo info = {};
        info.address = curr;
        info.id = (int)SafeReadPtr(curr + CR_ID);
        info.isDead = SafeReadByte(curr + CR_DEAD) != 0;
        info.hasModel = SafeReadPtr(curr + CR_CDATA) != 0;
        info.x = SafeReadFloat(curr + CR_POS_X);
        info.y = SafeReadFloat(curr + CR_POS_Y);
        info.z = SafeReadFloat(curr + CR_POS_Z);

        result.push_back(info);
        curr = SafeReadPtr(curr + CR_NEXT);
    }

    s_lastScan = result;
    return result;
}

bool Hijack(uintptr_t creatureAddr) {
    if (s_active) Release(); // release any existing hijack first

    // Verify the creature is still valid
    if (IsBadReadPtr((void*)creatureAddr, 0x700))
        return false;

    int ok = (int)SafeReadPtr(creatureAddr + CR_OK);
    if (ok != 1) return false;

    // Save and set dead flag
    s_origDead = SafeReadByte(creatureAddr + CR_DEAD);
    if (!SafeWriteByte(creatureAddr + CR_DEAD, 1))
        return false;

    // Zero velocity to stop any physics momentum
    SafeWriteFloat(creatureAddr + CR_VELOCITY + 0, 0.0f);
    SafeWriteFloat(creatureAddr + CR_VELOCITY + 4, 0.0f);
    SafeWriteFloat(creatureAddr + CR_VELOCITY + 8, 0.0f);

    s_hijackedAddr = creatureAddr;
    s_active = true;
    s_writeCount = 0;
    return true;
}

void Release() {
    if (!s_active || !s_hijackedAddr) return;

    // Restore original dead flag
    if (!IsBadWritePtr((void*)(s_hijackedAddr + CR_DEAD), 1)) {
        SafeWriteByte(s_hijackedAddr + CR_DEAD, s_origDead);
    }

    s_hijackedAddr = 0;
    s_origDead = 0;
    s_active = false;
    s_writeCount = 0;
}

bool IsActive() {
    if (!s_active) return false;

    // Validate the hijacked creature is still accessible
    if (IsBadReadPtr((void*)s_hijackedAddr, 0x700)) {
        s_active = false;
        s_hijackedAddr = 0;
        return false;
    }

    // Check if creature was removed (ok flag cleared)
    int ok = (int)SafeReadPtr(s_hijackedAddr + CR_OK);
    if (ok != 1) {
        s_active = false;
        s_hijackedAddr = 0;
        return false;
    }

    return true;
}

void SetTransform(float x, float y, float z, float angle) {
    if (!IsActive()) return;

    uintptr_t mtx = s_hijackedAddr + CR_TERRCHAR_MTX;

    // Build a D3D row-major Y-rotation + translation matrix.
    // The game's matrix row 3 stores position as [X, height, depth, W].
    // But the Multiplayer layer passes (x, y, z) matching getCoordinates:
    //   x = X horizontal, y = depth (game Y), z = height (game Z).
    // So we swap y↔z when writing to the matrix.
    float ca = cosf(angle), sa = sinf(angle);

    float mat[16] = {
         ca,  0.0f, sa,   0.0f,  // Row 0: right
        0.0f, 1.0f, 0.0f, 0.0f,  // Row 1: up
        -sa,  0.0f, ca,   0.0f,  // Row 2: forward
         x,    z,    y,   1.0f   // Row 3: [X, height(z), depth(y), W]
    };

    SafeWriteMatrix(mtx, mat);

    // Zero velocity to prevent physics from fighting our position
    SafeWriteFloat(s_hijackedAddr + CR_VELOCITY + 0, 0.0f);
    SafeWriteFloat(s_hijackedAddr + CR_VELOCITY + 4, 0.0f);
    SafeWriteFloat(s_hijackedAddr + CR_VELOCITY + 8, 0.0f);

    // Keep dead flag set (in case something resets it)
    SafeWriteByte(s_hijackedAddr + CR_DEAD, 1);

    s_writeCount++;
}

CreatureInfo GetHijackedInfo() {
    CreatureInfo info = {};
    if (!IsActive()) return info;

    info.address = s_hijackedAddr;
    info.id = (int)SafeReadPtr(s_hijackedAddr + CR_ID);
    info.isDead = true; // we set it
    info.hasModel = SafeReadPtr(s_hijackedAddr + CR_CDATA) != 0;
    // Read from matrix row 3: [X, height(gameZ), depth(gameY), W]
    // Map to CreatureInfo matching getCoordinates convention: x=X, y=depth, z=height
    info.x = SafeReadFloat(s_hijackedAddr + CR_POS_X);
    info.y = SafeReadFloat(s_hijackedAddr + CR_POS_Y);  // depth (matrix [2])
    info.z = SafeReadFloat(s_hijackedAddr + CR_POS_Z);  // height (matrix [1])
    return info;
}

void DrawUI() {
    ImGui::Separator();
    ImGui::Text("Creature Hijacker:");

    // Always show manager status
    uintptr_t mgr = GetCreatureManager();
    if (!mgr) {
        ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "No CreatureManager (not in level?)");
        ImGui::Text("  ExeBase=0x%08X, offset=0x%X",
            (unsigned)(uintptr_t)ExeBaseAddress, (unsigned)CREATURE_MANAGER);
        return;
    }

    if (s_active) {
        CreatureInfo hi = GetHijackedInfo();
        ImGui::TextColored(ImVec4(0.3f, 1, 0.3f, 1),
            "Hijacked: ID %d @ 0x%08X", hi.id, (unsigned)hi.address);
        ImGui::Text("  Read pos: %.1f, %.1f, %.1f", hi.x, hi.y, hi.z);
        ImGui::Text("  Model: %s  Writes: %d", hi.hasModel ? "Yes" : "No", s_writeCount);
        ImGui::Text("  Dead byte: %d", SafeReadByte(s_hijackedAddr + CR_DEAD));

        // Teleport to player test: visually confirms if the offset is correct
        if (ImGui::Button("Teleport to Player")) {
            Coordinates local;
            getCoordinates(local);
            if (local.isValid) {
                SetTransform(local.x, local.y, local.z, 0.0f);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Release Creature")) {
            Release();
        }

        // Diagnostic memory probe toggle
        ImGui::Checkbox("Show Memory Probe", &s_showDiag);
        if (s_showDiag && s_hijackedAddr) {
            ImGui::Indent();

            // Read matrix at our expected offset
            ImGui::Text("TerrChar.Mtx @ +0x%X:", (unsigned)CR_TERRCHAR_MTX);
            for (int row = 0; row < 4; row++) {
                uintptr_t rowAddr = s_hijackedAddr + CR_TERRCHAR_MTX + row * 16;
                ImGui::Text("  [%d] %.3f %.3f %.3f %.3f", row,
                    SafeReadFloat(rowAddr + 0), SafeReadFloat(rowAddr + 4),
                    SafeReadFloat(rowAddr + 8), SafeReadFloat(rowAddr + 12));
            }

            // Probe nearby offsets to find the REAL position
            // (Look for 3 consecutive floats that look like world coordinates)
            ImGui::Text("Position probes (looking for ~player pos):");
            Coordinates local;
            getCoordinates(local);
            if (local.isValid)
                ImGui::Text("  Player ref: %.1f, %.1f, %.1f", local.x, local.y, local.z);

            // Probe offsets around TerrChar (0x5E0) through OldPos (0x6E0)
            for (uintptr_t probe = 0x5E0; probe <= 0x710; probe += 0x10) {
                float px = SafeReadFloat(s_hijackedAddr + probe + 0);
                float py = SafeReadFloat(s_hijackedAddr + probe + 4);
                float pz = SafeReadFloat(s_hijackedAddr + probe + 8);
                float pw = SafeReadFloat(s_hijackedAddr + probe + 12);
                // Highlight if it looks like a position (reasonable range, w=1)
                bool looksLikePos = (fabsf(px) > 1.0f || fabsf(py) > 1.0f || fabsf(pz) > 1.0f)
                                 && fabsf(px) < 10000.0f && fabsf(py) < 10000.0f && fabsf(pz) < 10000.0f
                                 && fabsf(pw - 1.0f) < 0.01f;
                if (looksLikePos)
                    ImGui::TextColored(ImVec4(0, 1, 1, 1), "  +0x%03X: %.1f, %.1f, %.1f (w=%.1f) <--",
                        (unsigned)probe, px, py, pz, pw);
                else
                    ImGui::Text("  +0x%03X: %.1f, %.1f, %.1f (w=%.1f)",
                        (unsigned)probe, px, py, pz, pw);
            }

            ImGui::Unindent();
        }
    } else {
        ImGui::Text("Manager: 0x%08X  Head: 0x%08X",
            (unsigned)mgr, (unsigned)SafeReadPtr(mgr + MGR_LIST_HEAD));

        if (ImGui::Button("Scan Creatures")) {
            ScanCreatures();
        }

        if (!s_lastScan.empty()) {
            ImGui::Text("Found %d creature(s):", (int)s_lastScan.size());

            for (size_t i = 0; i < s_lastScan.size(); i++) {
                auto& c = s_lastScan[i];

                ImGui::PushID((int)i);
                bool canHijack = c.hasModel;
                if (!canHijack) ImGui::BeginDisabled();

                char label[128];
                sprintf_s(label, "Hijack##%d", (int)i);
                if (ImGui::SmallButton(label)) {
                    Hijack(c.address);
                }

                if (!canHijack) ImGui::EndDisabled();

                ImGui::SameLine();
                ImGui::Text("ID:%d %s%s pos(%.0f, %.0f, %.0f) @0x%08X",
                    c.id,
                    c.hasModel ? "" : "[no mdl] ",
                    c.isDead ? "[dead] " : "",
                    c.x, c.y, c.z,
                    (unsigned)c.address);
                ImGui::PopID();
            }
        } else {
            ImGui::TextColored(ImVec4(1, 1, 0.3f, 1), "Click Scan to find creatures.");
        }
    }
}

} // namespace CreatureHijacker
