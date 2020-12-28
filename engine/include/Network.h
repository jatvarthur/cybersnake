#pragma once
#ifndef NETWORK_H__
#define NETWORK_H__

#include "system.h"

std::string GetErrorMessage(int errcode);

int NetStartup();
void NetShutdown();


class InetAddr4 {
	friend bool operator==(const InetAddr4& lhs, const InetAddr4& rhs);

public:
	enum { Family = AF_INET };

public:
	// uninitialized IPv4 addr
	InetAddr4();
	// arbitrary IPv4 addr, in host byte ordr
	InetAddr4(u_long address, u_short port);
	// string dotted format X.X.X.X IPv4 addr
	InetAddr4(const char* address, u_short port);

	const sockaddr* saddr() const;
	sockaddr* saddr();
	int size() const;
	std::string toString() const;

private:
	struct sockaddr_in addr_;
};

bool operator==(const InetAddr4& lhs, const InetAddr4& rhs);

typedef InetAddr4 InetAddrT;

template<typename Addr>
class Socket {
public:
	Socket();
	virtual ~Socket();

	bool open();
	void close();
	bool bind(const Addr& addr);
	int sendto(const void* buffer, int bufferSize, const Addr& addrTo);
	int recvfrom(void* buffer, int bufferSize, Addr& addrFrom);

	bool isOpened() const;
	int getLastError() const;

protected:
	void etrace(const char* fn, int lineNo) const;

private:
	SOCKET socket_;
	int lastError_;
};

typedef Socket<InetAddr4> Socket4;
typedef Socket<InetAddrT> SocketT;

#include "../src/Socket.hpp"

/*class Packet {
public:
	Packet();
	~Packet();


private:
	std::vector<BYTE> buffer_;
};*/

#endif