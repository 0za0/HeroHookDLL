#include "MultiplayerNet.h"

// ============================================================================
// MultiplayerNet implementation
//
// UDP peer-to-peer networking. Non-blocking sockets used throughout so
// ReceiveUpdate() never stalls the game's render thread.
//
// Host flow:  Host(port) -> bind socket -> wait for first packet (learns peer addr)
// Join flow:  Join(ip, port) -> set peer addr -> start sending immediately
// Both sides: SendPosition() + ReceiveUpdate() called each frame from Present hook
// ============================================================================

MultiplayerNet::MultiplayerNet() {}

MultiplayerNet::~MultiplayerNet() {
    Disconnect();
    if (m_wsaInitialized) {
        WSACleanup();
        m_wsaInitialized = false;
    }
}

bool MultiplayerNet::InitWinsock() {
    if (m_wsaInitialized) return true;
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return false;
    m_wsaInitialized = true;
    return true;
}

void MultiplayerNet::CleanupSocket() {
    if (m_socket != INVALID_SOCKET) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
    m_hasPeerAddr = false;
    m_packetsSent = 0;
    m_packetsRecv = 0;
    m_sequence = 0;
    m_remote = {};
}

bool MultiplayerNet::Host(uint16_t port) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_state != NetState::Disconnected)
        Disconnect();

    if (!InitWinsock()) return false;

    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socket == INVALID_SOCKET) return false;

    // Set non-blocking
    u_long mode = 1;
    ioctlsocket(m_socket, FIONBIO, &mode);

    // Bind to port
    sockaddr_in bindAddr = {};
    bindAddr.sin_family = AF_INET;
    bindAddr.sin_port = htons(port);
    bindAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_socket, (sockaddr*)&bindAddr, sizeof(bindAddr)) == SOCKET_ERROR) {
        CleanupSocket();
        return false;
    }

    m_state = NetState::Hosting;
    m_hasPeerAddr = false; // Will learn peer address from first received packet
    return true;
}

bool MultiplayerNet::Join(const std::string& ip, uint16_t port) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_state != NetState::Disconnected)
        Disconnect();

    if (!InitWinsock()) return false;

    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socket == INVALID_SOCKET) return false;

    // Set non-blocking
    u_long mode = 1;
    ioctlsocket(m_socket, FIONBIO, &mode);

    // Bind to any port (OS assigns ephemeral port)
    sockaddr_in bindAddr = {};
    bindAddr.sin_family = AF_INET;
    bindAddr.sin_port = 0;
    bindAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(m_socket, (sockaddr*)&bindAddr, sizeof(bindAddr)) == SOCKET_ERROR) {
        CleanupSocket();
        return false;
    }

    // Set peer address
    m_peerAddr = {};
    m_peerAddr.sin_family = AF_INET;
    m_peerAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &m_peerAddr.sin_addr);
    m_hasPeerAddr = true;

    m_state = NetState::Joining;
    return true;
}

void MultiplayerNet::Disconnect() {
    // Note: caller may or may not hold m_mutex
    CleanupSocket();
    m_state = NetState::Disconnected;
}

void MultiplayerNet::SendPosition(float x, float y, float z, float angle, uint32_t flags) {
    if (m_socket == INVALID_SOCKET || !m_hasPeerAddr) return;

    PlayerPacket pkt;
    pkt.magic = MP_MAGIC;
    pkt.sequence = m_sequence++;
    pkt.x = x;
    pkt.y = y;
    pkt.z = z;
    pkt.angle = angle;
    pkt.flags = flags;

    sendto(m_socket, (const char*)&pkt, sizeof(pkt), 0,
           (sockaddr*)&m_peerAddr, sizeof(m_peerAddr));
    m_packetsSent++;
}

bool MultiplayerNet::ReceiveUpdate() {
    if (m_socket == INVALID_SOCKET) return false;

    PlayerPacket pkt;
    sockaddr_in fromAddr = {};
    int fromLen = sizeof(fromAddr);
    bool gotNew = false;

    // Drain all pending packets, keep the latest
    while (true) {
        int bytes = recvfrom(m_socket, (char*)&pkt, sizeof(pkt), 0,
                             (sockaddr*)&fromAddr, &fromLen);
        if (bytes <= 0) break;
        if (bytes < (int)sizeof(PlayerPacket)) continue;
        if (pkt.magic != MP_MAGIC) continue;

        // If hosting and don't have peer yet, learn from first packet
        if (!m_hasPeerAddr) {
            m_peerAddr = fromAddr;
            m_hasPeerAddr = true;
        }

        // Accept packet (use latest sequence)
        m_remote.x = pkt.x;
        m_remote.y = pkt.y;
        m_remote.z = pkt.z;
        m_remote.angle = pkt.angle;
        m_remote.flags = pkt.flags;
        m_remote.lastRecvTime = GetTickCount();
        m_remote.lastSequence = pkt.sequence;
        m_remote.valid = true;
        m_packetsRecv++;
        gotNew = true;

        // Upgrade state to Connected
        if (m_state == NetState::Hosting || m_state == NetState::Joining)
            m_state = NetState::Connected;
    }

    // Check for timeout
    if (m_remote.valid) {
        DWORD elapsed = GetTickCount() - m_remote.lastRecvTime;
        if (elapsed > MP_TIMEOUT_MS) {
            m_remote.valid = false;
            if (m_state == NetState::Connected) {
                // Revert to waiting state
                m_state = m_hasPeerAddr ? NetState::Joining : NetState::Hosting;
            }
        }
    }

    return gotNew;
}

bool MultiplayerNet::IsPeerConnected() const {
    return m_state == NetState::Connected && m_remote.valid;
}

std::string MultiplayerNet::GetStatusText() const {
    switch (m_state) {
    case NetState::Disconnected: return "Disconnected";
    case NetState::Hosting:      return "Hosting (waiting for peer...)";
    case NetState::Joining:      return "Joining (waiting for response...)";
    case NetState::Connected: {
        char buf[128];
        sprintf_s(buf, "Connected (sent:%u recv:%u)", m_packetsSent, m_packetsRecv);
        return buf;
    }
    default: return "Unknown";
    }
}
