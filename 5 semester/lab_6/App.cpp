#include <Windows.h>
#include <cmath>
#include <string>
#include <CommCtrl.h>

#pragma comment(lib, "Comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' \ name = 'Microsoft.Windows.Common-Controls' version = '6.0.0.0' \
processorArchitecture = '*' publicKeyToken = '6595b64144ccf1df' language = '*'\"")
#pragma once

HWND g_hMainWindow;
HWND g_hProgressLabel;
HWND g_hProgressBar;
HANDLE g_hCalculationThread;
bool g_bCalculationInProgress = false;
HANDLE g_hSemaphore;
HANDLE g_hMutex;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
double CalculateIntegral();
void UpdateProgress(double progress);
DWORD WINAPI CalculationThreadProc(LPVOID lpParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    InitCommonControls();

    g_hMainWindow = CreateWindowEx(0, L"Sample Window Class", L"Integration Calculation",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        350, 150, NULL, NULL, hInstance, NULL);

    if (g_hMainWindow == NULL)
    {
        return 0;
    }

    ShowWindow(g_hMainWindow, nCmdShow);
    UpdateWindow(g_hMainWindow);

    // Инициализация семафора
    g_hSemaphore = CreateSemaphore(NULL, 1, 1, NULL);

    // Инициализация мьютекса
    g_hMutex = CreateMutex(NULL, FALSE, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Закрытие семафора и мьютекса
    CloseHandle(g_hSemaphore);
    CloseHandle(g_hMutex);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        g_hProgressLabel = CreateWindowEx(0, L"STATIC", L"Progress: 0%",
            WS_VISIBLE | WS_CHILD, 10, 10, 200, 20, hwnd, NULL, NULL, NULL);

        g_hProgressBar = CreateWindowEx(0, PROGRESS_CLASS, NULL,
            WS_VISIBLE | WS_CHILD, 10, 40, 300, 20, hwnd, NULL, NULL, NULL);

        CreateWindowEx(0, L"BUTTON", L"Start", WS_VISIBLE | WS_CHILD,
            10, 70, 75, 25, hwnd, reinterpret_cast<HMENU>(1), NULL, NULL);
    }
    return 0;

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == 1)  // ID кнопки "Start"
        {
            if (!g_bCalculationInProgress)
            {
                // Запуск потока вычисления
                g_hCalculationThread = CreateThread(NULL, 0, CalculationThreadProc, NULL, 0, NULL);
            }
            else
            {
                MessageBox(hwnd, L"Calculation already in progress!", L"Error", MB_OK | MB_ICONERROR);
            }
        }
    }
    return 0;

    case WM_DESTROY:
    {
        if (g_bCalculationInProgress)
        {
            MessageBox(hwnd, L"Calculation in progress. Please wait...", L"Warning", MB_OK | MB_ICONWARNING);
            return 0;
        }

        if (g_hCalculationThread)
        {
            CloseHandle(g_hCalculationThread);
            g_hCalculationThread = NULL;
        }

        PostQuitMessage(0);
    }
    return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Функция вычисления интеграла
double CalculateIntegral()
{
    const double lowerBound = 0.0;
    const double upperBound = 1;

    const int numSteps = 500000000;  // Количество шагов

    double stepSize = (upperBound - lowerBound) / numSteps;
    double integral = 0.0;
    double proc = 0.0;

    for (int i = 0; i < numSteps; ++i)
    {
        double x = lowerBound + (i + 0.5) * stepSize;
        integral += std::sin(x) * stepSize;

        if (i % 50000 == 0)
        {
            proc = double(i) / 500000000;
            UpdateProgress(proc);
        }
    }
    UpdateProgress(1);

    return integral;
}

// Функция обновления прогресса вычисления
void UpdateProgress(double progress)
{
    int progressValue = static_cast<int>(progress * 100);

    // Использование семафора для контроля доступа к функции обновления прогресса
    WaitForSingleObject(g_hSemaphore, INFINITE);

    SendMessage(g_hProgressBar, PBM_SETPOS, progressValue, 0);

    std::wstring progressText = L"Progress: " + std::to_wstring(progressValue) + L"%";
    SetWindowText(g_hProgressLabel, progressText.c_str());

    ReleaseSemaphore(g_hSemaphore, 1, NULL);
}

// Функция вычисления интеграла в отдельном потоке
DWORD WINAPI CalculationThreadProc(LPVOID lpParam)
{
    // Использование мьютекса для синхронизации доступа к переменной g_bCalculationInProgress
    WaitForSingleObject(g_hMutex, INFINITE);

    g_bCalculationInProgress = true;

    ReleaseMutex(g_hMutex);

    double integral = CalculateIntegral();

    WaitForSingleObject(g_hMutex, INFINITE);

    g_bCalculationInProgress = false;

    ReleaseMutex(g_hMutex);

    MessageBox(g_hMainWindow, (L"Integral value: " + std::to_wstring(integral)).c_str(), L"Calculation Complete", MB_OK | MB_ICONINFORMATION);

    SendMessage(g_hProgressBar, PBM_SETPOS, 0, 0);
    SetWindowText(g_hProgressLabel, L"Progress: 0%");

    return 0;
}