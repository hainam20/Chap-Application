#include "client.h"
#include <iostream>

#define IP_SERVER "192.168.1.13"
static const int PORT = 8080;

int endLine = 4;
std::string senderName;
std::string keyEncrypt = "hello";

// Constructor

Client::Client()
{
}
Client::~Client()
{
    closesocket(clientSocket);
    WSACleanup();
}

void Client::RESET_SOCKET()
{
    closesocket(clientSocket);
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

int Client::SOCKET_START(SOCKET &clientSocket)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        gui.notiBox("WSAStartup failed");
        return 1;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        gui.notiBox("Socket creation failed");
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(IP_SERVER);

    if (connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        gui.notiBox("Connection failed");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    return 0;
}

void Client::SET_UP_CONNECTION()
{
    int socketComplete;
    do
    {
        // IP_CONFIG();
        gui.loadingEffect("Connecting to server...");
        socketComplete = SOCKET_START(clientSocket);
    } while (socketComplete);
}

void Client::GENERATE_LOGIN()
{
    system("cls");
    gui.setColor(7);
    gui.hideCursor(false);
    std::string warnLabel = "Welcome user";
    std::string continueNoti = "Press 'r' to continue...";

    int width = 45;
    int height = 10;
    int top = 7;
    int left = gui.centerWindow(width);
    std::string checkResponse = "200";
    do
    {
        std::string chatLabel = "CHAT APPLICATION";
        std::string loginLabel = "LOGIN";

        system("cls");
        gui.gotoXY(gui.centerWindow(warnLabel.length() - 1), top + 9);
        std::cout << warnLabel;
        gui.drawRectangle(left, top, width, height);

        gui.gotoXY(gui.centerWindow(chatLabel.length() - 1), 3);
        std::cout << chatLabel;
        gui.gotoXY(gui.centerWindow(loginLabel.length() - 1), 5);
        std::cout << loginLabel;

        gui.gotoXY(left + 5, top + 3);
        std::cout << "Username: ";
        gui.gotoXY(left + 5, top + 6);
        std::cout << "Password: ";
        gui.gotoXY(left + 40, top + 6);
        std::cout << "[TAB]" << std::endl;
        gui.gotoXY(left + 16, top + 3);
        std::getline(std::cin >> std::ws, username);
        if (username == "/exit")
        {
            return FIRST_MENU();
        }
        gui.gotoXY(left + 16, top + 6);
        // std::getline(std::cin>>std::ws, client.password);
        password = inputPassword(left + 16, top + 6);
        std::string message = username + "," + password + ",1";

        send(clientSocket, message.c_str(), message.length(), 0);
        char buffer[1024] = {0};
        int recvSize = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (recvSize > 0)
        {
            std::string response(buffer, recvSize);
            if (response == "401")
            {
                warnLabel = "Invalid login name or password.";
            }
            checkResponse = response;
        }
    } while (checkResponse != "201"); // check validate
    senderName = username;
    return room.SELECT_ROOM();
}

void Client::START()
{
    SET_UP_CONNECTION();
    GENERATE_LOGIN();
}

std::string encryptMessage(const std::string &plaintext, const std::string &key)
{
    std::string ciphertext = plaintext;
    for (size_t i = 0; i < plaintext.size(); ++i)
    {
        ciphertext[i] = plaintext[i] ^ key[i % key.size()];
    }

    return ciphertext;
}

std::string decryptMessage(const std::string &ciphertext, const std::string &key)
{
    std::string decryptedtext = ciphertext;
    for (size_t i = 0; i < ciphertext.size(); ++i)
    {
        decryptedtext[i] = ciphertext[i] ^ key[i % key.size()];
    }

    return decryptedtext;
}

// std::string createLabel(const std::string &roomName, size_t size, char fillChar)
// {
//     size_t fillSize = (roomName.size() < size) ? (size - roomName.size()) / 2 : 0;

//     std::string fill(fillSize, fillChar);

//     std::string label = fill + " " + roomName + " " + fill;
//     if (label.size() < size)
//     {
//         label += fillChar;
//     }
//     roomNameDisplay = roomName;

//     return label;
// }

std::string labelUser(const std::string &line, const std::string &username)
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

// void Client::printMessage(std::string &message)
// {
//     int terminalWidth = gui.getTerminalWidth();
//     int currentLineWidth = 0;
//     std::string label = "=================================================";
//     for (char c : message)
//     {
//         if (c == '\n')
//         {
//             std::cout << c;
//             currentLineWidth = 0;
//         }
//         else
//         {
//             std::cout << c;
//             currentLineWidth++;

//             if (currentLineWidth >= terminalWidth / 2.5)
//             {
//                 gui.gotoXY(gui.centerWindow(label.length() + 1), ++endLine);
//                 currentLineWidth = 0;
//             }
//         }
//     }
// }

std::string Client::inputPassword(int x, int y)
{
    std::string password;
    bool showPassword = true;
    char ch;

    while (true)
    {
        ch = _getch();
        if (ch == 13)
        { // Enter key
            break;
        }
        else if (ch == 9)
        { // Tab key
            showPassword = !showPassword;
            gui.gotoXY(x, y);
            if (showPassword)
            {
                std::cout << password;
            }
            else
            {
                std::cout << std::string(password.length(), '*');
            }
            std::cout.flush();
        }
        else if (ch == 8)
        { // Backspace key
            if (!password.empty())
            {
                password.pop_back();
                gui.gotoXY(x + password.length(), y);
                std::cout << " \b";
            }
        }
        else
        {
            password += ch;
            if (showPassword)
            {
                std::cout << ch;
            }
            else
            {
                std::cout << '*';
            }
        }
    }
    return password;
}

void Client::GENERATE_SIGNUP()
{
    system("cls");
    gui.hideCursor(false);
    std::string newPassword = "/";
    std::string check;
    std::string warnLabel = "Welcome user !";
    std::string continueNoti = "Press 'r' to continue...";
    int width = 45;
    int height = 9;
    int top = 7;
    int left = gui.centerWindow(width);
    while (password != newPassword || check != "200")
    {
        std::string chatLabel = "CHAT APPLICATION";
        std::string loginLabel = "SIGN UP";

        system("cls");
        gui.gotoXY(gui.centerWindow(warnLabel.length()), top + 9);
        std::cout << warnLabel;
        gui.drawRectangle(left, top, width, height);

        gui.gotoXY(gui.centerWindow(chatLabel.length()), 3);
        std::cout << chatLabel;
        gui.gotoXY(gui.centerWindow(loginLabel.length()), 5);
        std::cout << loginLabel;

        gui.gotoXY(left + 5, top + 2);
        std::cout << "Username: ";
        gui.gotoXY(left + 5, top + 4);
        std::cout << "Password: ";
        gui.gotoXY(left + 5, top + 6);
        std::cout << "Confirm Password: ";
        gui.gotoXY(left + 40, top + 6);
        std::cout << "[TAB]" << std::endl;
        gui.gotoXY(left + 16, top + 2);
        std::cin >> username;
        if (username == "/exit")
        {
            return FIRST_MENU();
        }
        gui.gotoXY(left + 16, top + 4);
        password = inputPassword(left + 16, top + 4);

        // Check password strength
        std::regex password_regex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*])[A-Za-z\\d!@#$%^&*]{8,}$");
        if (!std::regex_match(password, password_regex))
        {
            warnLabel = "Password must be at least 8 characters";
            continue;
        }
        gui.gotoXY(left + 23, top + 6);
        newPassword = inputPassword(left + 23, top + 6);
        if (password != newPassword)
        {
            warnLabel = "Password does not match";
            continue;
        }

        std::string message = username + "," + password + ",0";
        send(clientSocket, message.c_str(), message.length(), 0);
        char buffer[1024] = {0};
        int recvSize = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (recvSize > 0)
        {
            std::string response(buffer, recvSize);
            check = response;
            if (response == "200")
            {
                gui.notiBox("Sign up completed");
            }
            else if (check == "403")
            {
                warnLabel = "Username is already existed";
                continue;
            }
            else if (check == "507")
            {
                warnLabel = "ERROR";
                continue;
            }
            else
            {
                gui.notiBox("Something wrong with server");
            }
        }
    }
    // global.employee.setEmployeePassword(newPassword);
    // Set password and login name for user here
    //  std::string message = client.username + "," + client.password + ",0";
    //  send(clientSocket, message.c_str(), message.length(), 0);
    //  system("cls");

    // char buffer[1024] = {0};
    // int recvSize = recv(clientSocket, buffer, sizeof(buffer), 0);
    // if (recvSize > 0) {
    //     std::string response(buffer, recvSize);
    //     if (response == "200") {
    //         notiBox("Sign up completed");
    //     }
    //     else if(response == "403"){
    //         notiBox("Username is already existed");
    //     }
    //     else {
    //         notiBox("Something wrong with server");
    //     }
    // }
    return GENERATE_LOGIN();
}

