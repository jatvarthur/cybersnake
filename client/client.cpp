#include <iostream>

#include "../engine/include/Network.h"
#include "../engine/include/GameObject.h"

unsigned short PORT = 27015;


int main()
{
    NetStartup();

    InetAddr4 serverAddr("127.0.0.1", PORT);
    Socket4 sock;
    sock.open();

    char buf[] = "HELO";
    sock.sendto(buf, sizeof(buf), serverAddr);
    sock.close();

    NetShutdown();
    return 0;
}

/*
	GameObject go1, go2;

	go1.setName("Snake");
	go1.setLoc({ 10, 15 });

	BinaryStream bs;
	Serializer serial;

	serial.serialize(&go1, &bs);
	bs.rewind();
	serial.deserialize(&go2, &bs);

*/