#include "CubeRenderer.h"
#include <cmath>

// ============================================================================
// CubeRenderer implementation
//
// Draws a simple cube using DrawPrimitiveUP with triangle list.
// No D3DX dependency - matrices are built manually.
//
// State save/restore: uses D3D9 state block to safely restore all state.
// ============================================================================

// Build a D3D-style row-major 4x4 matrix (D3DMATRIX layout)
static D3DMATRIX MakeIdentity() {
    D3DMATRIX m = {};
    m._11 = m._22 = m._33 = m._44 = 1.0f;
    return m;
}

static D3DMATRIX MakeRotationY(float angle) {
    D3DMATRIX m = MakeIdentity();
    float c = cosf(angle), s = sinf(angle);
    m._11 =  c; m._13 = s;
    m._31 = -s; m._33 = c;
    return m;
}

static D3DMATRIX MakeTranslation(float x, float y, float z) {
    D3DMATRIX m = MakeIdentity();
    m._41 = x; m._42 = y; m._43 = z;
    return m;
}

static D3DMATRIX MatMul(const D3DMATRIX& a, const D3DMATRIX& b) {
    D3DMATRIX r = {};
    const float* A = &a._11;
    const float* B = &b._11;
    float* R = &r._11;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float sum = 0;
            for (int k = 0; k < 4; k++)
                sum += A[i * 4 + k] * B[k * 4 + j];
            R[i * 4 + j] = sum;
        }
    }
    return r;
}

void CubeRenderer::DrawCube(IDirect3DDevice9* device,
                             float x, float y, float z,
                             float angle, D3DCOLOR color, float size) {
    if (!device) return;

    // Save ALL device state using a state block
    IDirect3DStateBlock9* stateBlock = nullptr;
    if (FAILED(device->CreateStateBlock(D3DSBT_ALL, &stateBlock)))
        return;
    stateBlock->Capture();

    // --- Build world matrix: rotate then translate ---
    D3DMATRIX matRot = MakeRotationY(angle);
    D3DMATRIX matTrans = MakeTranslation(x, y, z);
    D3DMATRIX matWorld = MatMul(matRot, matTrans);

    // --- Set up render state for solid colored cube ---
    device->SetTransform(D3DTS_WORLD, &matWorld);

    device->SetFVF(FVF);
    device->SetTexture(0, nullptr);

    device->SetRenderState(D3DRS_LIGHTING, FALSE);
    device->SetRenderState(D3DRS_ZENABLE, TRUE);
    device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    device->SetRenderState(D3DRS_COLORVERTEX, TRUE);

    // Disable texture stage operations (use vertex color only)
    device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
    device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    device->SetVertexShader(nullptr);
    device->SetPixelShader(nullptr);

    // --- Build cube vertices ---
    float s = size;
    float h = size * 2.0f;

    // Per-face shading for visual clarity
    D3DCOLOR top    = color;
    D3DCOLOR bottom = D3DCOLOR_ARGB((color >> 24) & 0xFF,
                                     ((color >> 16) & 0xFF) * 3 / 4,
                                     ((color >> 8) & 0xFF) * 3 / 4,
                                     (color & 0xFF) * 3 / 4);
    D3DCOLOR front  = D3DCOLOR_ARGB((color >> 24) & 0xFF,
                                     ((color >> 16) & 0xFF) * 7 / 8,
                                     ((color >> 8) & 0xFF) * 7 / 8,
                                     (color & 0xFF) * 7 / 8);
    D3DCOLOR back   = D3DCOLOR_ARGB((color >> 24) & 0xFF,
                                     ((color >> 16) & 0xFF) * 5 / 8,
                                     ((color >> 8) & 0xFF) * 5 / 8,
                                     (color & 0xFF) * 5 / 8);
    D3DCOLOR left   = D3DCOLOR_ARGB((color >> 24) & 0xFF,
                                     ((color >> 16) & 0xFF) * 6 / 8,
                                     ((color >> 8) & 0xFF) * 6 / 8,
                                     (color & 0xFF) * 6 / 8);
    D3DCOLOR right  = front;

    // 12 triangles, CW winding (D3D left-handed, D3DCULL_CCW keeps CW front)
    Vertex verts[36] = {
        // Front face (Z = -s)
        {-s, 0, -s, front}, { s, h, -s, front}, { s, 0, -s, front},
        {-s, 0, -s, front}, {-s, h, -s, front}, { s, h, -s, front},
        // Back face (Z = +s)
        { s, 0,  s, back},  {-s, h,  s, back},  {-s, 0,  s, back},
        { s, 0,  s, back},  { s, h,  s, back},  {-s, h,  s, back},
        // Left face (X = -s)
        {-s, 0,  s, left},  {-s, h, -s, left},  {-s, 0, -s, left},
        {-s, 0,  s, left},  {-s, h,  s, left},  {-s, h, -s, left},
        // Right face (X = +s)
        { s, 0, -s, right}, { s, h,  s, right}, { s, 0,  s, right},
        { s, 0, -s, right}, { s, h, -s, right}, { s, h,  s, right},
        // Top face (Y = h)
        {-s, h, -s, top},   {-s, h,  s, top},   { s, h,  s, top},
        {-s, h, -s, top},   { s, h,  s, top},   { s, h, -s, top},
        // Bottom face (Y = 0)
        {-s, 0,  s, bottom},{-s, 0, -s, bottom},{ s, 0, -s, bottom},
        {-s, 0,  s, bottom},{ s, 0, -s, bottom},{ s, 0,  s, bottom},
    };

    device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 12, verts, sizeof(Vertex));

    // --- Wireframe outline ---
    D3DCOLOR wire = D3DCOLOR_ARGB(255, 255, 255, 255);
    Vertex edges[24];
    int ei = 0;
    // Bottom
    edges[ei++] = {-s, 0, -s, wire}; edges[ei++] = { s, 0, -s, wire};
    edges[ei++] = { s, 0, -s, wire}; edges[ei++] = { s, 0,  s, wire};
    edges[ei++] = { s, 0,  s, wire}; edges[ei++] = {-s, 0,  s, wire};
    edges[ei++] = {-s, 0,  s, wire}; edges[ei++] = {-s, 0, -s, wire};
    // Top
    edges[ei++] = {-s, h, -s, wire}; edges[ei++] = { s, h, -s, wire};
    edges[ei++] = { s, h, -s, wire}; edges[ei++] = { s, h,  s, wire};
    edges[ei++] = { s, h,  s, wire}; edges[ei++] = {-s, h,  s, wire};
    edges[ei++] = {-s, h,  s, wire}; edges[ei++] = {-s, h, -s, wire};
    // Verticals
    edges[ei++] = {-s, 0, -s, wire}; edges[ei++] = {-s, h, -s, wire};
    edges[ei++] = { s, 0, -s, wire}; edges[ei++] = { s, h, -s, wire};
    edges[ei++] = { s, 0,  s, wire}; edges[ei++] = { s, h,  s, wire};
    edges[ei++] = {-s, 0,  s, wire}; edges[ei++] = {-s, h,  s, wire};

    device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    device->DrawPrimitiveUP(D3DPT_LINELIST, 12, edges, sizeof(Vertex));

    // Restore ALL device state
    stateBlock->Apply();
    stateBlock->Release();
}
