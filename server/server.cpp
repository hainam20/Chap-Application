#include "server.h"
#include "Message.h"
#include "Account.h"

std::unordered_map<std::string, bool> loggedInAccounts; // Dùng để quản lý login logout của account
std::mutex accountsMutex;
std::mutex loginMutex; // Mutex mới để quản lý trạng thái đăng nhập
int currentID;         // Biến toàn cục để theo dõi ID hiện tại

Server::Server() {}
/**
 *
 * @param room name, clientSocket IP address of client
 *
 */
void Server::roomChat(std::string room, SOCKET clientSocket)
{
    auto check = std::find(chatroom[room].begin(), chatroom[room].end(), clientSocket);
    if (check == chatroom[room].end())
    {
        chatroom[room].emplace_back(clientSocket);
    }
    else
    {
        std::cout << "Client exist" << std::endl;
    }
}

/**
 * Send notify when someone join room
 */
void Server::notifyJoin(std::string username, const std::vector<SOCKET> &clients, const SOCKET senderSocket)
{
    Message msg;
    std::string notify = "              " + username + " is joining room";
    notify = msg.encryptMessage(notify, keyEcrypt);
    for (SOCKET client : clients)
    {
        if (client != senderSocket)
        {
            send(client, notify.c_str(), notify.length(), 0);
        }
    }
}

/**
 * save room name when user create
 */
int Server::saveRoom(std::string roomName)
{
    std::ofstream file("Room.txt", std::ios::app);
    if (!file.is_open())
    {
        std::cout << "Error when opening file\n";
        return -2;
    }
    file << roomName << std::endl;
    file.close();
    std::cout << "Save Successfully\n";
    return 0;
}
/**
 * When server ON need to load all room in vecto
 */
int Server::loadRoom(std::string fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        std::cout << "Error when opening file\n";
        return -1;
    }
    std::string line;
    while (std::getline(file, line))
    {
        chatroom[line];
    }
    file.close();

    return 1;
}
/**
 * Check room exist or not
 */
bool Server::checkRoom(std::string roomName)
{
    return chatroom.find(roomName) != chatroom.end();
}

// Send message
void Server::BroadcastMessage(const std::string &message, const std::vector<SOCKET> &clients, const SOCKET senderSocket)
{
    Message msg;
    std::lock_guard<std::mutex> lock(clientsMutex);
    std::string senderMessage;
    senderMessage = msg.decryptMessage(message, keyEcrypt);
    msg.splitMessage(senderMessage);

    std::string extractedUsername = msg.extractUsername(msg.getMessage());
    // std::cout << "Extracted Username: " << extractedUsername << std::endl; // Debug

    std::string actualMessage = msg.extractActualMessage(msg.getMessage());
    // std::cout << "Actual Message:" << actualMessage << std::endl; // Debug
    std::string msgSend = msg.encryptMessage(msg.getMessage(), keyEcrypt);

    // Với câu lệnh "./exit", tức client sẽ ngắt kết nối với server
    if (actualMessage == " /exit")
    {
        // Gui thong bao cho cac user khac
        std::string notify = "              " + extractedUsername + " exit room";
        notify = msg.encryptMessage(notify, keyEcrypt);
        for (SOCKET client : clients)
        {
            if (client != senderSocket)
            {
                send(client, notify.c_str(), notify.length(), 0);
            }
        }
    }
    else
    {
        // Gui tin nhan cho cac user khac
        for (SOCKET client : clients)
        {
            if (client != senderSocket)
            {
                send(client, msgSend.c_str(), msgSend.length(), 0);
            }
        }
    }
}

