#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <winsock2.h>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <cstdlib> // thư viện cần thiết cho rand() và srand()
#include <ctime>   // thư viện cần thiết cho time()
#include <set>
#include <bitset>

#pragma comment(lib, "Ws2_32.lib")

#define PORT_SERVER 8080

std::string FILE_NAME = "account.txt";
std::string keyEcrypt = "hello";
std::string keyFile = "123";
std::string ipServer = "192.168.1.13";
std::set<std::string> roomAvailable;
std::set<std::string> roomPrivateAllocate;
std::unordered_map<std::string, std::string> roomPrivate;

class Server
{
private:
    void HandleClient(SOCKET clientSocket);
    void BroadcastMessage(const std::string &message, const std::vector<SOCKET> &clients, const SOCKET senderSocket);
    void notifyJoin(std::string username, const std::vector<SOCKET> &clients, const SOCKET senderSocket);
    void roomChat(std::string room, SOCKET clientSocket);
    bool checkRoom(std::string roomName);
    int saveRoom(std::string roomName);
    int loadRoom(std::string fileName);

    std::string generateKey();

    std::vector<SOCKET> clients;
    std::unordered_map<std::string, std::vector<SOCKET>> chatroom;
    std::mutex clientsMutex;

public:
    Server();
    int StartServer();
};

#endif // SERVER_H
