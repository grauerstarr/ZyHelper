#include <winsock2.h>
#include <iostream>
#include <tchar.h>

#include "tcp_connection.h"


#include "Debug.h"



TCPConnection::TCPConnection()
{
}

int TCPConnection::Init()
{
	FILE *stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);


	// Winsock starten
	WSADATA wsa;
	error_code = WSAStartup(MAKEWORD(2, 0), &wsa);
	if (error_code != 0)
	{
		MessageBox(NULL, _T("error couldnt run WSAStartup"),
			_T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}
	DEBUG_MSG("WSAStartup" << std::endl);

	// Socket erstellen
	acceptSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (acceptSocket == INVALID_SOCKET)
	{
		MessageBox(NULL, _T("error couldnt create socket"),
			_T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}
	DEBUG_MSG("accepted socket" << std::endl);

	// Socket binden
	memset(&addr, 0, sizeof(SOCKADDR_IN));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(2206);
	addr.sin_addr.s_addr = ADDR_ANY;

	error_code = ::bind(acceptSocket, (SOCKADDR*)&addr, sizeof(addr));

	if (error_code == SOCKET_ERROR)
	{
		MessageBox(NULL, _T("error couldnt bind"),
			_T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}
	DEBUG_MSG("bound socket" << std::endl);
	return 0;

}

int TCPConnection::Listen()
{
	while (1)
	{
		// In den listen Modus
		error_code = listen(acceptSocket, 10);
		if (error_code == SOCKET_ERROR)
		{
			MessageBox(NULL, _T("Error couldnt switch to listening mode"),
				_T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
			return 1;
		}
		DEBUG_MSG("started listening" << std::endl);

		// Verbindung annehmen
		connectedSocket = accept(acceptSocket, NULL, NULL);
		if (connectedSocket == INVALID_SOCKET)
		{
			MessageBox(NULL, _T("FError couldnt accept connection"),
				_T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
			return 1;
		}
		DEBUG_MSG("accepted the connection" << std::endl);

		StartupTime = GetTickCount();

		Running = true;
		// Daten austauschen
		while (error_code != SOCKET_ERROR)
		{
			error_code = recv(connectedSocket, text, 255, 0);

			tcpmutex.lock();
			content = text;
			tcpmutex.unlock();

			//cout <<"recv:" << content <<" " << error_code<< endl;
			if (error_code == 0)
			{
				DEBUG_MSG("terminated" << std::endl);
				MessageBox(NULL, _T("terminated"),
					_T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
				Running = false;
				break;
			}
			if (error_code == SOCKET_ERROR) 
			{
				DEBUG_MSG("error recieved a socket error" << std::endl);
				MessageBox(NULL, _T("error recieved a socket error"),
					_T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
				Running = false;
				break;
			}
			text[error_code] = '\0';
			Sleep(1);
		}
		DEBUG_MSG("Socket Error" << std::endl);
		MessageBox(NULL, _T("Socket Error"),
			_T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		Running = false;
	}
}

int TCPConnection::Exit()
{
	closesocket(acceptSocket);
	closesocket(connectedSocket);
	WSACleanup();
	return 0;
}

bool TCPConnection::ElementAt(unsigned  i, unsigned j)
{
	std::string copy;
	bool result;
	tcpmutex.lock();
	if (content.length() >= j)
	{
		copy = content;
	}
	else 
	{
		tcpmutex.unlock();
		return false;
	}
	tcpmutex.unlock();

	result = (copy.at(j) & (1 << i))!=0;

	return result;
}

bool TCPConnection::ElementAt(byte* bytedata, unsigned j) {
	try {
		*bytedata = text[j];
		return true;
	}
	catch (const std::out_of_range&) {
		return false;
	}


}


int GetFloatFromByte(float* fdata, unsigned char data0, unsigned char data1, unsigned char data2, unsigned char data3) {
	//Get IEEE 754 Floating point format from data0, data1, data2, data3     
	//fdata  :  float  data  IEEE754 floating-point Translate 
	//data0, data1, data2, data3 : 8-bit Hexadecimal data       
	//return code ==> 0: OK, -1: Not Available         
	int  i, s = 1;
	unsigned char e = 0;
	unsigned long m = 0;
	double  value, result;
	if (data0 & 0x80)
		s = -1;
	e = (data0 & 0x7F) << 1;
	e = e | ((data1 & 0x80) ? 1 : 0);
	m = (data1 & 0x7F) << 16;      m = m | (data2 << 8);
	m = m | data3;
	if ((e == 0) && (m == 0))
	{
		fdata = 0;
		return 0;
	}
	else if ((data0 == 0x7F) && (data1 >= 0x80))//7F800001~7FFFFFFF   
	{
		if ((data1 != 0x80) && (data3 != 0x00))       return -1;
	}
	else if ((data0 == 0xFF) && (data1 >= 0x80))//FF800001~FFFFFFFF   
	{
		if ((data1 != 0x80) && (data3 != 0x00))       return -1;
	}
	result = 1;      for (i = -23; i < 0; i++)
	{
		value = (m & 0x00000001) ? pow(2, i) : 0;
		result += value;      m = m >> 1;
	}
	result *= s;
	result *= pow(2, e - 127);
	*fdata = result;
	return  0;
}


float TCPConnection::FloatAt(unsigned j)
{
	float x = 0.0f;
	byte nr1;
	byte nr2;
	byte nr3;
	byte nr4;
	if (ElementAt(&nr1, j) && ElementAt(&nr2, j + 1) && ElementAt(&nr3, j + 2) && ElementAt(&nr4, j + 3))
	{
		GetFloatFromByte(&x, nr1, nr2, nr3, nr4);
		return x;
	}
	else {
		return 0.0f;
	}

}

bool TCPConnection::IsReady()
{
	if (!Running) return false;
	if (GetTickCount() - 5000 < StartupTime) return false;

	tcpmutex.lock();
	bool viable = content.length() >= 2;
	tcpmutex.unlock();

	if (viable)
	{
		return true;
	}
	return false;
}

TCPConnection::~TCPConnection()
{
}
