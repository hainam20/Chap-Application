#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <ctime>
#include "Account.h"
#include "Message.h"

extern int currentID;
extern std::string FILE_NAME;

Account::Account()
{
}

std::string Account::getUsername()
{
    return username;
}
std::string Account::getPassword()
{
    return password;
}
std::string Account::getID()
{
    return ID;
}
int Account::getKey()
{
    return key;
}

void Account::setUsername(std::string username)
{
    this->username = username;
}
void Account::setPassword(std::string password)
{
    this->password = password;
}
void Account::setID(std::string ID)
{
    this->ID = ID;
}

bool Account::CheckID(const std::string &ID, const std::string &fileName)
{

    std::ifstream file(fileName);
    if (!file.is_open())
    {
        std::cout << "Error when opening file\n";
        return false;
    }
    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string username, password, id;
        if (std::getline(ss, username, ',') && std::getline(ss, password, ',') && std::getline(ss, id, ','))
        {
            // Kiểm tra ID
            if (id == ID)
            {
                file.close();
                return true;
            }
        }
    }
    file.close();
    return false;
}

void Account::logAccount(const std::string &username, int flagAccount)
{
    if (flagAccount == 0)
    {
        std::lock_guard<std::mutex> lock(accountsMutex);
        loggedInAccounts[username] = true;
    }
    if (flagAccount == 1)
    {
        std::lock_guard<std::mutex> lock(accountsMutex);
        loggedInAccounts[username] = false;
    }
}

/*
 *   Save account in file
 *   return -1 if account already exit
 *   return -2 if file cannot open
 *   return 1 save successfully
 */
int Account::insertAccount(const std::string &userName, const std::string &passWord, const int ID, const std::string &fileName, SOCKET clientSocket)
{
    Message msg;
    // Kiem tra user da ton tai trong he thong ?
    if (CheckAccount(userName, FILE_NAME) == true)
    {
        std::cout << "Username already exits\n";
        // Gui ma loi 403 thong bao cho user username da ton tai
        if (send(clientSocket, "403", 3, 0) < 0)
        {
            std::cout << "Error when send message" << std::endl;
        }
        return -1;
    }
    std::ofstream file(fileName, std::ios::app);
    if (!file.is_open())
    {
        std::cout << "Error when opening file\n";
        return -2;
    }

    // Ma hoa du lieu duoc ghi vao file
    std::string nameEncrypted = msg.encryptMessage(userName, "123");
    std::string pwdEncypted = msg.encryptMessage(passWord, "123");

    // file << userName<<","<<passWord<<","<<ID<<std::endl;
    file << nameEncrypted << "," << pwdEncypted << "," << ID << std::endl;
    file.close();
    std::cout << "Save Successfully\n";
    return 1;
}
int Account::CreateAccount(const std::string &username, const std::string &password, SOCKET clientSocket)
{
    // Khoi tao ID cho user
    currentID = generateID();
    // Goi ham insertAccount de them user vao he thong
    return insertAccount(username, password, currentID, FILE_NAME, clientSocket);
}

/*
 *   Find Account already exit or not
 *   return true if username already exit
 *   else return false
 */
bool CheckAccount(const std::string &userName, const std::string &fileName)
{
    Message msg;

    std::ifstream file(fileName);
    if (!file.is_open())
    {
        std::cout << "Error when opening file\n";
        return false;
    }
    std::string line;
    while (std::getline(file, line))
    {
        // Adding check encrypt data
        std::string encryptName = msg.encryptMessage(userName, "123");
        if (line.substr(0, userName.length()) == encryptName)
        {
            file.close();
            return true;
        }
    }
    file.close();
    return false;
}

/*
 *   Find Account already exit or not
 *   return true if username already exit
 *   else return false
 */
bool Account::CheckAccount(const std::string &userName, const std::string &fileName)
{
    Message msg;
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        std::cout << "Error when opening file\n";
        return false;
    }
    std::string line;
    while (std::getline(file, line))
    {
        // Adding check encrypt data
        std::string encryptName = msg.encryptMessage(userName, "123");
        if (line.substr(0, userName.length()) == encryptName)
        {
            file.close();
            return true;
        }
    }
    file.close();
    return false;
}

// check password when user login with username return true when password correct

bool Account::CheckLogIn(const std::string &username, const std::string &password, const std::string &fileName)
{
    Message msg;
    std::ifstream file(fileName);

    if (!file.is_open())
    {
        std::cout << "Error when opening file\n";
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        // adding check encrypt data
        std::string encryptName = msg.encryptMessage(username, "123");
        std::string encryptPass = msg.encryptMessage(password, "123");
        if (line.substr(0, username.length()) == encryptName)
        {

            if (line.substr(username.length() + 1, password.length()) == encryptPass)
            {
                std::cout << "Password correct";
                file.close();
                return true;
            }
        }
    }
    std::cout << "Password Invalid";
    return false;
}

/*
 *  Kiểm tra chuỗi message, tách chuỗi thành username, password, key để xử lý
 *  return username, password, key
 */
void Account::parseMessage(const std::string &message)
{
    size_t pos1 = message.find(',');
    size_t pos2 = message.rfind(',');
    if (pos1 == std::string::npos || pos2 == std::string::npos || pos1 == pos2)
    {
        throw std::invalid_argument("Invalid message format");
    }
    std::string username = message.substr(0, pos1);
    std::string password = message.substr(pos1 + 1, pos2 - pos1 - 1);
    int key = std::stoi(message.substr(pos2 + 1));
    this->username = username;
    this->password = password;
    this->key = key;
}

int Account::generateID()
{
    // Khởi tạo bộ sinh số ngẫu nhiên chỉ một lần
    static bool seedInitialized = false;
    if (!seedInitialized)
    {
        std::srand(std::time(0));
        seedInitialized = true;
    }

    std::string newID;
    do
    {
        int ID = 1000 + std::rand() % 9000;
        newID = std::to_string(ID);
    } while (CheckID(newID, FILE_NAME));

    return std::stoi(newID);
}