// battleships_final.cpp : Defines the entry point for the console application.

// WinSock2.h Client and Server by "https://www.youtube.com/watch?v=AjG4jcVn6QE"
// how to convert a string to a LPCWSTR "https://stackoverflow.com/questions/27220/how-to-convert-stdstring-to-lpcwstr-in-c-unicode"
#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <WinSock2.h>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;



const int MAXXY = 6;
const int MAXSHIPS = 6;
const int MAXSHOTS = MAXXY * MAXXY;
int eShipPasser = 1;



string endmessage;
SOCKET Connection;//This client's connection to the server

void OpenServer(string s)
{
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	wstring r(buf);
	delete[] buf;
	LPCWSTR Location = r.c_str();
	if (!CreateProcess
	(Location,
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&si,
		&pi
	)
		)
	{
		cout << "Unable to execute Server.exe.";
	}
}
enum Packet
{
	P_ChatMessage,
	P_Test
};

bool SendInt(int _int)
{
	int RetnCheck = send(Connection, (char*)&_int, sizeof(int), NULL); //send int: _int
	if (RetnCheck == SOCKET_ERROR) //If int failed to send due to connection issue
		return false; //Return false: Connection issue
	return true; //Return true: int successfully sent
}

bool GetInt(int & _int)
{
	int RetnCheck = recv(Connection, (char*)&_int, sizeof(int), NULL); //receive integer
	if (RetnCheck == SOCKET_ERROR) //If there is a connection issue
		return false; //return false since we did not get the integer
	return true;//Return true if we were successful in retrieving the int
}

bool SendPacketType(Packet _packettype)
{
	int RetnCheck = send(Connection, (char*)&_packettype, sizeof(Packet), NULL); //Send packet: _packettype
	if (RetnCheck == SOCKET_ERROR) //If packettype failed to send due to connection issue
		return false; //Return false: Connection issue
	return true; //Return true: int successfully sent
}

bool GetPacketType(Packet & _packettype)
{
	int RetnCheck = recv(Connection, (char*)&_packettype, sizeof(Packet), NULL); //receive packet type (same as integer)
	if (RetnCheck == SOCKET_ERROR) //If there is a connection issue
		return false; //return false since we did not properly get the packet type
	return true;//Return true if we were successful in retrieving the packet type
}

bool SendString(std::string & _string)
{
	if (!SendPacketType(P_ChatMessage)) //Send packet type: Chat Message, If sending packet type fails...
		return false; //Return false: Failed to send string
	int bufferlength = _string.size(); //Find string buffer length
	if (!SendInt(bufferlength)) //Send length of string buffer, If sending buffer length fails...
		return false; //Return false: Failed to send string buffer length
	int RetnCheck = send(Connection, _string.c_str(), bufferlength, NULL); //Send string buffer
	if (RetnCheck == SOCKET_ERROR) //If failed to send string buffer
		return false; //Return false: Failed to send string buffer
	return true; //Return true: string successfully sent
}

bool GetString(std::string & _string)
{
	int bufferlength; //Holds length of the message
	if (!GetInt(bufferlength)) //Get length of buffer and store it in variable: bufferlength
		return false; //If get int fails, return false
	char * buffer = new char[bufferlength + 1]; //Allocate buffer
	buffer[bufferlength] = '\0'; //Set last character of buffer to be a null terminator so we aren't printing memory that we shouldn't be looking at
	int RetnCheck = recv(Connection, buffer, bufferlength, NULL); //receive message and store the message in buffer array, set RetnCheck to be the value recv returns to see if there is an issue with the connection
	_string = buffer; //set string to received buffer message
	delete[] buffer; //Deallocate buffer memory (cleanup to prevent memory leak)
	if (RetnCheck == SOCKET_ERROR) //If connection is lost while getting message
		return false; //If there is an issue with connection, return false
	return true;//Return true if we were successful in retrieving the string
}

