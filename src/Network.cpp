/*
Copyright © 2012 Igor Paliychuk

This file is part of FLARE.

FLARE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

FLARE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FLARE.  If not, see http://www.gnu.org/licenses/

IpAddress -- ClientSocket classes are based on SDL NET tutorial from www.sdltutorials.com. Thanks
*/

#include "SharedResources.h"

//---------IpAddress Class------------//

IpAddress::IpAddress (Uint16 port) {
	if (SDLNet_ResolveHost(&m_Ip, NULL, port) < 0){
		fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		m_Ip.host = 0;
		m_Ip.port = 0;
	}
}


IpAddress::IpAddress (std::string host, Uint16 port) {
	if (SDLNet_ResolveHost(&m_Ip, host.c_str(), port) < 0) {
		fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		m_Ip.host = 0;
		m_Ip.port = 0;
	}
}

IpAddress::IpAddress() {
	m_Ip.host = 0;
	m_Ip.port = 0;
}

bool IpAddress::Ok() const {
	return !(m_Ip.port == 0);
}

void IpAddress::SetIp (IPaddress sdl_ip) {
	m_Ip = sdl_ip;
}

IPaddress IpAddress::GetIpAddress() const {
	return m_Ip;
}

Uint32 IpAddress::GetHost() const {
	return m_Ip.host;
}

Uint16 IpAddress::GetPort() const {
	return m_Ip.port;
}

//---------TCPSocket Class------------//

TcpSocket::TcpSocket() {
	m_Socket = NULL;
	set = SDLNet_AllocSocketSet(1);
}

TcpSocket::~TcpSocket() {
	if (!(m_Socket == NULL)) {
		SDLNet_TCP_DelSocket(set,m_Socket);
		SDLNet_FreeSocketSet(set);
		SDLNet_TCP_Close (m_Socket);
	}
}

void TcpSocket::SetSocket (TCPsocket the_sdl_socket) {
	if (!(m_Socket == NULL)) {
		SDLNet_TCP_DelSocket(set,m_Socket);
		SDLNet_TCP_Close (m_Socket);
	}
	m_Socket = the_sdl_socket;
	if(SDLNet_TCP_AddSocket(set,m_Socket)==-1) {
		fprintf(stderr, "SDLNet_TCP_AddSocket: %s\n", SDLNet_GetError());
		m_Socket = NULL;
	}
}

bool TcpSocket::Ok() const {
	return !(m_Socket == NULL);
}

bool TcpSocket::Ready() const {
	bool handle = false;
	int numready = SDLNet_CheckSockets(set, 0);
	if (numready == -1)
			fprintf(stderr, "SDLNet_CheckSockets: %s\n", SDLNet_GetError());
	else
			if (numready)
				handle = SDLNet_SocketReady (m_Socket);
	return handle;
}

//---------HostSocket Class------------//

HostSocket::HostSocket (IpAddress& the_ip_address) {
	TcpSocket();
	IPaddress iph = the_ip_address.GetIpAddress();
	if (!(m_Socket = SDLNet_TCP_Open(&iph))) {
		SDLNet_FreeSocketSet(set);
			fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
	}
}

HostSocket::HostSocket (Uint16 port) {
	IpAddress iplistener (port);
	if (!iplistener.Ok()) {
		m_Socket = NULL;
	}
	else {
		TcpSocket();
		IPaddress iph = iplistener.GetIpAddress();
		if (!(m_Socket = SDLNet_TCP_Open(&iph))) {
			SDLNet_FreeSocketSet(set);
			fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		}
	}
}


bool HostSocket::Accept (ClientSocket& the_client_socket) {
	TCPsocket cs;
	if ((cs = SDLNet_TCP_Accept(m_Socket))) {
		the_client_socket.SetSocket(cs);
		return true;
	}
	else
		return false;
}

//---------ClientSocket Class------------//

ClientSocket::ClientSocket() {
    TcpSocket();
}

ClientSocket::ClientSocket (std::string host, Uint16 port) {
	IpAddress remoteip (host.c_str(), port);
	if (!remoteip.Ok()) {
		m_Socket = NULL;
	}
	else {
		TcpSocket();
		Connect(remoteip);
	}
}

bool ClientSocket::Connect (HostSocket& the_listener_socket) {
	return false;
}

bool ClientSocket::Connect (IpAddress& remoteip) {
	TCPsocket cs;
	IPaddress ip = remoteip.GetIpAddress();
	if ((cs = SDLNet_TCP_Open(&ip)))
	{
		SetSocket(cs);
		return true;
	}
	else {
		fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		return false;
	}
}

