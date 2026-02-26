#pragma once

// winsock2.h MUST be included before windows.h to avoid winsock.h conflicts.
// If windows.h was already included (without WIN32_LEAN_AND_MEAN), ensure
// the translation unit includes winsock2.h first (see dllmain.cpp).
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <cstdint>
#include <string>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

// ============================================================================
// MultiplayerNet - UDP peer-to-peer networking for Bionicle Heroes multiplayer
// ============================================================================
//
// Protocol: Simple UDP packets containing player state.
// Architecture: Peer-to-peer. One player "hosts" (binds to port), the other
//               "joins" (sends to host IP:port). Both send and receive.
//
// Packet format (28 bytes):
//   [0x00] uint32_t magic     = 0x4248504B ("BHPK" - Bionicle Heroes PacKet)
//   [0x04] uint32_t sequence  = incrementing packet number
//   [0x08] float    x         = player X position
//   [0x0C] float    y         = player Y position (up)
//   [0x10] float    z         = player Z position
//   [0x14] float    angle     = player facing angle (radians)
//   [0x18] uint32_t flags     = bit 0: alive, bit 1: in-level
//
// Coordinate system: Same as game memory (D3D left-handed)
//   ExeBase+0x252CBC -> ptr -> +0x6E0=X, +0x6E4=Z, +0x6E8=Y
//   Note: game stores Z at +0x6E4, Y(up) at +0x6E8
// ============================================================================

constexpr uint32_t MP_MAGIC = 0x4248504B; // "BHPK"
constexpr uint16_t MP_DEFAULT_PORT = 27015;
constexpr int MP_TIMEOUT_MS = 3000; // peer considered disconnected after 3s

#pragma pack(push, 1)
struct PlayerPacket {
    uint32_t magic    = MP_MAGIC;
    uint32_t sequence = 0;
    float    x        = 0.0f;
    float    y        = 0.0f;
    float    z        = 0.0f;
    float    angle    = 0.0f;
    uint32_t flags    = 0;      // bit 0: alive, bit 1: in level
};
#pragma pack(pop)
static_assert(sizeof(PlayerPacket) == 28, "PlayerPacket must be 28 bytes");

enum class NetState {
    Disconnected,
    Hosting,       // Listening for a peer
    Joining,       // Connecting to a host
    Connected      // Peer is active (receiving packets)
};

struct RemotePlayer {
    float x = 0, y = 0, z = 0;
    float angle = 0;
    uint32_t flags = 0;
    DWORD lastRecvTime = 0;     // GetTickCount() of last packet
    uint32_t lastSequence = 0;
    bool valid = false;         // true if we've received at least one packet
};

class MultiplayerNet {
public:
    MultiplayerNet();
    ~MultiplayerNet();

    // Start hosting on the given port. Binds UDP socket.
    bool Host(uint16_t port = MP_DEFAULT_PORT);

    // Join a host at ip:port. Creates UDP socket, sets peer address.
    bool Join(const std::string& ip, uint16_t port = MP_DEFAULT_PORT);

    // Disconnect and clean up socket.
    void Disconnect();

    // Send local player state. Call once per frame.
    void SendPosition(float x, float y, float z, float angle, uint32_t flags);

    // Receive remote player state (non-blocking). Call once per frame.
    // Returns true if new data was received.
    bool ReceiveUpdate();

    // Get current state
    NetState GetState() const { return m_state; }
    const RemotePlayer& GetRemotePlayer() const { return m_remote; }
    bool IsPeerConnected() const;
    uint32_t GetPacketsSent() const { return m_packetsSent; }
    uint32_t GetPacketsRecv() const { return m_packetsRecv; }
    std::string GetStatusText() const;

private:
    bool InitWinsock();
    void CleanupSocket();

    SOCKET m_socket = INVALID_SOCKET;
    sockaddr_in m_peerAddr = {};
    bool m_hasPeerAddr = false;

    NetState m_state = NetState::Disconnected;
    RemotePlayer m_remote;
    uint32_t m_sequence = 0;
    uint32_t m_packetsSent = 0;
    uint32_t m_packetsRecv = 0;

    bool m_wsaInitialized = false;
    mutable std::mutex m_mutex;
};
