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

NetMessage -- ClientSocket classes are based on SDL NET tutorial from www.sdltutorials.com. Thanks
*/

#include "Network.h"

//---------NetMessage Class------------//

NetMessage::NetMessage() {
	reset();
}

void NetMessage::reset() {
	for (int i =0; i < 256; i++)
		buffer [i] = 0;
	state = EMPTY;
}

void NetMessage::finish() {
	if (state == READING)
		state = FULL;
}

int NetMessage::NumToLoad() {
	if (state == EMPTY)
		return 255;
	else
		return 0;
}

int NetMessage::NumToUnLoad() {
	if (state == FULL)
		return strlen(buffer) + 1;
	else
		return 0;
}

void NetMessage::LoadBytes(charbuf& inputbuffer, int n) {
	for (int i = 0; i < n; i++)
		buffer[i] = inputbuffer[i];
	state = READING;
}

void NetMessage::UnLoadBytes(charbuf& destbuffer) {
	for (int i=0; i < this->NumToUnLoad(); i++)
		destbuffer[i] = buffer[i];
	reset();
}

void NetMessage::LoadByte(char ID) {
	charbuf c;
	c[0] = ID;
	LoadBytes(c, 1);
	finish();
}

char NetMessage::UnLoadByte() {
	charbuf c;
	UnLoadBytes (c);
	return c[0];
}

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

bool ClientSocket::Receive(NetMessage& rData) {
//Firstly, check if there is a socket
	if (m_Socket == NULL)
		return false;
	charbuf buf;

//Check if the instance can receive bytes, if it can, load the number of bytes specfied by NumToLoad() virtual function
	while (rData.NumToLoad() > 0) {
		if (SDLNet_TCP_Recv(m_Socket, buf, rData.NumToLoad()) > 0) {
			rData.LoadBytes (buf, rData.NumToLoad());
		}
		else {
			return false;
		}
	}
	rData.finish();
	return true;
}

bool ClientSocket::Send (NetMessage& sData) {
	if (m_Socket == NULL)
		return false;
	charbuf buf;
	int len;

//Check if the instance can send bytes, if it can, unload the number of bytes specfied by NumToLoad()
	while ((len = sData.NumToUnLoad()) > 0) {
		sData.UnLoadBytes (buf);
		if (SDLNet_TCP_Send(m_Socket, (void *)buf, len) < len) {
			fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
			return false;
		}
	}
	return true;
}

//==============================================================================

Multiplayer::Multiplayer() {
	Connected = false;
	tcplistener = NULL;
	tcpclient = NULL;
	remoteip = NULL;
}

void Multiplayer::Cleanup() {

	delete tcplistener;
	delete tcpclient;
	delete remoteip;
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
	tcplistener = new HostSocket (port);
	if (!tcplistener->Ok())
		exit(EXIT_FAILURE);

	tcpclient = new ClientSocket();
}

void Multiplayer::connectToServer(std::string ip, int port) {
	tcpclient = new ClientSocket ();
	remoteip = new IpAddress (ip, port);
}

void Multiplayer::serverOnLoop() {
	if (!Connected) {
		if (tcplistener->Accept (*tcpclient)) {
			Connected = true;
		}
	}
	else {
		printf("Client is connected\n");
		//FIXME Connected, but tcpclient is not Ready
		if (tcpclient->Ready()) {
			if (tcpclient->Receive (msg)) {
				setEntityStatus();
				printf("Client reported its new X position %d\n", msg.UnLoadByte());
				CurrentPlayer = 0;
			}
			else
				Connected = false;
		}
	}
}

void Multiplayer::clientOnLoop() {
	if (!Connected) {
		if (tcpclient->Connect(*remoteip)) {
			if (tcpclient->Ok()) {
				Connected = true;
			}
		}
	}
	else {
		printf("Client is connected\n");
		//FIXME Connected, but tcpclient is not Ready
		if (tcpclient->Ready()){
			if (tcpclient->Receive (msg)) {
				setEntityStatus();
				printf("Server reported its new X position %d\n", msg.UnLoadByte());
				CurrentPlayer = 1;
			}
			else {
				Connected = false;
			}
		}
	}
}

/**
 * Get new Entity position and transfer it to remote server/client
 */
void Multiplayer::statusHandler(int new_x, int new_y) {
	//Handle client
	//Player == 0 is always Server.
	if(CurrentPlayer != 0) {
		setEntityStatus();//(new_X, new_Y, character, action);
		CurrentPlayer = 0;
		msg.LoadByte((char) new_x);
		tcpclient->Send(msg);
	}

	//Handle server
	if (Connected) {
	//Player == 0 is always Server.
		if(CurrentPlayer == 0) {
			setEntityStatus();//(new_X, new_Y, character, action);
			CurrentPlayer = 1;
			msg.LoadByte((char) new_x);
			tcpclient->Send(msg);
		}
	}
}

/**
 * Set received Entity status/position
 */
void Multiplayer::setEntityStatus() {
	//UNIMPLEMENTED
}