bool ProcessPacket(Packet _packettype)
{
	switch (_packettype)
	{
	case P_ChatMessage: //If packet is a chat message packet
	{
		std::string Message; //string to store our message we received
		if (!GetString(Message)) //Get the chat message and store it in variable: Message
			return false; //If we do not properly get the chat message, return false
		//std::cout << Message << std::endl; //Display the message to the user

		if (atoi(Message.c_str()) == 0)
		{

		}
		else
		{
			eShipPasser = atoi(Message.c_str());
		}


		endmessage = Message;


		break;
	}
	default: //If packet type is not accounted for
		std::cout << "Unrecognized packet: " << _packettype << std::endl; //Display that packet was not found
		break;
	}
	return true;
}

void ClientThread()
{
	Packet PacketType;
	while (true)
	{
		if (!GetPacketType(PacketType)) //Get packet type
			break; //If there is an issue getting the packet type, exit this loop
		if (!ProcessPacket(PacketType)) //Process packet (packet type)
			break; //If there is an issue processing the packet, exit this loop
	}
	std::cout << "Lost connection to the server." << std::endl;
	closesocket(Connection);
}

void UserInputs()
{
	std::string userinput; //holds the user's chat message
	while (true)
	{
		std::getline(std::cin, userinput); //Get line if user presses enter and fill the buffer
		if (!SendString(userinput)) //Send string: userinput, If string fails to send... (Connection issue)
			break;
		Sleep(10);
	}
}

void SendFrCo(string x)
{
	string Co;
	Co = x;

	//sending co

	if (SendString(Co));

	// sending end message
	Co = "0";
	if (SendString(Co));

	// doesnt end till em sends the end message so that it doesnt miss the loop in main
	string end = "0";
	int Cloop = 1;
	while (Cloop != 0)
	{
		if (endmessage == end)
		{
			endmessage = "1";
			Cloop = 0;
		}
	}
}


void EndlLoop(int x)
{
	for (int i = 0; i < x; i++)
	{
		cout << endl;
	}
}
void PrintRules()
{
	cout << "Welcome to BattleShips!!" << endl;
	cout << endl;
	cout << "* * * * * * *  * * * * * * * How to play * * * * * * * * * * * * * * " << endl; cout << endl;
	cout << "Choose if you would like to play against a PC or another player" << endl;
	cout << "Choose the x and y cordinates for the 6 ships" << endl;
	cout << "The square is 6x6" << endl;
	cout << "Choose the x and y cordinates for your fire" << endl;
	cout << "Open sea will be shown as '~'" << endl;
	cout << "Your placed ships show as '0'" << endl;
	cout << "Fire on the enemy and your water show as 'x' " << endl;
	cout << "An enemy ship that has been destroyed will show as a '0'" << endl;
	EndlLoop(2);
}

string ServerDestinationLocal()
{
	string choice;
	string home = "home";
	string uni = "uni";
	string output;
	int homeOrUniTest = 0;
	while (homeOrUniTest != 1)
	{
		cout << "Are you at home or uni?" << endl;
		cin >> choice;

		if (choice == home)
		{
			ifstream myfile("homeLocal.txt");
			if (myfile.is_open())
			{
				while (getline(myfile, output))
				{
				}
				return output;
			}
			myfile.close();
			homeOrUniTest = 1;
		}
		else if (choice == uni)
		{
			ifstream myfile("uniLocal.txt");
			if (myfile.is_open())
			{
				while (getline(myfile, output))
				{
				}
				return output;
			}
			myfile.close();
			homeOrUniTest = 1;
		}
	}
}
string ServerDestinationPublic()
{
	string choice;
	string home = "home";
	string uni = "uni";
	string output;
	int homeOrUniTest = 0;
	while (homeOrUniTest != 1)
	{
		cout << "Are you at home or uni?" << endl;
		cin >> choice;
		if (choice == home)
		{
			ifstream myfile("homePublic.txt");
			if (myfile.is_open())
			{
				while (getline(myfile, output))
				{
				}
				return output;
			}
			myfile.close();
			homeOrUniTest = 1;
		}
		else if (choice == uni)
		{
			ifstream myfile("uniPublic.txt");
			if (myfile.is_open())
			{
				while (getline(myfile, output))
				{
				}
				return output;
			}
			myfile.close();
			homeOrUniTest = 1;
		}
	}
}
struct XY {
	int x;
	int y;
};


