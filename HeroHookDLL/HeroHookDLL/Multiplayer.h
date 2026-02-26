#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <d3d9.h>
#include <string>
#include "MultiplayerNet.h"

// ============================================================================
// Multiplayer - Main multiplayer manager for Bionicle Heroes
// ============================================================================
//
// Ties together:
//   1. Reading local player position from game memory
//   2. Sending position over UDP (MultiplayerNet)
//   3. Receiving remote player position
//   4. Drawing a cube at the remote player's position (CubeRenderer)
//   5. ImGui window for host/join/disconnect UI
//
// Call order each frame (in Present hook):
//   1. Multiplayer::Update(device)  - reads pos, sends/receives network, draws cube
//   2. Multiplayer::DrawUI()        - renders ImGui multiplayer window
//
// Game memory layout (from GameManip.cpp):
//   ExeBase + 0x252CBC -> pointer (call it P)
//   P + 0x6E0 = float X
//   P + 0x6E4 = float Z  (note: game uses Z for "depth", not up)
//   P + 0x6E8 = float Y  (up axis)
//
// ============================================================================

namespace Multiplayer {
    // Initialize the multiplayer system. Call once at startup.
    void Init();

    // Shut down. Call on DLL unload.
    void Shutdown();

    // Per-frame update: read player pos, network send/recv, draw remote cube.
    // Call BEFORE ImGui rendering in the Present hook.
    void Update(IDirect3DDevice9* device);

    // Draw the ImGui multiplayer control window.
    // Call inside the ImGui frame (between NewFrame and EndFrame).
    void DrawUI();

    // Is the UI window visible?
    inline bool showWindow = false;
}
