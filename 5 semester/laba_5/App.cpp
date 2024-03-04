#include <Windows.h>
#include <iostream>

enum {
    EDIT_SUBKEY = 1001,
    EDIT_VALUENAME,
    EDIT_VALUEDATA,
    BUTTON_CREATE,
    BUTTON_READ,
    BUTTON_DELETE
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND CreateAppWindow(HINSTANCE hInstance);
void CreateRegistryKey(HWND hwnd);
void ReadRegistryKey(HWND hwnd);
void DeleteRegistryKey(HWND hwnd);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HWND hwnd = CreateAppWindow(hInstance);
    if (hwnd == NULL)
        return -1;

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case BUTTON_CREATE:
            CreateRegistryKey(hwnd);
            break;

        case BUTTON_READ:
            ReadRegistryKey(hwnd);
            break;

        case BUTTON_DELETE:
            DeleteRegistryKey(hwnd);
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

HWND CreateAppWindow(HINSTANCE hInstance)
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"RegistryAppClass";

    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, L"Ошибка при регистрации класса окна.", L"Ошибка", MB_OK | MB_ICONERROR);
        return NULL;
    }

    HWND hwnd = CreateWindowEx(
        0,
        L"RegistryAppClass",
        L"Редактор реестра",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBox(NULL, L"Ошибка при создании окна.", L"Ошибка", MB_OK | MB_ICONERROR);
        return NULL;
    }

    CreateWindow(
        L"STATIC", L"Субключ:",
        WS_VISIBLE | WS_CHILD,
        20, 20, 100, 20,
        hwnd, NULL, hInstance, NULL);

    CreateWindow(
        L"EDIT", L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        130, 20, 300, 20,
        hwnd, reinterpret_cast<HMENU>(EDIT_SUBKEY), hInstance, NULL);

    CreateWindow(
        L"STATIC", L"Имя значения:",
        WS_VISIBLE | WS_CHILD,
        20, 50, 100, 20,
        hwnd, NULL, hInstance, NULL);

    CreateWindow(
        L"EDIT", L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        130, 50, 300, 20,
        hwnd, reinterpret_cast<HMENU>(EDIT_VALUENAME), hInstance, NULL);

    CreateWindow(
        L"STATIC", L"Данные значения:",
        WS_VISIBLE | WS_CHILD,
        20, 80, 100, 20,
        hwnd, NULL, hInstance, NULL);

    CreateWindow(
        L"EDIT", L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        130, 80, 300, 20,
        hwnd, reinterpret_cast<HMENU>(EDIT_VALUEDATA), hInstance, NULL);

    CreateWindow(
        L"BUTTON", L"Создать",
        WS_VISIBLE | WS_CHILD,
        20, 120, 80, 30,
        hwnd, reinterpret_cast<HMENU>(BUTTON_CREATE), hInstance, NULL);

    CreateWindow(
        L"BUTTON", L"Прочитать",
        WS_VISIBLE | WS_CHILD,
        120, 120, 80, 30,
        hwnd, reinterpret_cast<HMENU>(BUTTON_READ), hInstance, NULL);

    CreateWindow(
        L"BUTTON", L"Удалить",
        WS_VISIBLE | WS_CHILD,
        220, 120, 80, 30,
        hwnd, reinterpret_cast<HMENU>(BUTTON_DELETE), hInstance, NULL);

    return hwnd;
}

void CreateRegistryKey(HWND hwnd)
{
    TCHAR subKey[256];
    WCHAR valueName[256];
    WCHAR valueData[256];

    GetDlgItemText(hwnd, EDIT_SUBKEY, subKey, 256);
    GetDlgItemText(hwnd, EDIT_VALUENAME, valueName, 256);
    GetDlgItemText(hwnd, EDIT_VALUEDATA, valueData, 256);

    std::wstring fullKey = L"App_lab_5\\";
    fullKey += subKey;

    HKEY hKey = HKEY_CURRENT_USER;
    HKEY hRegistryKey;
    LONG result = RegCreateKeyEx(hKey, fullKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hRegistryKey, NULL);
    if (result == ERROR_SUCCESS)
    {
        result = RegSetValueEx(hRegistryKey, valueName, 0, REG_SZ, reinterpret_cast<const BYTE*>(valueData), (wcslen(valueData) + 1) * sizeof(wchar_t));
        if (result == ERROR_SUCCESS)
            MessageBox(hwnd, L"Реестровая запись успешно создана.", L"Успех", MB_OK | MB_ICONINFORMATION);
        else
            MessageBox(hwnd, L"Ошибка при установке значения реестра.", L"Ошибка", MB_OK | MB_ICONERROR);

        RegCloseKey(hRegistryKey);
    }
    else
    {
        MessageBox(hwnd, L"Ошибка при создании реестровой записи.", L"Ошибка", MB_OK | MB_ICONERROR);
    }
}

void ReadRegistryKey(HWND hwnd)
{
    WCHAR subKey[256];
    WCHAR valueName[256];

    GetDlgItemText(hwnd, EDIT_SUBKEY, subKey, 256);
    GetDlgItemText(hwnd, EDIT_VALUENAME, valueName, 256);

    HKEY hKey = HKEY_CURRENT_USER;
    HKEY hRegistryKey;

    std::wstring fullKey = L"App_lab_5\\";
    fullKey += subKey;

    LONG result = RegOpenKeyEx(hKey, fullKey.c_str(), 0, KEY_READ, &hRegistryKey);
    if (result == ERROR_SUCCESS)
    {
        WCHAR valueData[256];
        DWORD bufferSize = sizeof(valueData);

        result = RegQueryValueEx(hRegistryKey, valueName, NULL, NULL, reinterpret_cast<LPBYTE>(valueData), &bufferSize);
        if (result == ERROR_SUCCESS)
        {
            MessageBox(hwnd, valueData, L"Значение реестра", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            MessageBox(hwnd, L"Ошибка при чтении значения реестра.", L"Ошибка", MB_OK | MB_ICONERROR);
        }

        RegCloseKey(hRegistryKey);
    }
    else
    {
        MessageBox(hwnd, L"Ошибка при открытии реестровой записи.", L"Ошибка", MB_OK | MB_ICONERROR);
    }
}

void DeleteRegistryKey(HWND hwnd)
{
    WCHAR subKey[256];

    GetDlgItemText(hwnd, EDIT_SUBKEY, subKey, 256);

    HKEY hKey = HKEY_CURRENT_USER;

    std::wstring fullKey = L"App_lab_5\\";
    fullKey += subKey;

    LONG result = RegDeleteKey(hKey, fullKey.c_str());
    if (result == ERROR_SUCCESS)
    {
        MessageBox(hwnd, L"Реестровая запись успешно удалена.", L"Успех", MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        MessageBox(hwnd, L"Ошибка при удалении реестровой записи.", L"Ошибка", MB_OK | MB_ICONERROR);
    }
}
