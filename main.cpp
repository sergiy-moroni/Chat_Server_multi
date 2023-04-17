#include <winsock2.h>
#include <iostream>
#include <filesystem>
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "User.h"
#include "Message.h"
#include "Logger.h"
#include <thread>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
const short BUFF_SIZE = 1024;
SOCKET Connections[100];
using namespace std;
Logger logger;
vector<User> Users;
int user_islogin = -1;
std::string Textforlog = logger.fromLog();

void readUsers() {
	//Добавление зарегистрированных пользователей из файла в Users
	std::fstream user_file = std::fstream("users.txt", std::ios::in | std::ios::out);
	if (!user_file)
		user_file = std::fstream("users.txt", std::ios::in | std::ios::out | std::ios::trunc);
	User obj;
	User objcnrl;
	user_file >> obj;
	while (true)
	{
		if (obj == objcnrl)
		{
			break;
		}
		objcnrl = obj;
		Users.push_back(obj);
		user_file >> obj;
	}
	cout << "Users count = " << Users.size() << endl;
	logger.toLog("Enter is OK");
};

//функия перевод из буера в строку
void vectostr(std::vector<char>& vec, std::string& str)
{
	str.clear();
	for (int i = 0; i < vec.size(); i++)
	{
		if (vec[i] == '\0')
		{
			break;
		}
		str.push_back(vec[i]);
	}
}

void strtovec(std::vector<char>& vec, const std::string& str)
{
	vec.clear();
	for (int i = 0; i < str.length(); i++)
	{
		vec.push_back(str[i]);
	}
	vec.push_back('\0');
}

bool FindUser(const std::string& login, const std::string& pass)
{
	int i = 0;
	bool j = false;

	for (auto it = Users.begin(); it != Users.end(); ++it)
	{
		std::string l = it->Getlog();
		std::string p = it->Getpass();
		if (login == l && pass == p)
		{
			user_islogin = i;
			j = true;
			return j;
		}
		else
		{
			i++;
		}
	}
	return j;
}

void start(SOCKET ClientConn)
{
	//инициализация вектора для буфера
	vector <char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);
	short packet_size = 0;
	string choise;
	string nameclientISLogin;
	packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);
	cout << "Client's message: " << servBuff.data() << endl;
	string text = "Connect is OK";
	strtovec(clientBuff, text);
	packet_size = send(ClientConn, clientBuff.data(), clientBuff.size(), 0);
	char clientChoise;
	Sleep(100);
	//logger.toLog("Клиент подключился");

	while (true)
	{
		cout << "Users count = " << Users.size() << endl;
		packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);
		cout << "client choise is : " << servBuff.data() << endl;
		clientChoise = servBuff[0];
		if (clientChoise != '0' && clientChoise != '1' && clientChoise != '2')
		{
			cout << "Incorrect Choise" << endl;
			continue;
		}
		//Выход
		if (clientChoise == '0')
		{
			text = "Good by!";
			strtovec(clientBuff, text);
			cout << clientBuff.data() << endl;
			packet_size = send(ClientConn, clientBuff.data(), clientBuff.size(), 0);
			Sleep(100);
			logger.toLog("Клиент вышел");
			break;
		}
		//Регистрация нового пользователя
		if (clientChoise == '2')
		{
			std::string name;
			std::string log;
			std::string pass;
			cout << "Registration: " << endl;
			packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);
			vectostr(servBuff, name);
			cout << "Name is : " << name << endl;
			packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);
			vectostr(servBuff, log);
			cout << "Login is : " << log << endl;
			packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);
			vectostr(servBuff, pass);
			cout << "Pass is : " << pass << endl;
			User NewUser(name, log, pass);
			Users.emplace_back(move(NewUser));
			std::fstream user_file = std::fstream("users.txt", ios::in | ios::app);
			user_file << NewUser;
			continue;
		}
		//Вход
		if (clientChoise == '1')
		{
			cout << "Entered" << endl;
			std::string log;
			std::string pass;
			packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);
			vectostr(servBuff, log);
			cout << "Login is : " << log << endl;
			packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);
			vectostr(servBuff, pass);
			cout << "Pass is : " << pass << endl;
			if (!FindUser(log, pass))
			{
				Textforlog = "Неудачная попытка входа по логину " + log;
				logger.toLog(Textforlog);
				Textforlog.clear();
				string textr = "0";
				strtovec(clientBuff, textr);
				packet_size = send(ClientConn, clientBuff.data(), clientBuff.size(), 0);
				Sleep(100);
				continue;
			}
			if (FindUser(log, pass))
			{
				Textforlog = "Пользователь вошел " + log;
				logger.toLog(Textforlog);
				Textforlog.clear();
				string text = "1";
				strtovec(clientBuff, text);
				packet_size = send(ClientConn, clientBuff.data(), clientBuff.size(), 0);
				Sleep(100);
				while (true)
				{
					char clchoise;
					packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);
					cout << "client choise is : " << servBuff[0] << endl;
					clchoise = servBuff[0];
					if (clchoise != '0' && clchoise != '1' && clchoise != '2') 
					{
						cout << "Incorrect Choise" << endl;
					}

					if (clchoise == '0') 
					{
						break; 
					}
					if (clchoise == '2')
					{
						string mes;
						mes = logger.fromLog();
						strtovec(clientBuff, mes);
						packet_size = send(ClientConn, clientBuff.data(), clientBuff.size(), 0);
						Sleep(100);
					}
					if (clchoise == '1') 
					{
						string mess;
						packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);
						vectostr(servBuff, mess);
						cout << " Message from client: " << mess << endl;
						logger.toLog(mess);
					}
				}
				continue;
			}
		}
	}
}

int main()
{
	cout << "Server Opend" << endl;
	setlocale(LC_ALL, "Rus");
	readUsers();
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}

	cout << "Socked inicialization" << endl;
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	cout << "Vector sockets creating" << endl;
	SOCKET newConnection;
	vector<thread> threads;
	for (int i = 0; i < 100; i++) {
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

		if (newConnection == 0) {
			std::cout << "Error #2\n";
		}
		else {
			std::cout << "Client Connected!\n";
			Connections[i] = newConnection;
			cout << "Thread number " << i << endl;
			threads.push_back(std::thread([newConnection]()
				{start(newConnection); })
			);
		}
	}
	std::for_each(threads.begin(), threads.end(), [](std::thread& t)
		{ t.join(); });
	
	system("pause");
	return 0;
}
