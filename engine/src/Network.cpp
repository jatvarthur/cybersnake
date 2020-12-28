#include "Network.h"


std::string GetErrorMessage(int errcode)
{
    LPSTR buffer = NULL;
    DWORD dwChars = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL,
        errcode,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPSTR)&buffer,
        0,
        NULL
    );

    std::string result;
    if (dwChars > 0) {
        result.assign(buffer);
    }
    if (buffer) {
        LocalFree(buffer);
    }
    return result;
}


int NetStartup()
{
    WSADATA wsaData;

    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed with error: " << iResult << std::endl;
        std::cerr << "  " << GetErrorMessage(iResult) << std::endl;
        return 0;
    }
    return 1;
}

void NetShutdown()
{
    WSACleanup();
}


InetAddr4::InetAddr4()
{
    addr_.sin_family = AF_INET;
}

InetAddr4::InetAddr4(u_long address, u_short port)
{
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = htonl(address);
}

InetAddr4::InetAddr4(const char* address, u_short port)
{
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    inet_pton(AF_INET, address, &addr_.sin_addr);
}

const sockaddr* InetAddr4::saddr() const
{
    return (sockaddr*)&addr_; 
}

sockaddr* InetAddr4::saddr()
{ 
    return (sockaddr*)&addr_;
}

int InetAddr4::size() const
{
    return sizeof(addr_);
}

std::string InetAddr4::toString() const
{
    char buf[128];
    inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
    int len = strlen(buf);
    snprintf(buf + len, sizeof(buf) - len, ":%d", ntohs(addr_.sin_port));
    return std::string(buf);
}

bool operator==(const InetAddr4& lhs, const InetAddr4& rhs)
{
    return lhs.addr_.sin_family == rhs.addr_.sin_family
        && lhs.addr_.sin_addr.s_addr == rhs.addr_.sin_addr.s_addr
        && lhs.addr_.sin_port == rhs.addr_.sin_port;
}



/*struct timeval read_timeout;
read_timeout.tv_sec = 0;
read_timeout.tv_usec = 10;
fd_set readfds = { 0 };
FD_SET(socket_, &readfds);
int result = select(0, &readfds, nullptr, nullptr, &read_timeout);

if (result > 0) {
    int senderAddrLen = sizeof(addrFrom);
    result = ::recvfrom(socket_,
        (char*)buffer, bufferSize, 0, (SOCKADDR*)&addrFrom, &senderAddrLen);
    if (result == SOCKET_ERROR) {
        lastError_ = WSAGetLastError();
        std::cerr << "recvfrom failed with error: " << lastError_ << std::endl
            << GetErrorMessage(lastError_) << std::endl;
        result = 0;
    }
}*/
