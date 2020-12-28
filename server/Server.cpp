#include "Server.h"
#include "../engine/include/Protocol.h"


ClientProxy::ClientProxy(int clientId, bool player, const InetAddr4& addr)
    : clientId_(clientId)
    , player_(player)
    , addr_(addr)
{
}

bool ClientProxy::isAddr(const InetAddr4& addr) const
{
    return addr_ == addr;
}

bool ClientProxy::isPlayer() const
{
    return player_;
}

int ClientProxy::getClientId() const
{
    return clientId_;
}


Server::Server(u_short port)
    : state_(S_IDLE)
    , listenAddr_(INADDR_ANY, port)
{
}

Server::~Server()
{

}

void Server::run()
{
    socket_.open();

    socket_.bind(listenAddr_);

    while (true) {
        ReceivePackets();
        switch (state_) {
        case S_IDLE:
            HandleIdlePackets();

        }
        /*Update();
        SendPackets();
        */
        Sleep(10);
    }

    socket_.close();
}

void Server::ReceivePackets()
{
    InetAddr4 clientAddr;
    BYTE buffer[4096];
    int bytesRead;
    while ((bytesRead = socket_.recvfrom(buffer, sizeof(buffer), clientAddr)) != 0) {
        BinaryStream stream;
        stream.write(buffer, bytesRead);
        packets_.push_back(std::make_pair(stream, clientAddr));
    }
}

void Server::HandleIdlePackets()
{
    while (!packets_.empty()) {
        auto packet = packets_.front();
        packets_.pop_front();

        int packetType;
        packet.first.read(&packetType, sizeof(packetType));
        packetType = ntohl(packetType);
        switch (packetType)
        {
        case P_HELO:
            ProcessHeloPacket(packet.first, packet.second);
            break;
        default:
            break;
        }
    }
}

void Server::ProcessHeloPacket(BinaryStream& packetData, const InetAddr4& clientAddr)
{
    auto it = std::find_if(clients_.begin(), clients_.end(), [&](ClientProxyPtr& clientPtr) {
        return clientPtr->isAddr(clientAddr);
    });

    bool isGameActive = state_ == S_GAME_ACTIVE;
    bool isPlayer = clients_.empty();
    int clientId;
    if (it != clients_.end()) {
        // send another welcome
        clientId = (*it)->getClientId();
        isPlayer = (*it)->isPlayer();
    }
    else {
        // add
        lastClientId_ += 1;
        clientId = lastClientId_;
        ClientProxyPtr clientPtr = std::make_shared<ClientProxy>(clientId, isPlayer, clientAddr);
        clients_.push_back(clientPtr);

        std::string addrStr = clientAddr.toString();
        fprintf(stdout, "New client from %s: clientId=%d, is %s\n", addrStr.c_str(), clientId,
            isPlayer ? "player" : "observer");
    }

    BinaryStream stream;
    int packetType = htonl(P_WLCM);
    stream.write(&packetType, sizeof(packetType));
    clientId = htonl(clientId);
    stream.write(&clientId, sizeof(clientId));
    stream.write(&isPlayer, sizeof(isPlayer));
    stream.write(&isGameActive, sizeof(isGameActive));

    socket_.sendto(stream.data(), stream.size(), clientAddr);
}
