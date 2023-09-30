#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <gdiplus.h> 
#pragma comment(lib,"gdiplus.lib")
#pragma comment(lib, "comctl32.lib")

using namespace Gdiplus;  

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500
#define X_PADDING 140                       // отступ слева
#define Y_PADDING 70                        // отступ сверху
#define FIELD_WIDTH 20
#define FIELD_HEIGHT 20
#define FIELD_ROWS 10
#define FIELD_COLUMNS 10
#define MINES 10                            // кол-во мин
#define ID_BUTTON_START 1000
#define ID_BUTTON_EXIT 993
#define TEXT_MENU1 1111                     // меню стилей текста
#define TEXT_MENU2 1112  
#define TEXT_MENU3 1113  
#define BG_MENU 2222                        // цвет заднего фона 

typedef struct {                            // клетка поля 
    bool isMine;
    bool isFlag;
    bool isOpen;
    int minesAround;
} FCell;

FCell field[FIELD_ROWS][FIELD_COLUMNS];         // поле
int closedCells;                                // закрытые ячейки

RECT winRectangle;
HBRUSH brushRect = CreateSolidBrush(RGB(255, 0, 255));
HFONT fontText;
COLORREF fontColor;

GdiplusStartupInput gdiplusstartupInput;

void new_game(HWND hwnd);
bool is_cell_in_field(int x, int y);
void open_fields(int x, int y, HWND hwnd);
//void OnPaint(HDC hdc);
void MainWndAddMenus(HWND hWnd);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SubclassWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) // точка входа приложения
{
    fontText = CreateFontA(50, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Times New Roman");

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc); 

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.      0 - поведение по умолчанию
        CLASS_NAME,                     // Window class
        L"My Minesweeper",              // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    HWND button;

    ULONG_PTR gdiplustokend;
    GdiplusStartup(&gdiplustokend, &gdiplusstartupInput, NULL);

    button = CreateWindow(L"BUTTON", L"Start", WS_VISIBLE | WS_CHILD, 205, 300, 75, 25, hwnd, (HMENU)ID_BUTTON_START, hInstance, NULL);
    button = CreateWindow(L"BUTTON", L"Exit", WS_VISIBLE | WS_CHILD, 205, 330, 75, 25, hwnd, (HMENU)ID_BUTTON_EXIT, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UINT message = uMsg;

    switch (uMsg)
    {
    case WM_CREATE:
        MainWndAddMenus(hwnd);
        winRectangle = { 500, 0, 0, 500 };
        //textWnd = CreateWindowEx(0, L"STATIC", L"Hello", WS_VISIBLE | WS_CHILD | ES_CENTER, 50, 50, 200, 200, hwnd, NULL, NULL, NULL);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        Graphics graphics(hdc);

        FillRect(ps.hdc, &winRectangle, brushRect);
        SetBkMode(ps.hdc, TRANSPARENT);
        SetTextColor(ps.hdc, fontColor);
        SelectObject(ps.hdc, fontText);
        DrawTextA(ps.hdc, "My Minesweeper", -1, &winRectangle, DT_SINGLELINE | DT_CENTER | DT_NOCLIP);

        /*FontFamily fontfamily(TEXT("Times New Roman"));
        Font font(&fontfamily, 10, FontStyleRegular, UnitPixel);
        PointF pointf(50.0f, 5.f);
        SolidBrush brush(Color(255, 255, 0, 0));
        graphics.DrawString(TEXT(" My\nMinesweeper"), -1, &font, pointf, &brush);*/

        EndPaint(hwnd, &ps);
    }
    return 0;

    case WM_COMMAND: 
    {
        if (wParam == TEXT_MENU1) {
            fontText = CreateFontA(50, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Times New Roman");

            RedrawWindow(hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);

            return 0;
        }

        else if (wParam == TEXT_MENU2) {
            fontText = CreateFontA(50, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");

            RedrawWindow(hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);

            return 0;
        }

        else if (wParam == TEXT_MENU3) {
            fontText = CreateFontA(50, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Calibri");

            RedrawWindow(hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);

            return 0;
        }

        else if (wParam == BG_MENU) {
            srand(time(NULL));
            int colorR = rand() % 255;
            int colorG = rand() % 255;
            int colorB = rand() % 255;
            brushRect = CreateSolidBrush(
                RGB(colorR,
                    colorG,
                    colorB
                ));

            fontColor = RGB(255 - colorR, 255 - colorG, 255 - colorB);

            RedrawWindow(hwnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);

            return 0;
        }

        else if (wParam == ID_BUTTON_START) {
            new_game(hwnd);
            return 0;
        }
        else if (wParam == ID_BUTTON_EXIT) {
            if (MessageBox(hwnd, L"Really quit?", L"My Minesweeper", MB_OKCANCEL) == IDOK)
            {
                DestroyWindow(hwnd);
            }
            // Else: User canceled. Do nothing.
            return 0;
        }
        else {
            HWND button = GetDlgItem(hwnd, LOWORD(wParam)); 
            int buttonId = GetDlgCtrlID(button);

            int cellX = (buttonId - 1) % 10;
            int cellY = (buttonId - 1) / 10;

            if (field[cellX][cellY].isMine) {
                SendMessage(button, WM_SETTEXT, 0, (LPARAM)(L"#"));
                if (MessageBox(hwnd, L"You lost!", L"My Minesweeper", MB_OK) == IDOK)
                {
                    new_game(hwnd);
                }
            }
            else if (!field[cellX][cellY].isFlag) {
                open_fields(cellX, cellY, hwnd);

            }
            if (closedCells == 0) {
                if (MessageBox(hwnd, L"You won!", L"My Minesweeper", MB_OK) == IDOK)
                {
                    new_game(hwnd);
                }
            }  
        }

        break;
    }
        return 0;

    case WM_CLOSE:
        if (MessageBox(hwnd, L"Really quit?", L"My Minesweeper", MB_OKCANCEL) == IDOK)
        {
            DestroyWindow(hwnd);
        }
        // Else: User canceled. Do nothing.
        return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK SubclassWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,    // обработка правого клика мышки
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {

    HWND button = GetDlgItem(hWnd, LOWORD(wParam));

    switch (uMsg) {
    case WM_NCDESTROY:
        RemoveWindowSubclass(hWnd, SubclassWindowProc, uIdSubclass);
        break;

    case WM_RBUTTONDOWN: {
        int buttonId = GetDlgCtrlID(hWnd);
        int cellX = (buttonId - 1) % 10;
        int cellY = (buttonId - 1) / 10;
        if (!field[cellX][cellY].isOpen) {
            if (!field[cellX][cellY].isFlag) {
                SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)(L"F"));
                field[cellX][cellY].isFlag = true;
            }
            else {
                SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)(L""));
                field[cellX][cellY].isFlag = false;
            }
        }
    }
        return 0;

    default:
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

void new_game(HWND hwnd) {                          // прорисовка поля игры
    srand(time(NULL));
    memset(field, 0, sizeof(field));

    closedCells = FIELD_ROWS * FIELD_COLUMNS - MINES;        

    for (int i = 0; i < MINES; i++) {                // расстановка мин
        int row = rand() % FIELD_ROWS;
        int col = rand() % FIELD_COLUMNS;
        if (field[row][col].isMine)
            i--;
        else {
            field[row][col].isMine = true;

            for (int dx = -1; dx < 2; dx++) {
                for (int dy = -1; dy < 2; dy++) {
                    if (is_cell_in_field(row + dx, col + dy))
                        field[row + dx][col + dy].minesAround++;
                }
            }
        }
    }

    for (int x = 0; x < FIELD_ROWS; x++) {
        for (int y = 0; y < FIELD_COLUMNS; y++) {
            HWND fieldButton = CreateWindowEx(0, L"BUTTON", L"",
                WS_CHILD | WS_VISIBLE,
                X_PADDING + x * FIELD_WIDTH, Y_PADDING + y * FIELD_HEIGHT, FIELD_WIDTH, FIELD_HEIGHT,
                hwnd, (HMENU)(x + y * FIELD_COLUMNS + 1), NULL, 0);

            SetWindowSubclass(fieldButton, SubclassWindowProc, 0, 0);
        }
    }
}

bool is_cell_in_field(int x, int y) {
    return (x >= 0) && (y >= 0) && (x < FIELD_ROWS) && (y < FIELD_COLUMNS);
}

void open_fields(int x, int y, HWND hwnd) {                            // открытие полей
    int buttonId = x + 1 + y * 10;
    HWND button = GetDlgItem(hwnd, buttonId);

    field[x][y].isOpen = true;
    closedCells--;

    if (field[x][y].minesAround) {
        SetWindowTextW(button, std::to_wstring(field[x][y].minesAround).c_str());
    }
    else if (field[x][y].minesAround == 0){
        SetWindowTextW(button, L".");
        for (int dx = -1; dx < 2; dx++) {
            for (int dy = -1; dy < 2; dy++) {
                if (is_cell_in_field(x + dx, y + dy) && !field[x + dx][y + dy].isOpen && !field[x + dx][y + dy].isFlag)
                    open_fields(x + dx, y + dy, hwnd);
            }
        }
    }
}

void MainWndAddMenus(HWND hWnd) {
    HMENU RootMenu = CreateMenu();
    HMENU SubMenu = CreateMenu();
    HMENU SubFontMenu = CreateMenu();

    AppendMenu(SubFontMenu, MF_STRING, TEXT_MENU1, L"Times New Roman");
    AppendMenu(SubFontMenu, MF_STRING, TEXT_MENU2, L"Arial");
    AppendMenu(SubFontMenu, MF_STRING, TEXT_MENU3, L"Calibri");

    AppendMenu(SubMenu, MF_POPUP, (UINT_PTR)SubFontMenu, L"Text");
    AppendMenu(SubMenu, MF_STRING, BG_MENU, L"Background");

    AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)SubMenu, L"Styles");

    SetMenu(hWnd, RootMenu);
}
