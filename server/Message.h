#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <string>
#include <vector>
#include <iostream>
#include <winsock2.h>

class Message
{
private:
    std::string roomName;
    std::string userName;
    std::string message;

public:
    Message();
    std::string encryptMessage(const std::string &plaintext, const std::string &key);
    std::string decryptMessage(const std::string &ciphertext, const std::string &key);
    std::string extractUsername(const std::string &fullMessage);
    std::string extractActualMessage(const std::string &fullMessage);
    void splitMessage(const std::string &message);
    std::string getRoomName();
    std::string getuserName();
    std::string getMessage();
    void setRoomName(std::string roomName);
    void getuserName(std::string userName);
    void getMessage(std::string message);
};

#endif // _MESSAGE_H_