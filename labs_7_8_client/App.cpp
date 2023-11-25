#include <iostream>
#include <winsock2.h>
#include <thread>
#include <string>
#include <Windows.h>
#include <CommCtrl.h>

#pragma comment(lib, "Comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' \ name = 'Microsoft.Windows.Common-Controls' version = '6.0.0.0' \
processorArchitecture = '*' publicKeyToken = '6595b64144ccf1df' language = '*'\"")
#pragma once

#pragma warning(disable: 4996)
#pragma comment(lib, "WS2_32.lib")

#define ID_BUTTON_SEND 1000
#define EDIT_MESSAGE 1001

const int BUFFER_SIZE = 4096;
char buffer[BUFFER_SIZE];

HWND hwnd; // Handle to the main window
HWND hShowMessage;
HWND hWriteMessage;
HWND hSendButton;
SOCKET clientSocket;
const UINT WM_SOCKET_MESSAGE = WM_USER + 1; // Custom message for socket events

void ReceiveMessages(SOCKET clientSocket) {
    while (true) {
        int bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesRead <= 0) {
            PostMessage(hwnd, WM_SOCKET_MESSAGE, 0, 0);
            break;
        }
        std::string message = "\nServer: ";
        message += buffer;

        int bufferSize = GetWindowTextLength(hShowMessage) + 1;
        std::wstring history(bufferSize, L'\0');
        GetWindowText(hShowMessage, &history[0], bufferSize);

        size_t buffer_Size = history.length() + 1;
        char* nbuffer = new char[buffer_Size];
        size_t convertedChars = 0;
        wcstombs_s(&convertedChars, nbuffer, buffer_Size, history.c_str(), _TRUNCATE);
        std::string str_history(nbuffer);
        delete[] nbuffer;

        std::string new_history = str_history + message;
        SetWindowTextA(hShowMessage, new_history.c_str());

        memset(buffer, 0, BUFFER_SIZE);
    }
}

void SendMessages(SOCKET clientSocket, HWND hwnd) {
    WCHAR message[256];
    GetDlgItemText(hwnd, EDIT_MESSAGE, message, 256);
    int size = WideCharToMultiByte(CP_UTF8, 0, message, -1, nullptr, 0, nullptr, nullptr);
    char* c_message = new char[size];
    WideCharToMultiByte(CP_UTF8, 0, message, -1, c_message, size, nullptr, nullptr);
    if (send(clientSocket, c_message, size, 0) == SOCKET_ERROR) {
        MessageBox(hwnd, L"Failed to send data to the server", L"Error", MB_OK | MB_ICONERROR);
    }
    else {
        int bufferSize = GetWindowTextLength(hShowMessage) + 1;
        std::wstring history(bufferSize, L'\0');
        GetWindowText(hShowMessage, &history[0], bufferSize);

        size_t buffer_Size = history.length() + 1;
        char* buffer = new char[buffer_Size];
        size_t convertedChars = 0;
        wcstombs_s(&convertedChars, buffer, buffer_Size, history.c_str(), _TRUNCATE);
        std::string str_history(buffer);
        delete[] buffer;

        std::string str_message;
        str_message.assign(c_message);
        std::string full_message = "\nClient: " + str_message;

        std::string new_history = str_history + full_message;
        SetWindowTextA(hShowMessage, new_history.c_str());
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {         
    case WM_SOCKET_MESSAGE:
        MessageBox(hwnd, L"Connection closed by the server.", L"Message", MB_OK | MB_ICONERROR);
        break;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam)) 
        {
        case ID_BUTTON_SEND:
            SendMessages(clientSocket, hwnd);
            break;
        }
    }
    break;  
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    WSADATA wsaData;
    struct sockaddr_in serverAddress;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        MessageBox(NULL, L"Failed to initialize socket.", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    const wchar_t className[] = L"MyWindowClass";
    const wchar_t windowTitle[] = L"Chat Window";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        0,
        className,
        windowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        MessageBox(NULL, L"Failed to create the window.", L"Error", MB_OK | MB_ICONERROR);
        WSACleanup();
        return 1;
    }

    hShowMessage = CreateWindow(
        L"STATIC", L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        50, 50, 290, 300,
        hwnd, NULL, hInstance, NULL);

    hWriteMessage = CreateWindow(
        L"EDIT", L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        50, 400, 290, 25,
        hwnd, reinterpret_cast<HMENU>(EDIT_MESSAGE), hInstance, NULL);

    hSendButton = CreateWindow(L"BUTTON", L"Send", WS_VISIBLE | WS_CHILD, 350, 400, 75, 25, hwnd, (HMENU)ID_BUTTON_SEND, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);

    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        MessageBox(hwnd, L"Failed to create socket.", L"Error", MB_OK | MB_ICONERROR);
        WSACleanup();
        return 1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        MessageBox(hwnd, L"Failed to connect to the server.", L"Error", MB_OK | MB_ICONERROR);
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    SetWindowTextA(hShowMessage, "Connected to the server.");

    std::thread receiveThread(ReceiveMessages, clientSocket);
    //std::thread sendThread(SendMessages, clientSocket);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    receiveThread.join();
    //sendThread.join();

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}