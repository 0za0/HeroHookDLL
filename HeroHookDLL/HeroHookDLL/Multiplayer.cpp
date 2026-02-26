#include "Multiplayer.h"
#include "MultiplayerNet.h"
#include "CubeRenderer.h"
#include "CreatureHijacker.h"
#include "GameManip.h"
#include "../imGUI/imgui.h"
#include <cmath>

// ============================================================================
// Multiplayer implementation
//
// Architecture:
//   - MultiplayerNet handles UDP socket + packet send/recv
//   - CreatureHijacker takes over a game creature for the remote player
//   - CubeRenderer is the fallback when no creature is available
//   - This file coordinates them and provides the ImGui UI
//
// Remote player representation (in priority order):
//   1. Hijacked creature: AI suppressed, position written from network data.
//      The game's own render pipeline draws the creature model automatically.
//   2. Cube fallback: D3D9 DrawPrimitiveUP cube when no creature is hijacked.
// ============================================================================

static MultiplayerNet g_net;
static char g_ipBuffer[64] = "127.0.0.1";
static int g_port = MP_DEFAULT_PORT;

// Cube appearance (fallback renderer)
static float g_cubeSize = 1.5f;
static float g_cubeColorRGB[3] = { 0.0f, 0.7f, 1.0f }; // cyan-blue
static float g_cubeAlpha = 0.78f;

namespace Multiplayer {

void Init() {
    // Nothing extra needed; MultiplayerNet initializes Winsock on demand
}

void Shutdown() {
    CreatureHijacker::Release();
    g_net.Disconnect();
}

void Update(IDirect3DDevice9* device) {
    // --- Hotkeys (work even without ImGui mouse) ---
    // F1 = Host, F2 = Join, F3 = Disconnect
    if (GetAsyncKeyState(VK_F1) & 1) {
        if (g_net.GetState() == NetState::Disconnected)
            g_net.Host((uint16_t)g_port);
    }
    if (GetAsyncKeyState(VK_F2) & 1) {
        if (g_net.GetState() == NetState::Disconnected)
            g_net.Join(g_ipBuffer, (uint16_t)g_port);
    }
    if (GetAsyncKeyState(VK_F3) & 1) {
        if (g_net.GetState() != NetState::Disconnected) {
            CreatureHijacker::Release();
            g_net.Disconnect();
        }
    }

    if (g_net.GetState() == NetState::Disconnected) return;

    // --- Read local player position from game memory ---
    Coordinates local;
    getCoordinates(local);

    if (local.isValid) {
        // Send position to peer
        // Game memory: X at +0x6E0, Z at +0x6E4, Y(up) at +0x6E8
        // We send as (X, Y, Z) matching D3D convention
        uint32_t flags = 0x03; // alive + in-level
        g_net.SendPosition(local.x, local.y, local.z, 0.0f, flags);
    }

    // --- Receive remote player updates ---
    g_net.ReceiveUpdate();

    // --- Position remote player ---
    if (g_net.IsPeerConnected()) {
        const RemotePlayer& remote = g_net.GetRemotePlayer();

        if (CreatureHijacker::IsActive()) {
            // Write remote position directly into the hijacked creature's world matrix.
            // The game's render pipeline draws it automatically — no D3D calls needed.
            CreatureHijacker::SetTransform(remote.x, remote.y, remote.z, remote.angle);
        } else if (device) {
            // Fallback: draw a colored cube at the remote position
            D3DCOLOR cubeColor = D3DCOLOR_ARGB(
                (int)(g_cubeAlpha * 255),
                (int)(g_cubeColorRGB[0] * 255),
                (int)(g_cubeColorRGB[1] * 255),
                (int)(g_cubeColorRGB[2] * 255));

            CubeRenderer::DrawCube(device,
                remote.x, remote.y, remote.z,
                remote.angle, cubeColor, g_cubeSize);
        }
    }
}

void DrawUI() {
    if (!showWindow) return;

    ImGui::SetNextWindowSize(ImVec2(420, 480), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Multiplayer", &showWindow)) {
        ImGui::End();
        return;
    }

    NetState state = g_net.GetState();

    // --- Connection status ---
    ImVec4 col;
    switch (state) {
    case NetState::Disconnected: col = ImVec4(1, 0.3f, 0.3f, 1); break;
    case NetState::Hosting:
    case NetState::Joining:      col = ImVec4(1, 1, 0.3f, 1); break;
    case NetState::Connected:    col = ImVec4(0.3f, 1, 0.3f, 1); break;
    }
    ImGui::TextColored(col, "%s", g_net.GetStatusText().c_str());
    ImGui::Separator();

    if (state == NetState::Disconnected) {
        // --- Host/Join controls ---
        ImGui::InputText("IP Address", g_ipBuffer, sizeof(g_ipBuffer));
        ImGui::InputInt("Port", &g_port);
        if (g_port < 1) g_port = 1;
        if (g_port > 65535) g_port = 65535;

        ImGui::Spacing();

        if (ImGui::Button("Host Game [F1]", ImVec2(190, 35))) {
            g_net.Host((uint16_t)g_port);
        }
        ImGui::SameLine();
        if (ImGui::Button("Join Game [F2]", ImVec2(190, 35))) {
            g_net.Join(g_ipBuffer, (uint16_t)g_port);
        }
    }
    else {
        // --- Connected/waiting controls ---
        if (ImGui::Button("Disconnect [F3]", ImVec2(190, 35))) {
            CreatureHijacker::Release();
            g_net.Disconnect();
        }

        ImGui::Spacing();
        ImGui::Text("Packets sent: %u", g_net.GetPacketsSent());
        ImGui::Text("Packets recv: %u", g_net.GetPacketsRecv());

        if (g_net.IsPeerConnected()) {
            const RemotePlayer& r = g_net.GetRemotePlayer();
            ImGui::Separator();
            ImGui::Text("Remote Player:");
            ImGui::Text("  X: %.2f  Y: %.2f  Z: %.2f", r.x, r.y, r.z);
            ImGui::Text("  Angle: %.2f  Flags: 0x%02X", r.angle, r.flags);
        }
    }

    // --- Creature Hijacker controls ---
    CreatureHijacker::DrawUI();

    // --- Cube fallback appearance settings ---
    if (!CreatureHijacker::IsActive()) {
        ImGui::Separator();
        ImGui::Text("Cube Fallback:");
        ImGui::SliderFloat("Size", &g_cubeSize, 0.5f, 10.0f, "%.1f");
        ImGui::ColorEdit3("Color", g_cubeColorRGB);
        ImGui::SliderFloat("Opacity", &g_cubeAlpha, 0.1f, 1.0f, "%.2f");
    }

    // --- Local player info ---
    ImGui::Separator();
    Coordinates local;
    getCoordinates(local);
    if (local.isValid) {
        ImGui::Text("Local Player: X:%.1f Y:%.1f Z:%.1f", local.x, local.y, local.z);
    } else {
        ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Local Player: Not in level");
    }

    ImGui::End();
}

} // namespace Multiplayer
