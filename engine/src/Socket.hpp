
template<typename Addr>
Socket<Addr>::Socket()
    : socket_(INVALID_SOCKET)
    , lastError_(0)
{
}

template<typename Addr>
Socket<Addr>::~Socket()
{
    if (socket_ != INVALID_SOCKET) {
        close();
    }
}

#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)

template<typename Addr>
bool Socket<Addr>::open()
{
    lastError_ = 0;
    socket_ = socket(Addr::Family, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_ == INVALID_SOCKET) {
        lastError_ = WSAGetLastError();
        etrace("socket", __LINE__);
        return false;
    }

    u_long mode = 1; // non-blocking mode enabled
    int result = ioctlsocket(socket_, FIONBIO, &mode);
    if (result != NO_ERROR) {
        lastError_ = result;
        etrace("ioctlsocket", __LINE__);
    }
    mode = 0; // no WSAECONNRESET for UPD sockets
    result = ioctlsocket(socket_, SIO_UDP_CONNRESET, &mode);
    if (result != NO_ERROR) {
        lastError_ = result;
        etrace("ioctlsocket", __LINE__);
    }
    return socket_ != INVALID_SOCKET;
}

template<typename Addr>
void Socket<Addr>::close()
{
    if (socket_ != INVALID_SOCKET) {
        closesocket(socket_);
        socket_ = INVALID_SOCKET;
    }
    lastError_ = 0;
}

template<typename Addr>
bool Socket<Addr>::bind(const Addr& addr)
{
    assert(socket_ != INVALID_SOCKET);

    lastError_ = 0;
    int result = ::bind(socket_, addr.saddr(), addr.size());
    if (result == SOCKET_ERROR) {
        lastError_ = WSAGetLastError();
        etrace("bind", __LINE__);
        return false;
    }
    return true;
}

template<typename Addr>
int Socket<Addr>::sendto(const void* buffer, int bufferSize, const Addr& addrTo)
{
    assert(socket_ != INVALID_SOCKET);

    lastError_ = 0;
    int result = ::sendto(socket_,
        (char*)buffer, bufferSize, 0, addrTo.saddr(), addrTo.size());
    if (result == SOCKET_ERROR) {
        lastError_ = WSAGetLastError();
        etrace("sendto", __LINE__);
    }
    return result;
}

template<typename Addr>
int Socket<Addr>::recvfrom(void* buffer, int bufferSize, Addr& addrFrom)
{
    assert(socket_ != INVALID_SOCKET);

    lastError_ = 0;
    int addrFromLen = addrFrom.size();
    int result = ::recvfrom(socket_,
        (char*)buffer, bufferSize, 0, addrFrom.saddr(), &addrFromLen);
    if (result == SOCKET_ERROR) {
        lastError_ = WSAGetLastError();
        result = 0;
        switch (lastError_) {
        case WSAEWOULDBLOCK:
            // no data to read
            lastError_ = 0;
            break;
        case WSAECONNRESET:
            // Windows ICMP reports client disconnects for UDP, 
            // which is weird and may not desirable, we turn off this
            // feature in sockopts. Or, here we can attempt to report 
            // disconnects and remove clients
        default:
            etrace("recvfrom", __LINE__);
        }
    }

    return result;
}

template<typename Addr>
bool Socket<Addr>::isOpened() const
{
    return socket_ != INVALID_SOCKET;
}

template<typename Addr>
int Socket<Addr>::getLastError() const
{
    return lastError_;
}

template<typename Addr>
void Socket<Addr>::etrace(const char* fn, int lineNo) const
{
    std::string message = GetErrorMessage(lastError_);
    fprintf(stderr, "%s(:%d) failed: %d\n  %s\n",
        fn, lineNo, lastError_, message.c_str());
}
