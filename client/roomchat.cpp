#include "RoomChat.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>

extern SOCKET clientSocket;
extern std::string senderName;

int endLine;
RoomChat::RoomChat()
{
    keyEncrypt = "hello";
}
void RoomChat::receiveMessages()
{
    char buffer[4096]; // Tăng kích thước buffer lên 4096 byte
    std::string messageAfter;
    std::string label = "=================================================";
    while (true)
    {
        int recvSize = recv(clientSocket, buffer, sizeof(buffer), 0);
        gui.gotoXY(gui.centerWindow(label.length()) - 1, ++endLine);
        if (recvSize > 0)
        {
            std::string message(buffer, recvSize);
            messageAfter = decryptMessage(message, keyEncrypt);
            std::cout << messageAfter << std::endl;
        }
        gui.gotoXY(22, 2);
    }
}

std::string RoomChat::encryptMessage(const std::string &plaintext, const std::string &key)
{
    std::string ciphertext = plaintext; // Khởi tạo chuỗi mã hóa với cùng độ dài như plaintext

    for (size_t i = 0; i < plaintext.size(); ++i)
    {
        ciphertext[i] = plaintext[i] ^ key[i % key.size()]; // XOR từng ký tự của plaintext với key
    }

    return ciphertext;
}

std::string RoomChat::decryptMessage(const std::string &ciphertext, const std::string &key)
{
    std::string decryptedtext = ciphertext;

    for (size_t i = 0; i < ciphertext.size(); ++i)
    {
        decryptedtext[i] = ciphertext[i] ^ key[i % key.size()];
    }

    return decryptedtext;
}

void RoomChat::sendMessage(const std::string &message)
{
    send(clientSocket, message.c_str(), message.length(), 0);
}

void RoomChat::displayChat()
{
    system("cls");
    gui.setColor(7);
    for (const auto &message : chatHistory)
    {
        std::cout << message << std::endl;
    }
    gui.gotoXY(0, 2);
    std::cout << "You: ";
}

void RoomChat::displayUserList()
{
    gui.gotoXY(60, 2);
    gui.setColor(6); // Màu vàng
    std::cout << "Users in room:";
    for (const auto &user : userList)
    {
        std::cout << "\n- " << user;
    }
}

void RoomChat::saveChatHistory()
{
    std::ifstream file(roomName + "_history.txt");
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            chatHistory.push_back(line);
        }
        file.close();
    }
}

// void RoomChat::saveChatHistory()
// {
//     std::ofstream file(roomName + "_history.txt");
//     if (file.is_open())
//     {
//         for (const auto &message : chatHistory)
//         {
//             file << message << std::endl;
//         }
//         file.close();
//     }
// }

void RoomChat::EXIT_ROOM()
{
    std::cout << "You have left the room: " << roomName << std::endl;
}

void RoomChat::JOIN_CHAT(std::string roomName)
{
    system("cls");
    gui.hideCursor(true);
    std::string messageBefore;
    int width = 80;
    int height = 1;
    int top = 1;
    int left = gui.centerWindow(width);
    char key;
    std::string label_s = "======================";
    std::string label = createLabel(roomName, 4, 'a');
    std::thread receiveThread(std::bind(&RoomChat::receiveMessages, this));
    receiveThread.detach();
    gui.gotoXY(gui.centerBox(width, label.length()), 4);
    std::cout << label;
    INFO_TABLE();
    std::string msg = "";
    gui.gotoXY(22, 2);
    // LOAD_HISTORY_CHAT(chatDatabase, senderName);
    while (msg != "/exit")
    {
        key = _getch();
        gui.drawRectangle(left, top, width, height);
        if (key == 't' || key == 'T')
        {
            gui.hideCursor(false);
            gui.gotoXY(23, 2);
            std::cout << "You: ";
            std::getline(std::cin >> std::ws, msg);
            if (msg == "/smile")
                msg = "(^_^)";
            else if (msg == "/sad")
                msg = "(T_T)";
            else if (msg == "/wow")
                msg = "(O.O)";
            else if (msg == "/scare")
                msg = "(|-.-|)";
            else if (msg == "/key")
                msg = keyRoom;
            else if (msg == "/help")
            {
                gui.gotoXY(gui.centerWindow(label.length()), ++endLine);
                HELP(endLine, label.length());
                endLine += 6;
                continue;
            }
            gui.gotoXY(23, 2); // Move cursor to the beginning of the input line
            for (size_t i = 0; i < msg.length() + 5; ++i)
            {                     // +5 to cover "You: " prefix
                std::cout << " "; // Overwrite with spaces
            }
            // Move cursor back to the beginning of the input line
            gui.gotoXY(27, 2);
            gui.gotoXY(gui.centerWindow(label.length()), ++endLine);
            std::cout << "You: ";

            printMessage(msg);
            std::string senderName;
            std::string fullMessage = roomName + "," + senderName + ": " + msg; // Tạo tin nhắn đầy đủ
            messageBefore = encryptMessage(fullMessage, keyEncrypt);
            send(clientSocket, messageBefore.c_str(), messageBefore.length(), 0);
        }
        gui.removeRectangle(left, top, width, height);
        }
    return SELECT_ROOM();
}

