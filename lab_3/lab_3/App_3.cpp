#include <Windows.h>
#include <CommCtrl.h>
#include <thread>
#include <vector>
#include <string>

#pragma comment(lib, "Comctl32.lib")

HWND g_hTreeView;
const wchar_t* initialPath = L"D:\\\\test";

void PopulateTreeView(HWND hTreeView, const wchar_t* path, HTREEITEM hParentItem);
DWORD WINAPI WatchFileSystem(LPVOID lpParam);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{ 
    // ����������� ������ ����
    const wchar_t CLASS_NAME[] = L"MyFileExplorerClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // �������� ����
    HWND hwnd = CreateWindowEx(
        0,                              // �������������� ����� ����
        CLASS_NAME,                     // ��� ������ ����
        L"��� �������� ���������",      // ��������� ����
        WS_OVERLAPPEDWINDOW,            // ����� ����
        CW_USEDEFAULT, CW_USEDEFAULT,   // ������� ����
        500, 500,                       // ������ ����
        NULL,                           // ������������ ����
        NULL,                           // ����
        hInstance,                      // ���������� ����������
        NULL                            // �������������� ������
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // ���� ��������� ���������
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UpdateWindow(hwnd);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        {
            // �������� TreeView
            g_hTreeView = CreateWindowEx(0, WC_TREEVIEW, L"", WS_VISIBLE | WS_CHILD | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS,
                0, 0, 400, 400, hwnd, NULL, NULL, NULL);

            // ���������� TreeView �������� ��������
            PopulateTreeView(g_hTreeView, initialPath, TVI_ROOT);

            // ������ ������ ������������ ��������� � �������� �������
            DWORD threadId;
            HANDLE hThread = CreateThread(NULL, 0, WatchFileSystem, NULL, 0, &threadId);

            if (hThread != NULL)
                CloseHandle(hThread);
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

// ������� ���������� TreeView
void PopulateTreeView(HWND hTreeView, const wchar_t* path, HTREEITEM hParentItem)
{
    WIN32_FIND_DATA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    wchar_t searchPath[MAX_PATH];
    wcscpy_s(searchPath, path);
    wcscat_s(searchPath, L"\\*");

    hFind = FindFirstFile(searchPath, &findData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0)
            {
                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    wchar_t newPath[MAX_PATH];
                    wcscpy_s(newPath, path);
                    wcscat_s(newPath, L"\\");
                    wcscat_s(newPath, findData.cFileName);

                    TVINSERTSTRUCT insertStruct;
                    insertStruct.hParent = hParentItem;
                    insertStruct.hInsertAfter = TVI_LAST;
                    insertStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
                    insertStruct.item.pszText = findData.cFileName;
                    insertStruct.item.iImage = 0;
                    insertStruct.item.iSelectedImage = 0;

                    HTREEITEM hNewItem = TreeView_InsertItem(hTreeView, &insertStruct);

                    // ���������� ��������� �������������
                    PopulateTreeView(hTreeView, newPath, hNewItem);
                }
                else
                {
                    TVINSERTSTRUCT insertStruct;
                    insertStruct.hParent = hParentItem;
                    insertStruct.hInsertAfter = TVI_LAST;
                    insertStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
                    insertStruct.item.pszText = findData.cFileName;
                    insertStruct.item.iImage = 1;
                    insertStruct.item.iSelectedImage = 1;

                    TreeView_InsertItem(hTreeView, &insertStruct);
                }
            }
        } while (FindNextFile(hFind, &findData));

        FindClose(hFind);
    }
}

// ������� ������������ ��������� � �������� �������
DWORD WINAPI WatchFileSystem(LPVOID lpParam)
{
    std::wstring g_rootPath(initialPath);
    HANDLE hDirectory = CreateFile(g_rootPath.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

    if (hDirectory == INVALID_HANDLE_VALUE)
        return 1;

    DWORD bufferLength = 1024 * 1024;
    std::vector<BYTE> buffer(bufferLength);

    while (true)
    {
        DWORD bytesReturned = 0;
        BOOL result = ReadDirectoryChangesW(hDirectory, &buffer[0], bufferLength, TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME,
            &bytesReturned, NULL, NULL);

        if (!result)
            break;

        // ������� TreeView ����� ������������
        TreeView_DeleteAllItems(g_hTreeView);

        // �������������� TreeView
        PopulateTreeView(g_hTreeView, initialPath, TVI_ROOT);
    }

    CloseHandle(hDirectory);
    return 0;
}
