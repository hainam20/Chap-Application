# 🚀 Chat Application in C++ (OOP)

## 📖 Introduction
This is a **Chat Application** built in C++ using Object-Oriented Programming (OOP). It allows users to:
- **Sign up / Log in** to the server.
- **Create public and private chat rooms.**
- **Send and receive messages** in real-time.
- **Manage user lists in chat rooms.**

The application is developed using **C++**, utilizing **TCP/IP Sockets** for communication between client and server, and **ANSI Escape Codes** for terminal-based UI on Linux.

---

## ⚙️ Features
### Client:
- Sign up, log in, and authenticate users with the server.
- Create and join public or private chat rooms (with room key).
- Real-time messaging within chat rooms.
- Display the list of users in the current chat room.

### Server:
- Manage multiple client connections using **std::thread**.
- Authenticate users and store account information securely.
- Manage public and private chat rooms.
- Broadcast messages to all users in a chat room.
- Optimized message processing with the updated `Account`, `Message`, and `RoomChat` classes.

### User Interface (GUI):
- Display notifications, loading effects, and option menus.
- Utilize **ANSI Escape Codes** for color-changing and cursor manipulation on Linux terminal.

---

## 🧩 Class Design
### 1. **Account:**
- Manages user authentication, registration, and login status.
- Uses file storage for account information.
- Supports parsing and validating message formats.

### 2. **Message:**
- Handles encryption and decryption of messages.
- Extracts usernames and message content.
- Manages message formatting for broadcasting.

### 3. **RoomChat:**
- Manages chat rooms (public and private).
- Handles joining, leaving, and notifying users in the room.
- Stores chat history and broadcasts messages within the room.

### 4. **Server:**
- Manages client connections using **std::thread**.
- Authenticates users and manages sessions.
- Coordinates messaging and broadcasting with `RoomChat`.

### 5. **GUI:**
- Displays terminal-based UI using **ANSI Escape Codes**.
- Provides loading effects, notification boxes, option menus, and other console effects.

---

## 📂 Project Structure
```
src/
├── client/
│   ├── Client_OOP.h       # Class declaration for Client
│   ├── Client_OOP.cpp     # Class implementation for Client
│   ├── RoomChat.h         # Class declaration for RoomChat
│   ├── RoomChat.cpp       # Class implementation for RoomChat
│   └── GUI_OOP.cpp        # Class implementation for GUI
└── server/
    ├── Server_OOP.h       # Class declaration for Server
    ├── Server_OOP.cpp     # Class implementation for Server
    └── main.cpp           # Main function to launch Server

src/
├── client/
│   ├── Client.h       # Class declaration for Client
│   ├── Client.cpp     # Class implementation for Client
│   ├── RoomChat.h         # Class declaration for RoomChat
│   ├── RoomChat.cpp       # Class implementation for RoomChat
│   └── GUI_OOP.cpp        # Class implementation for GUI
└── server/
    ├── Account.h        # Class declaration for Account
    ├── Account.cpp      # Class implementation for Account
    ├── Message.h        # Class declaration for Message
    ├── Message.cpp      # Class implementation for Message
    ├── Server.h         # Class declaration for Server
    ├── Server.cpp       # Class implementation for Server
```

---

## 💻 Installation and Execution
### 1. **Requirements:**
- `g++` compiler (supporting C++11 or later)
- `make` (optional)
- `Ws2_32` library for socket programming on Windows


### 2. **Compile and Run on Windows (MinGW):**
- **Compile Client:**
```sh
g++ -o client.exe main.cpp Account.cpp Message.cpp RoomChat.cpp -lWs2_32 -std=c++11 -pthread
```
- **Run Client:**
```sh
client.exe
```

- **Compile Server:**
```sh
g++ -o server.exe Server.cpp Account.cpp Message.cpp RoomChat.cpp -lWs2_32 -std=c++11 -pthread
```
- **Run Server:**
```sh
server.exe
```

---

## 🔨 Usage
### 1. **Start the Server:**
- In the terminal, navigate to the source directory and run:
```sh
server.exe # Windows
```
- The server will listen on port `8080`.

### 2. **Start the Client:**
- In another terminal, run:
```sh
client.exe # Windows
```
- Log in or sign up for a new account.
- Select `New room chat` to create a new room or `Available room chat` to join an existing room.
- Start chatting in the room.

---

## 🤝 Contribution Guidelines
If you'd like to contribute:
1. **Fork the project** and create a new branch (`feature/YourFeature`).
2. **Commit** your changes (`git commit -m 'Add YourFeature'`).
3. **Push** to your branch (`git push origin feature/YourFeature`).
4. **Create a Pull Request** on GitHub with a detailed description of your changes.

---

## 📜 License
This project is licensed under the **MIT License**. You are free to use, modify, and distribute this code.

---

## 📧 Contact
For questions or contributions, feel free to reach out:
- **Email:** hai-nam@example.com
- **GitHub:** [[https://github.com/your-github](https://github.com/hainam20)]

---

## ⭐ Acknowledgments
Special thanks to all contributors and open-source communities for their support and inspiration.