void RoomChat::SELECT_ROOM()
{
    int option = gui.optionMenu(8, "CHAT APPLICATION", "WELCOME TO CHAT", 2, "1. New room chat", "2. Available room chat");
    switch (option)
    {
    case 1:
        GENERATE_ROOM();
        break;
    case 2:
        ENTER_ROOM();
        break;
    case 3:
        exit(1);
        break;
    }
}

void RoomChat::GENERATE_ROOM()
{
    int option = gui.optionMenu(8, "CHAT APPLICATION", "CHOOSE ROOM CATEGORIZE", 2, "1. Public room (Everyone can join)", "2. Private room (Join with key)");
    switch (option)
    {
    case 1:
        PUBLIC_ROOM_CREATE();
        break;
    case 2:
        PRIVATE_ROOM_CREATE();
        break;
    case 3:
        return SELECT_ROOM();
        break;
    }
}

void RoomChat::PUBLIC_ROOM_CREATE()
{
    std::string roomName = gui.getTextElementBox("Enter room name");
    std::string roomAssignToServer = "cr00m," + roomName;
    send(clientSocket, roomAssignToServer.c_str(), roomAssignToServer.length(), 0);

    char buffer[1024] = {0};
    int recvSize = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (recvSize > 0)
    {
        std::string response(buffer, recvSize);
        if (response == "603")
        {
            gui.notiBox("Room name is already existed");
            return SELECT_ROOM();
        }
        else if (response == "601")
        {
            gui.notiBox("Now you can start the conversation");
            REQUEST_TO_SERVER(chatDatabase, roomName);
            // roomNameDisplay = roomName;
            JOIN_CHAT(roomName);
        }
    }
}

void RoomChat::PRIVATE_ROOM_CREATE()
{
    std::string roomName = gui.getTextElementBox("Enter room name");
    std::string roomAssignToServer = "cr00mP," + roomName;
    send(clientSocket, roomAssignToServer.c_str(), roomAssignToServer.length(), 0);
    char buffer[1024] = {0};
    int recvSize = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (recvSize > 0)
    {
        std::string response(buffer, recvSize);
        if (response == "603")
        {
            gui.notiBox("Room name is already existed");
            return SELECT_ROOM();
        }
        else if (response == "602")
        {
            gui.notiBox("Cannot join room due to specific error");
            return SELECT_ROOM();
        }
        else
        {
            size_t commaPos = response.find(',');
            std::string prefix = response.substr(0, commaPos);
            keyRoom = response.substr(commaPos + 1);
            gui.notiBox("YOUR ROOM KEY IS: " + keyRoom);
            // std::string userValidateRoomKey = getTextElementBox("ENTER KEY ROOM");
            // if(userValidateRoomKey == key){
            gui.notiBox("Now you can start the conversation");
            REQUEST_TO_SERVER(chatDatabase, roomName);
            // roomNameDisplay = roomName;
            JOIN_CHAT(roomName);
            // }
            // else{
            //     notiBox("Invalid Key");
            //     return SELECT_ROOM();
            // }
        }
    }
}

