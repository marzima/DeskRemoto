#include <winsock2.h>    // Includi prima di <windows.h> per evitare conflitti
#include <ws2tcpip.h>    // Per inet_pton
#include <windows.h>     // Per SendInput
#include <iostream>
#include <sstream>
#include <thread>

#pragma comment(lib, "ws2_32.lib")  // Linka la libreria Winsock

int clientScreenWidth = GetSystemMetrics(SM_CXSCREEN);
int clientScreenHeight = GetSystemMetrics(SM_CYSCREEN);

// Variabili globali
HHOOK clientMouseHook;
SOCKET sock;

// Funzione per simulare il movimento del mouse
void SimulateMouseMove(int x, int y) {
    // Normalizza le coordinate per lo schermo del client
    double screenWidth = GetSystemMetrics(SM_CXSCREEN);
    double screenHeight = GetSystemMetrics(SM_CYSCREEN);
    double normalizedX = x * (65535.0 / screenWidth);
    double normalizedY = y * (65535.0 / screenHeight);

    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dx = static_cast<LONG>(normalizedX);
    input.mi.dy = static_cast<LONG>(normalizedY);
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    SendInput(1, &input, sizeof(INPUT));
}

// Funzione per simulare la pressione di un tasto
void SimulateKeyPress(BYTE key) {
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    SendInput(1, &input, sizeof(INPUT));
}

// Funzione per ricevere dati dal server
void ReceiveData(SOCKET sock) {
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) break;

        std::string data(buffer, bytesReceived);
        std::istringstream iss(data);
        std::string message;

        while (std::getline(iss, message)) {
            if (message.empty()) continue;

            if (message.find("MOUSE_MOVE:") == 0) {
                int x = std::stoi(message.substr(11, message.find(',')));
                int y = std::stoi(message.substr(message.find(',') + 1));
                SimulateMouseMove(x, y);
            }
            else if (message.find("KEY:") == 0) {
                BYTE key = static_cast<BYTE>(std::stoi(message.substr(4)));
                SimulateKeyPress(key);
            }
        }
    }
}

int main() {
    // In client.cpp (aggiungi all'inizio del main)

    std::cout << "Risoluzione client: " << clientScreenWidth << "x" << clientScreenHeight << std::endl;

    // Inizializza Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Errore durante l'inizializzazione di Winsock!" << std::endl;
        return 1;
    }

    // Crea il socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Errore nella creazione del socket!" << std::endl;
        WSACleanup();
        return 1;
    }

    // Configura l'indirizzo del server
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000);

    // Converti l'IP del server in formato binario
    if (inet_pton(AF_INET, "192.168.1.58", &serverAddr.sin_addr) <= 0) {
        std::cerr << "Indirizzo IP non valido!" << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Connetti al server
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Errore di connessione al server!" << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Connesso al server!" << std::endl;

    // Avvia il thread per ricevere i dati
    std::thread receiverThread(ReceiveData, sock);
    receiverThread.detach();

    // Mantieni il client attivo
    while (true) {
        Sleep(1000);
    }

    // Cleanup (mai raggiunto, ma necessario)
    closesocket(sock);
    WSACleanup();
    return 0;
}