// void Client::IP_CONFIG()
// {
//     int IP_GENERATE = gui.optionMenu(9, "CHAT APPLICATION", "IP SERVER", 4, "192.168.37.110", "192.168.37.44", "192.168.37.40", "192.168.37.41");
//     switch (IP_GENERATE)
//     {
//     case 1:
//         IP_SERVER = "192.168.37.110";
//         break;
//     case 2:
//         IP_SERVER = "192.168.37.44";
//         break;
//     case 3:
//         IP_SERVER = "192.168.37.40";
//         break;
//     case 4:
//         IP_SERVER = "192.168.37.41";
//         break;
//     }
//     system("cls");
// }

// void LOAD_HISTORY_CHAT(std::string &chatDatabase, std::string &username)
// {
//     std::ifstream file(chatDatabase);
//     // if (!file.is_open()) {
//     //     std::cerr << "Failed to load history chat! " << chatDatabase << std::endl;
//     //     return;
//     // }

//     std::string line;
//     const int fixedWidth = 50;
//     std::string label = std::string((fixedWidth - username.length() - 2) / 2, '=') + " " + username + " " + std::string((fixedWidth - username.length() - 2) / 2, '=');

//     while (std::getline(file, line))
//     {
//         line = labelUser(line, username);
//         gui.gotoXY(gui.centerWindow(label.length()) - 1, ++endLine);
//         std::cout << line << std::endl;
//     }

