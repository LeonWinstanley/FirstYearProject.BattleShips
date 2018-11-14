/*
Author: Pindrought
Date: 2/23/2016
This is the solution for the server that you should have at the end of tutorial 5.
*/
#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <string>
#include <iostream>

enum Packet
{
	P_ChatMessage,
	P_Test
};

const int MAXCONNECTIONS = 100;

SOCKET Connections[MAXCONNECTIONS];

int TotalConnections = 0;

bool SendInt(int ID, int _int)
{
	int RetnCheck = send(Connections[ID], (char*)&_int, sizeof(int), NULL); //send int: _int
	if (RetnCheck == SOCKET_ERROR) //If int failed to send due to connection issue
		return false; //Return false: Connection issue
	return true; //Return true: int successfully sent
}

bool GetInt(int ID, int & _int)
{
	int RetnCheck = recv(Connections[ID], (char*)&_int, sizeof(int), NULL); //receive integer
	if (RetnCheck == SOCKET_ERROR) //If there is a connection issue
		return false; //return false since we did not get the integer
	return true;//Return true if we were successful in retrieving the int
}

bool SendPacketType(int ID, Packet _packettype)
{
	int RetnCheck = send(Connections[ID], (char*)&_packettype, sizeof(Packet), NULL); //Send packet: _packettype
	if (RetnCheck == SOCKET_ERROR) //If packettype failed to send due to connection issue
		return false; //Return false: Connection issue
	return true; //Return true: int successfully sent
}

bool GetPacketType(int ID, Packet & _packettype)
{
	int RetnCheck = recv(Connections[ID], (char*)&_packettype, sizeof(Packet), NULL); //receive packet type (same as integer)
	if (RetnCheck == SOCKET_ERROR) //If there is a connection issue
		return false; //return false since we did not properly get the packet type
	return true;//Return true if we were successful in retrieving the packet type
}

bool SendString(int ID, std::string & _string)
{
	if (!SendPacketType(ID, P_ChatMessage)) //Send packet type: Chat Message, If sending packet type fails...
		return false; //Return false: Failed to send string
	int bufferlength = _string.size(); //Find string buffer length
	if (!SendInt(ID, bufferlength)) //Send length of string buffer, If sending buffer length fails...
		return false; //Return false: Failed to send string buffer length
	int RetnCheck = send(Connections[ID], _string.c_str(), bufferlength, NULL); //Send string buffer
	if (RetnCheck == SOCKET_ERROR) //If failed to send string buffer
		return false; //Return false: Failed to send string buffer
	return true; //Return true: string successfully sent
}

bool GetString(int ID, std::string & _string)
{
	int bufferlength; //Holds length of the message
	if (!GetInt(ID, bufferlength)) //Get length of buffer and store it in variable: bufferlength
		return false; //If get int fails, return false
	char * buffer = new char[bufferlength + 1]; //Allocate buffer
	buffer[bufferlength] = '\0'; //Set last character of buffer to be a null terminator so we aren't printing memory that we shouldn't be looking at
	int RetnCheck = recv(Connections[ID], buffer, bufferlength, NULL); //receive message and store the message in buffer array, set RetnCheck to be the value recv returns to see if there is an issue with the connection
	_string = buffer; //set string to received buffer message
	delete[] buffer; //Deallocate buffer memory (cleanup to prevent memory leak)
	if (RetnCheck == SOCKET_ERROR) //If connection is lost while getting message
		return false; //If there is an issue with connection, return false
	return true;//Return true if we were successful in retrieving the string
}

bool ProcessPacket(int ID, Packet _packettype)
{
	switch (_packettype)
	{
		case P_ChatMessage: //Packet Type: chat message
		{
			std::string Message; //string to store our message we received
			if (!GetString(ID, Message)) //Get the chat message and store it in variable: Message
				return false; //If we do not properly get the chat message, return false

			for (int i = 0; i < TotalConnections; i++) //Next we need to send the message out to each user
			{
				if (i == ID) //If connection is the user who sent the message...
					continue;//Skip to the next user since there is no purpose in sending the message back to the user who sent it.
				if (!SendString(i, Message)) //Send message to connection at index i, if message fails to be sent...
				{
					std::cout << "Failed to send message from user ID: " << ID << " to user ID: " << i << std::endl;
				}
			}
			std::cout << "Processed chat message packet from user ID: " << ID << std::endl;
			break;
		}

		default: //If packet type is not accounted for
		{
			std::cout << "Unrecognized packet: " << _packettype << std::endl; //Display that packet was not found
			break;
		}
	}
	return true;
}

void ClientHandlerThread(int ID) //ID = the index in the SOCKET Connections array
{
	Packet PacketType;
	while (true)
	{
		if (!GetPacketType(ID, PacketType)) //Get packet type
			break; //If there is an issue getting the packet type, exit this loop
		if (!ProcessPacket(ID, PacketType)) //Process packet (packet type)
			break; //If there is an issue processing the packet, exit this loop
	}
	std::cout << "Lost connection to user ID: " << ID << std::endl;
	closesocket(Connections[ID]);
}

int main()
{
	//Winsock Startup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0) //If WSAStartup returns anything other than 0, then that means an error has occured in the WinSock Startup.
	{
		MessageBoxA(NULL, "WinSock startup failed", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	
	SOCKADDR_IN addr; //Address that we will bind our listening socket to
	int addrlen = sizeof(addr); //length of the address (required for accept call)
	
	// ** if doing it through localhost use this:
	//addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// ** if doing it through local network use this:
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	
	addr.sin_port = htons(5010); //Port
	addr.sin_family = AF_INET; //IPv4 Socket
	std::cout << "Waiting for connection" << std::endl;
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); //Create socket to listen for new connections
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr)); //Bind the address to the socket
	listen(sListen, SOMAXCONN); //Places sListen socket in a state in which it is listening for an incoming connection. Note:SOMAXCONN = Socket Oustanding Max Connections

	SOCKET newConnection; //Socket to hold the client's connection
	int ConnectionCounter = 0; //# of client connections
	for (int i = 0; i < MAXCONNECTIONS; i++)
	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); //Accept a new connection
		if (newConnection == 0) //If accepting the client connection failed
		{
			std::cout << "Failed to accept the client's connection." << std::endl;
		}
		else //If client connection properly accepted
		{
			std::cout << "Client Connected!" << std::endl;
			Connections[i] = newConnection; //Set socket in array to be the newest connection before creating the thread to handle this client's socket.
			TotalConnections += 1; //Incremenent total # of clients that have connected
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(i), NULL, NULL); //Create Thread to handle this client. The index in the socket array for this thread is the value (i).
			
		}
	}
	
	
	return 0;
}