// Hàm handle cac client
void Server::HandleClient(SOCKET clientSocket)
{
    Message msg;
    char buffer[2048] = {0};
    // Bien UserName de luu username tu user
    std::string UserName;
    int recvSize;
    bool loginSuccessful = false;
    try
    {
        while (recvSize = recv(clientSocket, buffer, sizeof(buffer), 0))
        {
            std::string message(buffer, recvSize);
            // Kiem tra tin nhan, tach lay username, password va key
            Account account;
            account.parseMessage(message);

            // TH1: Key = 0: Server tien hanh tao tai khoan cho user
            if (account.getKey() == 0)
            {
                // Neu viec khoi tao thanh cong, gui ma 200 bao cho user
                if (account.CreateAccount(account.getUsername(), account.getPassword(), account.getKey()) > 0)
                {
                    send(clientSocket, "200", 3, 0);
                }
                else
                {
                    std::cout << "HAHA" << std::endl;
                    continue;
                }
            }

            // TH2: Key = 1: Server doi user dang nhap
            else if (account.getKey() == 1)
            {
                // Xac thuc tai khoan co trong he thong
                if (account.CheckAccount(account.getUsername(), FILE_NAME) == true && account.CheckLogIn(account.getUsername(), account.getPassword(), FILE_NAME) == true && loggedInAccounts[account.getUsername()] == false)
                {
                    // Neu thanh cong, lay user luu vao UserName phuc vu viec gui, nhan tin nhan
                    UserName = account.getUsername();
                    std::cout << "Account login: " << account.getUsername() << std::endl;
                    // Gui ma 201 thong bao thanh cong cho user
                    send(clientSocket, "201", 3, 0);
                    // Danh dau user da dang nhap
                    account.logAccount(UserName, 0);
                    loginSuccessful = true;
                    // break;
                }
                else
                {
                    // Nguoc lai, gui ma 401 cho user thong bao dang nhap khong thanh cong
                    std::cout << "Account login failed: " << account.getUsername() << std::endl;
                    send(clientSocket, "401", 3, 0);
                }
            }
            // Các trường hợp còn lại in thông báo lỗi ra màn hìnhplitMessage
            else
            {
                std::cout << "Invalid key value" << std::endl;
                closesocket(clientSocket);
                return;
            }

            // Cho client chon room
            if (loginSuccessful)
            {
                bool joinRoomSuccesful = false;
                while (!joinRoomSuccesful)
                {
                    while (recvSize = recv(clientSocket, buffer, sizeof(buffer), 0))
                        ;

                    std::string request, username, room;
                    std::string roomAssignFromClient(buffer, recvSize);
                    size_t delimiterPos = roomAssignFromClient.find(',');
                    size_t delimiterPos2 = roomAssignFromClient.rfind(',');
                    std::cout << buffer << std::endl
                              << delimiterPos << std::endl
                              << delimiterPos2 << std::endl;
                    if (delimiterPos != std::string::npos || delimiterPos2 != std::string::npos)
                    {
                        request = roomAssignFromClient.substr(0, delimiterPos);
                        username = roomAssignFromClient.substr(delimiterPos + 1, delimiterPos2 - delimiterPos - 1);
                        room = roomAssignFromClient.substr(delimiterPos2 + 1);
                    }
                    else
                    {
                        // Neu khong kiem tra duoc dau phay trong chuoi
                        std::cout << "Undefined request from client !" << std::endl;
                        continue;
                    }
                    if (request == "cr00m")
                    {
                        if (checkRoom(room))
                        {
                            send(clientSocket, "603", 3, 0);
                            std::cout << "Room exist\n";
                        }
                        else
                        {
                            std::cout << "Creat room chat\n";
                            roomAvailable.insert(room);
                            roomChat(room, clientSocket);
                            saveRoom(room);
                            send(clientSocket, "601", 3, 0);
                            joinRoomSuccesful = true;
                        }
                    }
                    else if (request == "cr00mP")
                    {
                        if (checkRoom(room))
                        {
                            send(clientSocket, "603", 3, 0);
                        }
                        // else{
                        //     roomAvailable.insert(room);
                        //     roomChat(room, clientSocket);
                        //     saveRoom(room);
                        //     send(clientSocket, "601", 3, 0);
                        //     joinRoomSuccesful = true;
                        // }
                        std::string roomKey = generateKey();
                        roomPrivate[roomKey] = room;
                        roomAvailable.insert(room);
                        roomPrivateAllocate.insert(room);
                        roomChat(room, clientSocket);
                        saveRoom(room);
                        std::string prefix = "601,";
                        std::string packageSend = prefix + roomKey;
                        std::cout << packageSend << std::endl;
                        const char *send_m = packageSend.c_str();
                        send(clientSocket, send_m, 10, 0);
                        // send(clientSocket, "601", 3, 0);
                        joinRoomSuccesful = true;
                    }

                    else if (request == "ar00m")
                    {
                        if ((roomPrivateAllocate.find(room) == roomPrivateAllocate.end()) && (chatroom.find(room) != chatroom.end()))
                        {
                            send(clientSocket, "601", 3, 0);
                            notifyJoin(username, chatroom[room], clientSocket);
                            roomChat(room, clientSocket);
                            joinRoomSuccesful = true;
                        }
                        else if (roomPrivateAllocate.find(room) != roomPrivateAllocate.end())
                        {
                            char bufferVal[1024];
                            send(clientSocket, "666", 3, 0);
                            int checkRec = recv(clientSocket, bufferVal, sizeof(bufferVal), 0);
                            if (checkRec > 0)
                            {
                                std::string validate(bufferVal, checkRec);
                                if (roomPrivate.find(validate) != roomPrivate.end())
                                {
                                    notifyJoin(username, chatroom[room], clientSocket);
                                    roomChat(room, clientSocket);
                                    joinRoomSuccesful = true;
                                    send(clientSocket, "601", 3, 0);
                                }
                                else
                                {
                                    send(clientSocket, "602", 3, 0);
                                }
                            }
                        }
                        else
                        {
                            send(clientSocket, "602", 3, 0);
                        }
                    }
                    else
                    {
                        send(clientSocket, "602", 3, 0);
                    }
                }
                break;
            }
        }
        // hjgjh
        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.push_back(clientSocket);
        }

        bool join = true;
        // Xử lý phần gửi nhận tin nhắn
        while (true)
        {
            recvSize = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (recvSize > 0)
            {
                std::string clientMessage(buffer, recvSize);
                std::string historyChat = clientMessage;
                std::cout << "Encrypt: " << historyChat << std::endl;
                historyChat = msg.decryptMessage(historyChat, keyEcrypt);
                Message fullMessage;
                fullMessage.splitMessage(historyChat);

                std::string extract_username = msg.extractUsername(fullMessage.getMessage());
                // std::cout << "Extracted Username: " << extract_username << std::endl; // Debug

                std::string actual_message = msg.extractActualMessage(fullMessage.getMessage());
                // std::cout << "Actual Message:" << actual_message << std::endl; // Debug

                // ghi lai thong bao chat
                std::ofstream chatHistory(fullMessage.getRoomName(), std::ios::app);
                if (actual_message == " /exit")
                {
                    std::cout << "Check function when get /exit" << std::endl;
                    std::string notifySaved = "              " + extract_username + " exit room";
                    chatHistory << notifySaved + '\n'; // chuyen tin nhan thanh chuoi binary va luu vao file
                    chatHistory.close();
                }
                else
                {
                    std::cout << "Check function when not get /exit" << std::endl;
                    chatHistory << fullMessage.getMessage() + '\n';
                    chatHistory.close();
                }

                std::string messageDecrypt = msg.encryptMessage(fullMessage.getMessage(), keyEcrypt);
                BroadcastMessage(clientMessage, chatroom[fullMessage.getRoomName()], clientSocket); // gọi hàm gửi tin nhắn cho các client khác
            }
            else
            {
                // Lock the mutex before erasing from clients vector
                {
                    std::lock_guard<std::mutex> lock(clientsMutex);
                    auto it = std::remove_if(clients.begin(), clients.end(), [clientSocket](SOCKET s)
                                             { return s == clientSocket; });
                    clients.erase(it, clients.end());
                }
                closesocket(clientSocket);
                break;
            }
        }

        // Remove the client from the clients list and close the socket
        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
        }

        // Ghi nhan tai khoan da thoat, bat flag = 1
        // account.logAccount(UserName, 1);
        closesocket(clientSocket);
    }
    catch (...)
    {
        std::cerr << "Maybe all user is out or unknown error - Server waiting for connection..." << std::endl;
        if (loggedInAccounts.find(UserName) != loggedInAccounts.end())
        {
            loggedInAccounts.erase(UserName);
        }
        closesocket(clientSocket);
        return;
    }
}

