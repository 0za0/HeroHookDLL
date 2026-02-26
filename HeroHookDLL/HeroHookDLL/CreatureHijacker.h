#pragma once
#include <windows.h>
#include <cstdint>
#include <vector>

// ============================================================================
// CreatureHijacker - Take control of an in-game Creature for multiplayer
// ============================================================================
//
// The game's Creature system uses a linked list managed by CreatureManager.
// Each creature has a ~0xF40 byte struct containing AI state, physics, position
// (TerrainCharacter matrix), and a CharInst pointer for HGP model rendering.
//
// To make a creature network-controlled:
//   1. Scan the creature list to find available creatures
//   2. Hijack one by setting its "dead" flag (suppresses AI/movement)
//   3. Each frame, overwrite its world matrix with the network position
//   4. The game's normal render pipeline draws it automatically
//
// Memory layout (PC binary, verified from Ghidra disassembly):
//   theCreatureManager at ExeBase + 0x248BF8 (absolute 0x648BF8)
//   CreatureList (m_Creatures) at manager + 0x08, head = first field
//   Creature struct (0xF40 bytes): next(+0), prev(+4), ok(+8), id(+C),
//     cdata(+20), dead(+93), TerrChar at +5E0, Mtx at TerrChar+C0 = +6A0
//   TerrChar.Mtx row 3 (position) at +6D0, OldPos at +6E0
//   Velocity at TerrChar+80 = +660
//   Matrix position order: [X, height(Z), depth(Y), W]
//   getCoordinates maps: coords.x=+6E0, coords.z=+6E4(height), coords.y=+6E8(depth)
// ============================================================================

namespace CreatureHijacker {

// PC binary offsets (verified from Ghidra disassembly of Bionicle.exe)
namespace Offsets {
    constexpr uintptr_t CREATURE_MANAGER = 0x248BF8;  // theCreatureManager global (abs 0x648BF8)
    constexpr uintptr_t MGR_LIST_HEAD    = 0x08;       // CreatureList.head within manager

    // Creature struct fields (struct size = 0xF40)
    constexpr uintptr_t CR_NEXT          = 0x00;       // next creature in linked list
    constexpr uintptr_t CR_OK            = 0x08;       // int, 1 = valid/alive
    constexpr uintptr_t CR_ID            = 0x0C;       // int, creature type ID
    constexpr uintptr_t CR_CDATA         = 0x20;       // creaturedata_s* pointer
    constexpr uintptr_t CR_DEAD          = 0x93;       // byte, apiobj.dead (1 = suppresses AI)
    constexpr uintptr_t CR_TERRCHAR      = 0x5E0;      // TerrainCharacter sub-struct
    constexpr uintptr_t CR_VELOCITY      = 0x660;      // TerrChar+0x80: float3 velocity
    constexpr uintptr_t CR_TERRCHAR_MTX  = 0x6A0;      // TerrChar+0xC0: 4x4 float matrix (64 bytes)
    constexpr uintptr_t CR_POS_X         = 0x6D0;      // Mtx row 3 [0] = X horizontal
    constexpr uintptr_t CR_POS_Z         = 0x6D4;      // Mtx row 3 [1] = height (game Z axis)
    constexpr uintptr_t CR_POS_Y         = 0x6D8;      // Mtx row 3 [2] = depth (game Y axis)
    constexpr uintptr_t CR_OLDPOS        = 0x6E0;      // TerrChar+0x100: copy of Mtx row 3
}

struct CreatureInfo {
    uintptr_t address;
    int id;
    bool isDead;
    bool hasModel;       // CharInst != NULL
    float x, y, z;
};

// Scan all creatures in the linked list. Returns empty if no CreatureManager.
std::vector<CreatureInfo> ScanCreatures();

// Hijack a creature: set dead flag, take position control.
bool Hijack(uintptr_t creatureAddr);

// Release: restore original dead flag.
void Release();

// Is a creature currently hijacked?
bool IsActive();

// Write world position + Y-axis rotation to the hijacked creature.
// Call once per frame when remote player data is available.
void SetTransform(float x, float y, float z, float angle);

// Get status of hijacked creature (returns zeroed struct if none).
CreatureInfo GetHijackedInfo();

// Draw creature control UI section (call inside ImGui frame).
void DrawUI();

} // namespace CreatureHijacker
