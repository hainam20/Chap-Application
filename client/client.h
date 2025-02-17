#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <string>
#include <vector>
#include <thread>
#include <regex>
#include <set>
#include <winsock2.h>
#include "GUI.h"
#include "RoomChat.h"

SOCKET clientSocket;

class Client
{
private:
    std::string username;
    std::string password;
    std::vector<RoomChat> roomList;
    GUI gui;
    RoomChat room;
    void printMessage(std::string &message);
    std::string inputPassword(int x, int y);
    // void IP_CONFIG();

    void RESET_SOCKET();
    int SOCKET_START(SOCKET &clientSocket);
    void GENERATE_LOGIN();
    void GENERATE_SIGNUP();

public:
    void SET_UP_CONNECTION();
    void FIRST_MENU();
    Client();
    ~Client();
    void START();
};

#endif
