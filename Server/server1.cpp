#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include<sstream>
#include<thread>

#pragma comment(lib, "ws2_32.lib")

//Global Varible:
HHOOK mouseHook;
HHOOK keyboardHook;
SOCKET clientSocket = INVALID_SOCKET;

//FUNCTION: Send data to Client:
void SendData(const std::string& data) {
    if (clientSocket != INVALID_SOCKET) {
        send(clientSocket, data.c_str(), data.size(), 0);
    }
}

//CallBack mouse's event:
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;
        int x = pMouseStruct->pt.x;
        int y = pMouseStruct->pt.y;

        //Send coordinate and event type (movement, click...)
        std::ostringstream oss;
        oss << "Mouse_Move: " << x << " , " << y << "\n";
        SendData(oss.str());
    }
    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* pKeyStruct = (KBDLLHOOKSTRUCT*)lParam;
        BYTE key = pKeyStruct->vkCode;

        //Send code to press key
        std::ostringstream oss;
        oss << "KEY " << static_cast<int>(key) << "\n";
        SendData(oss.str());
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

//Function to start Hook:
void StartHooks() {
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);

    if (!mouseHook || !keyboardHook) {
        std::cerr << "Errore durante l'inizializzazione degli Hook! " << std::endl;
        return;
    }

    //Loop to mantain Hook activate
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(mouseHook);
    UnhookWindowsHookEx(keyboardHook);
}
void StartServer() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(5000);

    bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(listenSocket, SOMAXCONN);

    std::cout << "Server in ascolto sulla porta 5000..." << std::endl;

    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientAddrSize);

    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Errore durante l'accettazione della connessione! " << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    std::cout << "Client connesso! " << std::endl;
}

int main() {
    //Start server with separate thread:
    std::thread serverThread(StartServer);
    serverThread.detach();


    std::cout << "Avvio del server completato. In attesa di connessione..." << std::endl;
    //Start hook Keyboard and mouse:
    StartHooks();

    // Mantieni il programma attivo
    while (true) {
        Sleep(1000);  // Loop infinito per mantenere il server attivo
    }

    return 0;
}