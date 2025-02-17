
#include <string>
#include <vector>
#include <thread>
#include <winsock2.h>
#include "GUI.h"

class RoomChat
{
private:
    std::string roomName;
    std::vector<std::string> chatHistory;
    std::vector<std::string> userList;
    GUI gui;
    std::string keyEncrypt;
    std::string chatDatabase = "";
    std::string keyRoom;
    void receiveMessages();
    void sendMessage(const std::string &message);
    void displayChat();
    void displayUserList();
    void saveChatHistory();
    std::string encryptMessage(const std::string &plaintext, const std::string &key);
    std::string decryptMessage(const std::string &ciphertext, const std::string &key);
    std::string createLabel(const std::string &roomName, size_t size, char fillChar);
    std::string labelUser(const std::string &line, const std::string &username);
    void HELP(int line, int len);
    void printMessage(std::string &message);
    std::vector<std::string> readDatabaseFile(const std::string &fileName);

public:
    RoomChat();
    void JOIN_CHAT(std::string roomName);
    void SELECT_ROOM();
    void ENTER_ROOM();
    void LIST_ROOM();
    void EXIT_ROOM();
    void GENERATE_ROOM();
    void PUBLIC_ROOM_CREATE();
    void PRIVATE_ROOM_CREATE();
    void INFO_TABLE();
    void LOAD_HISTORY_CHAT(std::string &chatDatabase, std::string &username);
    void REQUEST_TO_SERVER(std::string &chatDatabase, std::string roomName);
    std::string SHOW_AVAILABLE_ROOM(const std::string &chatDatabase);
    std::string GET_USERNAME();
    std::string GET_PASSWD();
};