void RoomChat::ENTER_ROOM()
{
    std::string roomName = SHOW_AVAILABLE_ROOM("D://Chat-Application-main//server//Room.txt");
    std::string roomAssignToServer = "ar00m," + senderName + std::string(",") + roomName;

    send(clientSocket, roomAssignToServer.c_str(), roomAssignToServer.length(), 0);
    char buffer[1024] = {0};
    int recvSize = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (recvSize > 0)
    {
        std::string response(buffer, recvSize);
        if (response == "601")
        {
            gui.notiBox("Now you can start the conversation");
            REQUEST_TO_SERVER(chatDatabase, roomName);
            JOIN_CHAT(roomName);
        }
        else if (response == "666")
        {
            std::string userValidateRoomKey = gui.getTextElementBox("ENTER KEY ROOM");
            send(clientSocket, userValidateRoomKey.c_str(), userValidateRoomKey.length(), 0);
            char bufferP[1024] = {0};
            int recvSizeP = recv(clientSocket, bufferP, sizeof(bufferP), 0);
            if (recvSizeP > 0)
            {
                std::string response(bufferP, recvSizeP);
                if (response == "601")
                {
                    gui.notiBox("Now you can start the conversation");
                    REQUEST_TO_SERVER(chatDatabase, roomName);
                    // roomNameDisplay = roomName;
                    JOIN_CHAT(roomName);
                }
                else
                {
                    gui.notiBox("Invalid Key");
                    return SELECT_ROOM();
                }
            }
            // if(userValidateRoomKey == key){
            //     notiBox("Now you can start the conversation");
            //     REQUEST_TO_SERVER(chatDatabase, roomName);
            //     JOIN_CHAT(roomName);
            // }
            // else{
            //     notiBox("Invalid Key");
            //     return SELECT_ROOM();
            // }
        }
        else
        {
            gui.notiBox("Cannot join room due to specific error");
            return SELECT_ROOM();
        }
    }
}

void RoomChat::INFO_TABLE()
{
    gui.gotoXY(6, 10);
    int left = 1, top = 4, width = 30, height = 10;
    int inlin = left + 1;
    std::string label = "ROOM INFO";
    gui.drawRectangle(left, top, width, height);
    gui.gotoXY(inlin + 8, 5);
    std::cout << label;
    gui.gotoXY(inlin, 7);
    std::cout << "Room name: " << roomName;
    gui.gotoXY(inlin, 9);
    // displayCurrentDay();
    gui.gotoXY(inlin, 11);
    std::cout << "/help for more detail.";
    gui.gotoXY(inlin, 14);
    std::cout << "Test version: 1.37 (Commit).";
}

void RoomChat::LOAD_HISTORY_CHAT(std::string &chatDatabase, std::string &username)
{
    std::ifstream file(chatDatabase);
    if (!file.is_open())
    {
        std::cerr << "Failed to load history chat! " << chatDatabase << std::endl;
        return;
    }

    std::string line;
    const int fixedWidth = 50;
    std::string label = std::string((fixedWidth - username.length() - 2) / 2, '=') + " " + username + " " + std::string((fixedWidth - username.length() - 2) / 2, '=');

    while (std::getline(file, line))
    {
        line = labelUser(line, username);
        gui.gotoXY(gui.centerWindow(label.length()) - 1, ++endLine);
        std::cout << line << std::endl;
    }

    file.close();
}

void RoomChat::REQUEST_TO_SERVER(std::string &chatDatabase, std::string roomName)
{
    chatDatabase = "D://Chat-Application-main//server//" + roomName;
}

