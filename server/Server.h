#pragma once
#ifndef SERVER_H__
#define SERVER_H__

#include "../engine/include/Network.h"
#include "../engine/include/GameObject.h"


enum ServerState {
	S_IDLE,
	S_GAME_ACTIVE
};


class ClientProxy {
public:
	ClientProxy(int clientId, bool player, const InetAddr4& addr);

	bool isAddr(const InetAddr4& addr) const;
	bool isPlayer() const;
	int getClientId() const;

private:
	InetAddr4 addr_;
	int clientId_;
	bool player_;
};

typedef std::shared_ptr<ClientProxy> ClientProxyPtr;

class Server {
public:
	Server(u_short port);
	~Server();

	void run();

protected:
	void ReceivePackets();
	void HandleIdlePackets();

	void ProcessHeloPacket(BinaryStream &packetData, const InetAddr4 &clientAddr);


private:
	InetAddr4 listenAddr_;
	Socket4 socket_;
	enum ServerState state_;
	std::deque<std::pair<BinaryStream, InetAddr4>> packets_;

	int lastClientId_;
	std::vector<ClientProxyPtr> clients_;
};

#endif