std::string Server::generateKey()
{
    std::string key;
    srand(time(nullptr));
    const char charset[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789";
    const size_t maxIndex = sizeof(charset) - 1;
    for (size_t i = 0; i < 6; ++i)
    {
        key += charset[rand() % maxIndex];
    }
    return key;
}

// Hàm khởi động Servere
int Server::StartServer()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ipServer.c_str()); // Replace with your server IP address
    serverAddr.sin_port = htons(8080);                        // Replace with your server port number

    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is listening on port 8080..." << std::endl;

    try
    {
        while (true)
        {
            SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET)
            {
                std::cerr << "Accept failed" << std::endl;
                closesocket(serverSocket);
                WSACleanup();
                return 1;
            }
            try
            {
                // std::thread clientThread(HandleClient, clientSocket);
                std::thread clientThread(&Server::HandleClient, this, clientSocket);
                clientThread.detach();
            }
            catch (const std::exception &e)
            {
                std::cerr << "Client handling thread creation failed: " << e.what() << std::endl;
                closesocket(clientSocket);
            }
            catch (...)
            {
                std::cerr << "Unknown error occurred while creating client handling thread" << std::endl;
                closesocket(clientSocket);
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Server encountered an error: " << e.what() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    catch (...)
    {
        std::cerr << "Server encountered an unknown error" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}

// Hàm main
int main()
{
    Server server;
    int choose;
    std::cout << "Choose Server: " << std::endl;
    std::cout << "[1] Server 1 (44)" << std::endl;
    std::cout << "[2] Server 2 (41)" << std::endl;
    std::cout << "[3] Server 3 (110)" << std::endl;
    std::cout << "[4] Server 4 (40)" << std::endl;
    std::cin >> choose;
    switch (choose)
    {
    case 1:
        ipServer = "192.168.80.1";
        break;
    case 2:
        ipServer = "192.168.1.13";
        break;
    case 3:
        ipServer = "192.168.37.110";
        break;
    case 4:
        ipServer = "192.168.37.40";
        break;
    default:
        break;
    }
    server.StartServer();
}
