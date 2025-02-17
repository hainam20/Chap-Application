#ifndef _ACCOUNT_H_
#define _ACCOUNT_H_

#include <string>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <winsock2.h>

class Account
{
private:
    std::string username;
    std::string password;
    std::string ID;
    std::unordered_map<std::string, bool> loggedInAccounts;
    std::mutex accountsMutex;
    int key;
    bool CheckID(const std::string &ID, const std::string &fileName);
    int generateID();
    int insertAccount(const std::string &userName, const std::string &passWord, const int ID, const std::string &fileName, SOCKET clientSocket);

public:
    Account();
    int CreateAccount(const std::string &username, const std::string &password, SOCKET clientSocket);
    bool CheckAccount(const std::string &userName, const std::string &fileName);
    void parseMessage(const std::string &message);
    void logAccount(const std::string &username, int flagAccount);
    bool CheckLogIn(const std::string &username, const std::string &password, const std::string &fileName);

    std::string getUsername();
    std::string getPassword();
    std::string getID();
    void setUsername(std::string username);
    void setPassword(std::string password);
    void setID(std::string ID);
    int getKey();
};

#endif // _ACCOUNT_H_