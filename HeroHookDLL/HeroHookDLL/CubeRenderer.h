#pragma once
#include <d3d9.h>
#include <cmath>

// ============================================================================
// CubeRenderer - Draws a colored cube at a world position using D3D9
// ============================================================================
//
// Usage: Call DrawCube() during the Present hook, BEFORE ImGui rendering.
// The renderer saves and restores all D3D9 state it modifies so it doesn't
// break the game's rendering.
//
// The cube is drawn using DrawPrimitiveUP (no vertex/index buffers needed).
// We retrieve the game's current View and Projection matrices from the device
// using GetTransform(), so the cube appears correctly in the game's 3D scene.
//
// Coordinate system: D3D left-handed, same as game.
//   X = right, Y = up, Z = into screen
//
// The cube is 3 units wide (roughly character-sized in Bionicle Heroes scale).
// ============================================================================

class CubeRenderer {
public:
    // Draw a colored cube at the given world position.
    // device: the game's IDirect3DDevice9
    // x, y, z: world position (center of cube base)
    // angle: Y-axis rotation in radians
    // color: D3DCOLOR (use D3DCOLOR_ARGB)
    // size: half-extent of the cube (default 1.5 units)
    static void DrawCube(IDirect3DDevice9* device,
                         float x, float y, float z,
                         float angle = 0.0f,
                         D3DCOLOR color = D3DCOLOR_ARGB(200, 0, 180, 255),
                         float size = 1.5f);

private:
    struct Vertex {
        float x, y, z;
        D3DCOLOR color;
    };
    static constexpr DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};
