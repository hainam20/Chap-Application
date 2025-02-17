#include "Message.h"
#include <mutex>
Message::Message() {}

std::string Message::getRoomName()
{
    return roomName;
}
std::string Message::getuserName()
{
    return userName;
}
std::string Message::getMessage()
{
    return message;
}

void Message::setRoomName(std::string roomName)
{
    this->roomName = roomName;
}
void Message::getuserName(std::string userName)
{
    this->userName = userName;
}
void Message::getMessage(std::string message)
{
    this->message = message;
}

// Hàm thực hiện mã hóa
std::string Message::encryptMessage(const std::string &plaintext, const std::string &key)
{
    std::string ciphertext = plaintext;

    for (size_t i = 0; i < plaintext.size(); ++i)
    {
        ciphertext[i] = plaintext[i] ^ key[i % key.size()]; // XOR từng ký tự của plaintext với key
    }

    return ciphertext;
}

// Hàm thực hiện giải mã
std::string Message::decryptMessage(const std::string &ciphertext, const std::string &key)
{
    std::string decryptedtext = ciphertext; // Khởi tạo chuỗi giải mã với cùng độ dài như ciphertext

    for (size_t i = 0; i < ciphertext.size(); ++i)
    {
        decryptedtext[i] = ciphertext[i] ^ key[i % key.size()]; // XOR từng ký tự của ciphertext với key
    }

    return decryptedtext;
}

std::string Message::extractUsername(const std::string &fullMessage)
{
    size_t colonPos = fullMessage.find(':');
    if (colonPos == std::string::npos)
    {
        return ""; // Không tìm thấy dấu hai chấm, trả về chuỗi rỗng
    }
    return fullMessage.substr(0, colonPos); // Trả về phần username trước dấu hai chấm
}

// Tach message
std::string Message::extractActualMessage(const std::string &fullMessage)
{
    size_t colonPos = fullMessage.find(':');
    if (colonPos == std::string::npos)
    {
        return ""; // Không tìm thấy dấu hai chấm, trả về chuỗi rỗng
    }
    return fullMessage.substr(colonPos + 1); // Trả về phần tin nhắn sau dấu hai chấm
}

void Message::splitMessage(const std::string &message)
{
    size_t pos1 = message.find(',');
    size_t pos2 = message.rfind(',');

    std::string roomName = message.substr(0, pos1);
    std::string userName = message.substr(pos1 + 1, pos2 - pos1 - 1);
    std::string messages = message.substr(pos2 + 1, message.length() - pos2);

    this->roomName = roomName;
    this->userName = userName;
    this->message = messages;
}
