# Bionicle Heroes - Multiplayer Mod

## Overview

A peer-to-peer multiplayer mod for Bionicle Heroes (2006, PC) that shows a second player as a colored cube in the game world. Built as an extension to HeroHookDLL (binkw32.dll proxy).

## Architecture

```
+------------------+     UDP (28-byte packets)     +------------------+
|  Player 1 (Host) |  <========================>  |  Player 2 (Join) |
|                   |                               |                   |
|  Read local pos   |    Send: {x,y,z,angle,flags}  |  Read local pos   |
|  Draw remote cube |    Recv: {x,y,z,angle,flags}  |  Draw remote cube |
+------------------+                               +------------------+
```

### Components

| File | Purpose |
|------|---------|
| `MultiplayerNet.h/cpp` | UDP networking - Winsock2 sockets, non-blocking send/recv |
| `CubeRenderer.h/cpp` | D3D9 drawing - colored cube with wireframe at world position |
| `Multiplayer.h/cpp` | Manager - coordinates networking + rendering + ImGui UI |

### Data Flow (per frame, in Present hook)

1. **Read position**: `getCoordinates()` reads player XYZ from game memory
2. **Send**: `MultiplayerNet::SendPosition()` sends UDP packet to peer
3. **Receive**: `MultiplayerNet::ReceiveUpdate()` drains pending packets (non-blocking)
4. **Draw**: `CubeRenderer::DrawCube()` renders cube at remote player position

## Network Protocol

### Packet Format (28 bytes, little-endian)

```
Offset  Type      Field     Description
0x00    uint32    magic     0x4248504B ("BHPK")
0x04    uint32    sequence  Incrementing packet number
0x08    float     x         Player X position
0x0C    float     y         Player Y position (up axis)
0x10    float     z         Player Z position
0x14    float     angle     Player facing angle (radians)
0x18    uint32    flags     Bit 0: alive, Bit 1: in level
```

### Connection Model

- **Host**: Binds UDP socket to port (default 27015), waits for first packet to learn peer address
- **Join**: Creates UDP socket, sends to host IP:port
- **Timeout**: Peer considered disconnected after 3 seconds of no packets
- **No handshake**: First valid BHPK packet establishes connection

## Game Memory Layout

Player position is read from game memory via pointer chain:

```
ExeBaseAddress + 0x252CBC  ->  pointer P
P + 0x6E0  =  float X      (right)
P + 0x6E4  =  float Z      (depth)  [game calls this Z]
P + 0x6E8  =  float Y      (up)     [game calls this Y]
```

This is the same path used by the existing Coordinate Manipulator feature.

## D3D9 Rendering

The cube is drawn using `DrawPrimitiveUP` (no vertex/index buffers) with the fixed-function pipeline:

1. Create D3D9 state block to save ALL current state
2. Build world matrix: `RotationY(angle) * Translation(x, y, z)`
3. Set render states: no lighting, Z-test enabled, alpha blending, no texture
4. Set FVF: `D3DFVF_XYZ | D3DFVF_DIFFUSE` (position + vertex color)
5. Draw 12 triangles (solid cube) + 12 lines (wireframe edges)
6. Restore state block

View and Projection matrices are NOT modified - the game has already set them for the current frame, so `SetTransform(D3DTS_WORLD, ...)` is sufficient.

## Building

```
MSBuild HeroHookDLL.sln /p:Configuration=Release /p:Platform=x86
```

Output: `Release/binkw32.dll` - copy to game directory (alongside `binkw32_original.dll`).

**Requirements**: Visual Studio 2022, DirectX SDK (June 2010), Windows SDK.

## Usage

1. Copy built `binkw32.dll` to game directory
2. Launch Bionicle Heroes
3. Press F12 to open HeroHookDLL overlay
4. Go to Windows > Multiplayer
5. **To host**: Click "Host Game" (default port 27015)
6. **To join**: Enter host's IP address, click "Join Game"
7. Load into any level - remote player appears as a colored cube

### ImGui UI Features

- Host/Join/Disconnect buttons
- IP address and port input
- Connection status with packet counters
- Remote player position display
- Cube customization: size, color, opacity

## Extending

### Adding more player data

Add fields to `PlayerPacket` (keep `magic` first, update `sizeof` check):
```cpp
struct PlayerPacket {
    uint32_t magic;
    uint32_t sequence;
    float x, y, z, angle;
    uint32_t flags;
    // Add new fields here:
    uint8_t heroId;     // which character they're playing
    float health;       // health bar
};
```

### Replacing cube with game model

To render an actual character model instead of a cube:
1. Load an HGP file into D3D9 vertex/index buffers
2. Replace `CubeRenderer::DrawCube()` with proper mesh rendering
3. The world matrix setup and state save/restore pattern stays the same

### Adding more than 2 players

Change from single `RemotePlayer` to `std::vector<RemotePlayer>`:
1. Add player ID to `PlayerPacket`
2. Store multiple remote players in `MultiplayerNet`
3. Draw a cube for each in `Multiplayer::Update()`