int main()
{
	srand(time(NULL));
	XY boardArray[MAXXY];
	int frBoard[MAXXY][MAXXY];
	int emBoard[MAXXY][MAXXY];
	int end = 0;
	int choice = 0;
	int frcounter = 0;
	int emcounter = 0;
	string ServerFile = "";


	// filling the struct array

	for (int i = 0; i < MAXXY; i++)
	{
		boardArray[i].x = i + 1;
		boardArray[i].y = i + 1;
		for (int a = 0; a < MAXXY; a++)
		{
			frBoard[a][i] = 0;
			emBoard[a][i] = 0;
		}
	}

	PrintRules();


	//// setting up frBoard ships

	//for (int i = 0; i < MAXSHIPS; i++)
	//{
	//	int temp1, temp2;
	//	cout << "Please enter x for ship " << i + 1 << endl;
	//	cin >> temp1;
	//	cout << "Please enter y for ship " << i + 1 << endl;
	//	cin >> temp2;
	//	frBoard[temp1 -1][temp2 -1]++;
	//}


	//// printing with ships
	//EndlLoop(2);
	//for (int i = 0; i < MAXXY; i++)
	//{
	//	cout << "    " << boardArray[i].x;
	//}
	//EndlLoop(2);
	//for (int y = 0; y < MAXXY; y++)
	//{
	//	cout << boardArray[y].y;
	//	if (frBoard[0][y] == 1)
	//	{
	//		cout << "   0";
	//	}
	//	else
	//	{
	//		cout << "   ~";
	//	}
	//	for (int x = 1; x < MAXXY; x++)
	//	{
	//		if (frBoard[x][y] == 1)
	//		{
	//			cout << "    0";
	//		}
	//		else
	//		{
	//			cout << "    ~";
	//		}
	//	}
	//	EndlLoop(2);
	//}

	// choice
	cout << "1 - play against comp. 2 - play against player. Any other number will quit" << endl;
	cin >> choice;
	if (choice == 1)
		//computer
	{
		cout << "You have selected to play against the computer" << endl;
		EndlLoop(2);

		// setting up frBoard ships


		cout << flush;
		system("CLS");
		PrintRules();

		for (int i = 0; i < MAXSHIPS; i++)
		{
			int temp1, temp2;
			int endShip = 0;
			while (endShip != 1)
			{
				cout << "Please enter x for ship " << i + 1 << endl;
				cin >> temp1;
				if ((temp1 > 0)&&(temp1 < MAXXY + 1))
				{
					endShip = 1;
				}
			}
			endShip = 0;
			while (endShip != 1)
			{
				cout << "Please enter y for ship " << i + 1 << endl;
				cin >> temp2;
				if ((temp2 > 0)&&(temp2 < MAXXY + 1))
				{
					endShip = 1;
				}
			}
			endShip = 0;
			frBoard[temp1 - 1][temp2 - 1]++;
		}

		cout << flush;
		system("CLS");
		PrintRules();
		// printing with ships
		EndlLoop(2);
		for (int i = 0; i < MAXXY; i++)
		{
			cout << "    " << boardArray[i].x;
		}
		EndlLoop(2);
		for (int y = 0; y < MAXXY; y++)
		{
			cout << boardArray[y].y;
			if (frBoard[0][y] == 1)
			{
				cout << "   0";
			}
			else
			{
				cout << "   ~";
			}
			for (int x = 1; x < MAXXY; x++)
			{
				if (frBoard[x][y] == 1)
				{
					cout << "    0";
				}
				else
				{
					cout << "    ~";
				}
			}
			EndlLoop(2);
		}

		// setting up comp ships

		int arr1[MAXXY];
		int arr2[MAXXY];
		for (int a = 0; a < MAXXY; a++)
		{
			arr1[a] = a;
			arr2[a] = a;

		}

		random_shuffle(&arr1[0], &arr1[MAXXY]);
		random_shuffle(&arr2[0], &arr2[MAXXY]);




		for (int i = 0; i < MAXSHIPS; i++)
		{
			int temp1 = arr1[i];
			int temp2 = arr2[i];
			emBoard[temp1][temp2] = 1;
		}




		// Main Game 
		while (end != 1)
		{

			// firing
			int temp1, temp2;
			cout << "Where would you like to fire captain?" << endl;
			int endFire = 0;
			while (endFire != 1)
			{
				cout << "Enter the x co" << endl;
				cin >> temp1;
				if ((temp1 > 0) && (temp1 < MAXXY + 1))
				{
					endFire = 1;
				}
			}
			endFire = 0;
			while (endFire != 1)
			{
				cout << "Enter the y co" << endl;
				cin >> temp2;
				if ((temp2 > 0) && (temp2 < MAXXY + 1))
				{
					endFire = 1;
				}

			}
			endFire = 0;
			EndlLoop(2);

			int temp3 = rand() % MAXXY;
			int temp4 = rand() % MAXXY;


			frBoard[temp3][temp4] = frBoard[temp3][temp4] + 2;
			emBoard[temp1 - 1][temp2 - 1] = emBoard[temp1 - 1][temp2 - 1] + 2;

			cout << flush;
			system("CLS");

			PrintRules();
			// print fr board

			int inCounter = 0;

			cout << "Fr Board" << endl;
			for (int i = 0; i < MAXXY; i++)
			{
				cout << "    " << boardArray[i].x;
			}
			EndlLoop(2);
			for (int y = 0; y < MAXXY; y++)
			{
				cout << boardArray[y].y;
				switch (frBoard[0][y])
				{

				case 1: cout << "   0";
					break;
				case 0: cout << "   ~";
					break;
				default: cout << "   x";
				}
				if (frBoard[0][y] == 3)
				{
					inCounter++;
				}
				for (int x = 1; x < MAXXY; x++)
				{
					switch (frBoard[x][y])
					{

					case 1: cout << "    0";
						break;
					case 0: cout << "    ~";
						break;
					default: cout << "    x";

					}
					if (frBoard[x][y] == 3)
					{
						inCounter++;
					}
				}
				EndlLoop(2);
			}

			if (inCounter == MAXSHIPS)
			{
				frcounter = 1;
			}

			inCounter = 0;
			// print em board
			cout << "Em Board" << endl;
			for (int i = 0; i < MAXXY; i++)
			{
				cout << "    " << i + 1;
			}
			cout << endl; cout << endl;
			for (int y = 0; y < MAXXY; y++)
			{
				cout << y + 1;
				switch (emBoard[0][y])
				{
				case 3: cout << "   0";
					break;
				case 2: cout << "   x";
					break;
				case 1: cout << "   ~"; // change this to reveal em ships
					break;
				case 0: cout << "   ~";
					break;
				default: cout << "   x";
				}
				if (emBoard[0][y] == 3)
				{
					inCounter++;
				}
				for (int x = 1; x < MAXXY; x++)
				{
					switch (emBoard[x][y])
					{
					case 3: cout << "    0";
						break;
					case 2: cout << "    x";
						break;
					case 1: cout << "    ~"; // change this to reveal em ships
						break;
					case 0: cout << "    ~";
						break;
					default: cout << "    x";

					}
					if (emBoard[x][y] == 3)
					{
						inCounter++;
					}
				}
				EndlLoop(2);
			}
			if (inCounter == MAXSHIPS)
			{
				emcounter = 1;
			}

			if (emcounter == 1)
			{
				cout << "Well done. You won." << endl;
				end = 1;

			}
			if (frcounter == 1)
			{
				cout << "Maybe next time. You Lost." << endl;
				end = 1;
			}


		}
		int endQuestion = 0;



	}
	else if (choice == 2)
		//PvP choice 
	{
		//Winsock Startup
		WSAData wsaData;
		WORD DllVersion = MAKEWORD(2, 1);
		// making the server load if its not open auto


		if (WSAStartup(DllVersion, &wsaData) != 0)
		{
			MessageBoxA(NULL, "Winsock startup failed. Please start again", "Error", MB_OK | MB_ICONERROR);
			return 0;
		}

		SOCKADDR_IN addr; //Address to be binded to our Connection socket
		int sizeofaddr = sizeof(addr); //Need sizeofaddr for the connect function


		string intpref;
		string local = "local";
		string ppublic = "public";
		int localOrPublicTest = 0;
		while (localOrPublicTest != 1)
		{
			cout << "local or public?" << endl;
			cin >> intpref;

			if (intpref == ppublic)
			{
				std::cout << "Please enter your public IP address." << std::endl;
				char IP[14];
				std::cin >> IP;
				addr.sin_addr.s_addr = inet_addr(IP);
				ServerFile = ServerDestinationPublic();
				localOrPublicTest = 1;
			}
			if (intpref == local)
			{
				addr.sin_addr.s_addr = inet_addr("127.0.0.1");
				ServerFile = ServerDestinationLocal();
				localOrPublicTest = 1;
			}

		}
		// ** if doing it through public use this:
		//std::cout << "Please enter your public IP address." << std::endl;
		//char IP[14];
		//std::cin >> IP;
		//addr.sin_addr.s_addr = inet_addr(IP);

		// ** if doing it through local  use this:
		//addr.sin_addr.s_addr = inet_addr("127.0.0.1");


		addr.sin_port = htons(5010);
		addr.sin_family = AF_INET; //IPv4 Socket

		Connection = socket(AF_INET, SOCK_STREAM, NULL); //Set Connection socket
		if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0)
		{
			cout << "Please wait whilst we set things up for you" << endl;
			OpenServer(ServerFile);
			Sleep(5000);
			if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0) //If we are unable to connect...
			{
				MessageBoxA(NULL, "Failed to Connect", "Error", MB_OK | MB_ICONERROR);
				return 0; //Failed to Connect
			}
		}


		std::cout << "Connected!" << std::endl;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL); //Create the client thread that will receive any data that the server sends.






			// sending fr ships, recv em ships
		for (int i = 0; i < MAXSHIPS; i++)
		{
			string passer;
			int FrX = 0;
			int FrY = 0;
			int EmX = 0;
			int EmY = 0;

			
			int passerTest = 0;
			while (passerTest != 1)
			{
				cout << "Please enter x for ship " << i + 1 << endl;
				cin >> passer;
				if ((atoi(passer.c_str()) > 0) && (atoi(passer.c_str()) < MAXXY + 1))
				{
					passerTest = 1;
				}
			}

			passerTest = 0;
			EndlLoop(1);
			SendFrCo(passer);
			FrX = atoi(passer.c_str());
			EmX = eShipPasser;

			while (passerTest != 1)
			{
				cout << "Please enter y for ship " << i + 1 << endl;
				cin >> passer;
				if ((atoi(passer.c_str()) > 0) && (atoi(passer.c_str()) < MAXXY + 1))
				{
					passerTest = 1;
				}
			}
			passerTest = 0;
			EndlLoop(1);
			SendFrCo(passer);
			FrY = atoi(passer.c_str());
			EmY = eShipPasser;

			// checking messages pass
			/*cout << "The Frx is set as " << FrX << endl;
			cout << "The FrY is set as " << FrY << endl;
			cout << "The EmX is set as " << EmX << endl;
			cout << "The EmY is set as " << EmY << endl;*/
			frBoard[FrX - 1][FrY - 1] = 1;
			emBoard[EmX - 1][EmY - 1] = 1;
		}

		// print fr board
		for (int i = 0; i < MAXXY; i++)
		{
			cout << "    " << boardArray[i].x;
		}
		EndlLoop(2);
		for (int y = 0; y < MAXXY; y++)
		{
			cout << boardArray[y].y;
			if (frBoard[0][y] == 1)
			{
				cout << "   0";
			}
			else
			{
				cout << "   ~";
			}
			for (int x = 1; x < MAXXY; x++)
			{
				if (frBoard[x][y] == 1)
				{
					cout << "    0";
				}
				else
				{
					cout << "    ~";
				}
			}
			EndlLoop(2);
		}

		//Main Game 
		while (end != 1)
		{
			int FrFX, FrFY, EmFX, EmFY;
			string passer;

			cout << "Where would you like to fire?" << endl;

			cout << "Enter the x co" << endl;
			cin >> passer;
			EndlLoop(1);
			SendFrCo(passer);
			FrFX = atoi(passer.c_str());
			EmFX = eShipPasser;

			cout << "Enter the y co" << endl;

			cin >> passer;
			EndlLoop(1);
			SendFrCo(passer);
			FrFY = atoi(passer.c_str());
			EmFY = eShipPasser;

			emBoard[FrFX - 1][FrFY - 1] = emBoard[FrFX - 1][FrFY - 1] + 2;
			frBoard[EmFX - 1][EmFY - 1] = frBoard[EmFX - 1][EmFY - 1] + 2;

			cout << flush;
			system("CLS");

			PrintRules();

			// print fr board

			int inCounter = 0;

			cout << "Fr Board" << endl;
			for (int i = 0; i < MAXXY; i++)
			{
				cout << "    " << boardArray[i].x;
			}
			EndlLoop(2);
			for (int y = 0; y < MAXXY; y++)
			{
				cout << boardArray[y].y;
				switch (frBoard[0][y])
				{

				case 1: cout << "   0";
					break;
				case 0: cout << "   ~";
					break;
				default: cout << "   x";
				}
				if (frBoard[0][y] == 3)
				{
					inCounter++;
				}
				for (int x = 1; x < MAXXY; x++)
				{
					switch (frBoard[x][y])
					{

					case 1: cout << "    0";
						break;
					case 0: cout << "    ~";
						break;
					default: cout << "    x";

					}
					if (frBoard[x][y] == 3)
					{
						inCounter++;
					}
				}
				EndlLoop(2);
			}

			if (inCounter == MAXSHIPS)
			{
				frcounter = 1;
			}

			inCounter = 0;
			// print em board
			cout << "Em Board" << endl;
			for (int i = 0; i < MAXXY; i++)
			{
				cout << "    " << i + 1;
			}
			cout << endl; cout << endl;
			for (int y = 0; y < MAXXY; y++)
			{
				cout << y + 1;
				switch (emBoard[0][y])
				{
				case 3: cout << "   0";
					break;
				case 2: cout << "   x";
					break;
				case 1: cout << "   ~";
					break;
				case 0: cout << "   ~";
					break;
				default: cout << "   x";
				}
				if (emBoard[0][y] == 3)
				{
					inCounter++;
				}
				for (int x = 1; x < MAXXY; x++)
				{
					switch (emBoard[x][y])
					{
					case 3: cout << "   0";
						break;
					case 2: cout << "   x";
						break;
					case 1: cout << "   ~";
						break;
					case 0: cout << "   ~";
						break;
					default: cout << "   x";

					}
					if (emBoard[x][y] == 3)
					{
						inCounter++;
					}
				}
				EndlLoop(2);
			}
			if (inCounter == MAXSHIPS)
			{
				emcounter = 1;
			}

			if (emcounter == 1)
			{
				cout << "Well done. You won." << endl;
				end = 1;

			}
			if (frcounter == 1)
			{
				cout << "Maybe next time. You Lost." << endl;
				end = 1;
			}
		}





				return 0;
	}


	return 0;
}