//     file.close();
// }

// std::vector<std::string> readDatabaseFile(const std::string &fileName)
// {
//     std::vector<std::string> roomDatabase;
//     std::ifstream file(fileName);
//     if (!file)
//     {
//         std::cerr << "Unable to open file: " << fileName << std::endl;
//         return roomDatabase;
//     }

//     std::string line;
//     while (std::getline(file, line))
//     {
//         roomDatabase.push_back(line);
//     }

//     file.close();
//     return roomDatabase;
// }

// void HELP(int line, int len)
// {
//     gui.gotoXY(gui.centerWindow(len), line);
//     std::cout << "=== (This message is only show to you) ===" << std::endl;
//     gui.gotoXY(gui.centerWindow(len), line + 1);
//     std::cout << "- Emoji command: /smile, /sad, /wow, /scare." << std::endl;
//     gui.gotoXY(gui.centerWindow(len), line + 2);
//     std::cout << "- If you are host a private room, /key to see the key room" << std::endl;
//     gui.gotoXY(gui.centerWindow(len), line + 3);
//     std::cout << "- If you are in public room, /key will show nothing." << std::endl;
//     gui.gotoXY(gui.centerWindow(len), line + 4);
//     std::cout << "- /exit to out the room." << std::endl;
//     gui.gotoXY(gui.centerWindow(len), line + 5);
//     std::cout << "- The text box only contain no more than 50 words" << std::endl;
//     gui.gotoXY(gui.centerWindow(len), line + 6);
//     std::cout << ">> That's it - HAPPY CHATTING <<" << std::endl;
// }

void displayCurrentDay()
{
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm *localTime = std::localtime(&currentTime);
    std::cout << "Date: "
              << std::put_time(localTime, "%Y-%m-%d") << std::endl;
}

void Client::FIRST_MENU()
{
    int option = gui.optionMenu(8, "CHAT APPLICATION", "LOGIN OR SIGN UP", 2, "1. Login", "2. Sign up");
    switch (option)
    {
    case 1:
        GENERATE_LOGIN();
        break;
    case 2:
        GENERATE_SIGNUP();
        break;
    }
}

int main()
{
    GUI gui;
    Client client;
    gui.WINDOW_RESOLUTION();
    client.SET_UP_CONNECTION();
    client.FIRST_MENU();
}
