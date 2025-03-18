#include <winsock2.h>
#include <windows.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

int main() {
    // Inizializza Winsock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // TODO: Aggiungi codice del server

    WSACleanup();
    return 0;
}