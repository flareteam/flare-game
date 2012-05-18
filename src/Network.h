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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <cstring>
#include "SDL_net.h"

// Data Packet size is 100 bytes
typedef struct DataPacket {
    unsigned char img_main [30];
    unsigned char img_armor [30];
    unsigned char img_off [30];
    bool takehit;
    unsigned char current_power;

    unsigned short int face_map_x;
    unsigned short int face_map_y;
    unsigned short int hero_pos_x;
    unsigned short int hero_pos_y;
} DataPacket;

//==============================================================================

class IpAddress {
  private:
	IPaddress m_Ip;
  public:
	IpAddress();
	IpAddress (Uint16 port);
	IpAddress (std::string host, Uint16 port);
	void SetIp (IPaddress sdl_ip);
	bool Ok() const; //True if the object have a port and a host associated
	IPaddress GetIpAddress() const;
	Uint32 GetHost() const;
	Uint16 GetPort() const;
};

class TcpSocket {
  protected:
	TCPsocket m_Socket;
	SDLNet_SocketSet set; //a set of sockets. Used here only to check existing packets
  public:
	TcpSocket();
	virtual ~TcpSocket();
	virtual void SetSocket (TCPsocket  the_sdl_socket);
	bool Ok() const;
	bool Ready() const;
};

class ClientSocket;

class HostSocket : public TcpSocket {
  public:
	HostSocket (IpAddress& the_ip_address); //create and open a new socket, with an existing IpAddress object

	HostSocket (Uint16 port); //create and open a new socket with the desired port
	bool Accept (ClientSocket&); //set a client TcpSocket object after listening to the port
};

class ClientSocket : public TcpSocket {
  private:
	IpAddress m_RemoteIp;
  public:
	ClientSocket();
	ClientSocket (std::string host, Uint16 port); //Create the object and connect to a host, in a given port
	bool Connect (IpAddress& remoteip);
	bool Connect (HostSocket& the_listener_socket);
	void SetSocket (TCPsocket  the_sdl_socket);
	IpAddress getIpAddress () const;
	bool Receive(DataPacket* rData);
	bool Send (DataPacket* sData);
};

//==============================================================================

class Multiplayer {

	private:
		//network status indicator
		bool Connected;
		//network objects
		HostSocket* tcplistener;
		ClientSocket* tcpclient;
		IpAddress* remoteip;

		int CurrentPlayer;

		void serverLoop();
		void clientLoop();
		void setEntityStatus(int recv_x, int recv_y);

	public:
		Multiplayer();
		~Multiplayer();

		bool Init(); //Initialize SDL_net
		void Quit(); //Exit SDL_net
		void startServer( int port);
		void connectToServer(std::string ip, int port);
		void MultiplayerLoop();
		void statusHandler(int new_x, int new_y);
};