void ClientSocket::SetSocket (TCPsocket the_sdl_socket) {
	TcpSocket::SetSocket (the_sdl_socket);
	IPaddress* ips;
	if ((ips = SDLNet_TCP_GetPeerAddress(m_Socket))) {
	/* Print the address, converting it onto the host format */
		m_RemoteIp.SetIp(*ips);
		Uint32 hbo = m_RemoteIp.GetHost();
		Uint16 pbo = m_RemoteIp.GetPort();
		printf("Client connected: %u %u\n", SDLNet_Read32(&hbo), SDLNet_Read16 (&pbo));
	}
	else
		fprintf(stderr, "SDLNet_TCP_GetPeerAddress: %s\n", SDLNet_GetError());
}

IpAddress ClientSocket::getIpAddress () const {
	return m_RemoteIp;
}

bool ClientSocket::Receive(DataPacket* rData) {
	if (m_Socket == NULL)
		return false;
		
	int len = sizeof(DataPacket);

	if (SDLNet_TCP_Recv(m_Socket, (void *)rData, len) <= 0)
		return false;

	return true;
}

bool ClientSocket::Send (DataPacket* sData) {
	if (m_Socket == NULL)
		return false;
		
	int len = sizeof(DataPacket);

	if (SDLNet_TCP_Send(m_Socket, (void *)sData, len) < len) {
		fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
		return false;
		}
	return true;
}

//==============================================================================

Multiplayer::Multiplayer() {

	CurrentPlayer = 0;
	Connected = false;

	Init();

	if (isHost) startServer(1234);
	else connectToServer("127.0.0.1", 1234);
	
	//DataPacket packet_to_send;
}

Multiplayer::~Multiplayer() {

	delete tcpclient;
	if (isHost) delete tcplistener;
	else delete remoteip;
}

bool Multiplayer::Init() {
	if (SDLNet_Init() < 0) {
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
		return false;
		}
		else
			return true;
}

void Multiplayer::Quit() {
	SDLNet_Quit();
}

void Multiplayer::startServer(int port) {

	tcplistener = NULL;
	tcpclient = NULL;

	tcplistener = new HostSocket (port);
	if (!tcplistener->Ok())
		exit(EXIT_FAILURE);

	tcpclient = new ClientSocket();
}

void Multiplayer::connectToServer(std::string ip, int port) {

	tcpclient = NULL;
	remoteip = NULL;

	tcpclient = new ClientSocket ();
	remoteip = new IpAddress (ip, port);
}

void Multiplayer::MultiplayerLoop() {

	if (!multiplayer) return;

	if(isHost) serverLoop();
	else clientLoop();
}

void Multiplayer::serverLoop() {

	DataPacket* packet_get = new DataPacket;

	if (!Connected) {
		if (tcplistener->Accept (*tcpclient)) {
			Connected = true;
		}
	}
	else {
		if (tcpclient->Ready()) {
			if (tcpclient->Receive (packet_get)) {
				printf("Received hero position %d, %d\n", packet_get->hero_pos_x, packet_get->hero_pos_y);
				setEntityStatus(packet_get->hero_pos_x, packet_get->hero_pos_y);//(new_X, new_Y, character, action);
			}
			else
				Connected = false;
		}
	}
	
	delete packet_get;
}

void Multiplayer::clientLoop() {

	DataPacket* packet_get = new DataPacket;

	if (!Connected) {
		if (tcpclient->Connect(*remoteip)) {
			if (tcpclient->Ok()) {
				Connected = true;
			}
		}
	}
	else {
		if (tcpclient->Ready()){
			if (tcpclient->Receive (packet_get)) {
				printf("Received hero position %d, %d\n", packet_get->hero_pos_x, packet_get->hero_pos_y);
				setEntityStatus(packet_get->hero_pos_x, packet_get->hero_pos_y);//(new_X, new_Y, character, action);
			}
			else {
				Connected = false;
			}
		}
	}
	
	delete packet_get;
}

/**
 * Get new Entity position and transfer it to remote server/client
 */
void Multiplayer::statusHandler(int new_x, int new_y) {

	DataPacket* packet_to_send = new DataPacket;

	packet_to_send->hero_pos_x = new_x;
	packet_to_send->hero_pos_y = new_y;

	//Handle client
	if (!isHost) {
		setEntityStatus(packet_to_send->hero_pos_x, packet_to_send->hero_pos_y);
		printf("Send hero position %d, %d\n", packet_to_send->hero_pos_x, packet_to_send->hero_pos_y);
		tcpclient->Send(packet_to_send);
	} else

	//Handle server
	if (Connected) {
		setEntityStatus(packet_to_send->hero_pos_x, packet_to_send->hero_pos_y);
		printf("Send hero position %d, %d\n", packet_to_send->hero_pos_x, packet_to_send->hero_pos_y);
		tcpclient->Send(packet_to_send);
	}

	delete packet_to_send;
}

/**
 * Set received Entity status/position
 */
void Multiplayer::setEntityStatus(int recv_x, int recv_y) {

	//UNIMPLEMENTED
}
