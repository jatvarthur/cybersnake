#include <iostream>

#include "../engine/include/Network.h"
#include "Server.h"


unsigned short PORT = 27015;

int main()
{
    NetStartup();

    Server server(PORT);
    server.run();

    NetShutdown();
    return 0;
}

/*struct timeval read_timeout;
read_timeout.tv_sec = 0;
read_timeout.tv_usec = 10;
iResult = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (LPCSTR)&read_timeout, sizeof read_timeout);
if (iResult == SOCKET_ERROR) {
    std::cerr << "setsockopt failed with error: " << WSAGetLastError() << std::endl
        << GetErrorMessage(WSAGetLastError()) << std::endl;
}*/