std::string RoomChat::SHOW_AVAILABLE_ROOM(const std::string &chatDatabase)
{
    system("cls");
    std::vector<std::string> roomDatabase = readDatabaseFile(chatDatabase);
    std::string returnRoom;
    std::string chatLabel = "PICK YOUR ROOM AND HAVE FUN";
    int counter = 1, width = 30, height = roomDatabase.size() + 7, top = 7, lefts = gui.centerWindow(width), leftBox = gui.centerBox(width, 3), geti;
    char key;
    int pageCounter = roomDatabase.size();
    std::vector<int> initMenuColor;
    for (int i = 0; i < roomDatabase.size() + 3; i++)
    {
        initMenuColor.push_back(7);
    }
    while (true)
    {
        if (roomDatabase.empty())
        {
            gui.notiBox("No room now available now");
            SELECT_ROOM();
        }
        else
        {
            gui.setColor(7);
            gui.gotoXY(gui.centerWindow(chatLabel.length() - 1), 3);
            std::cout << chatLabel;
            gui.drawRectangle(lefts, top, width, height);
            for (int i = 0; i < roomDatabase.size(); i++)
            {
                gui.gotoXY(leftBox, top + 2 + i + 1);
                gui.setColor(initMenuColor[i]);
                std::cout << roomDatabase[i];
                geti = i;
            }
            gui.gotoXY(gui.centerWindow(3), top + 5 + geti + 1);
            gui.setColor(initMenuColor[geti + 1]);
            std::cout << "Exit";
            // std::cout << counter << std::endl;
            // std::cout << pageCounter << std::endl;
            key = _getch();
            if (key == 'w' && counter >= 2 && counter <= roomDatabase.size() + 3)
            {
                counter--;
            }
            if (key == 's' && counter >= 1 && counter <= roomDatabase.size() + 2)
            {
                counter++;
            }
            if (key == '\r')
            {
                if (counter == pageCounter + 1)
                {
                    SELECT_ROOM();
                }
                else
                {
                    returnRoom = roomDatabase[counter - 1];
                    return returnRoom;
                }
            }
            for (int i = 0; i < roomDatabase.size() + 3; i++)
            {
                initMenuColor[i] = 7;
            }
            initMenuColor[counter - 1] = 3;
        }
    }
}

std::string RoomChat::createLabel(const std::string &roomName, size_t size, char fillChar)
{
    size_t fillSize = (roomName.size() < size) ? (size - roomName.size()) / 2 : 0;

    std::string fill(fillSize, fillChar);

    std::string label = fill + " " + roomName + " " + fill;
    if (label.size() < size)
    {
        label += fillChar;
    }
    // roomNameDisplay = roomName;

    return label;
}

std::string RoomChat::labelUser(const std::string &line, const std::string &username)
{
    size_t pos = line.find(":");
    if (pos != std::string::npos)
    {
        std::string name = line.substr(0, pos);
        if (name == username)
        {
            return "You" + line.substr(pos);
        }
    }
    return line;
}

void RoomChat::HELP(int line, int len)
{
    gui.gotoXY(gui.centerWindow(len), line);
    std::cout << "=== (This message is only show to you) ===" << std::endl;
    gui.gotoXY(gui.centerWindow(len), line + 1);
    std::cout << "- Emoji command: /smile, /sad, /wow, /scare." << std::endl;
    gui.gotoXY(gui.centerWindow(len), line + 2);
    std::cout << "- If you are host a private room, /key to see the key room" << std::endl;
    gui.gotoXY(gui.centerWindow(len), line + 3);
    std::cout << "- If you are in public room, /key will show nothing." << std::endl;
    gui.gotoXY(gui.centerWindow(len), line + 4);
    std::cout << "- /exit to out the room." << std::endl;
    gui.gotoXY(gui.centerWindow(len), line + 5);
    std::cout << "- The text box only contain no more than 50 words" << std::endl;
    gui.gotoXY(gui.centerWindow(len), line + 6);
    std::cout << ">> That's it - HAPPY CHATTING <<" << std::endl;
}

void RoomChat::printMessage(std::string &message)
{
    int terminalWidth = gui.getTerminalWidth();
    std::cout << message << std::endl;
    int currentLineWidth = 0;
    // std::cout << message << std::endl;

    std::string label = "=================================================";
    for (char c : message)
    {
        if (c == '\n')
        {
            std::cout << c;
            currentLineWidth = 0;
        }
        else
        {
            std::cout << c;
            currentLineWidth++;

            if (currentLineWidth >= terminalWidth / 2.5)
            {
                gui.gotoXY(gui.centerWindow(label.length() + 1), ++endLine);
                currentLineWidth = 0;
            }
        }
    }
}

std::vector<std::string> RoomChat::readDatabaseFile(const std::string &fileName)
{
    std::vector<std::string> roomDatabase;
    std::ifstream file(fileName);
    if (!file)
    {
        std::cerr << "Unable to open file: " << fileName << std::endl;
        return roomDatabase;
    }

    std::string line;
    while (std::getline(file, line))
    {
        roomDatabase.push_back(line);
    }

    file.close();
    return roomDatabase;
}
