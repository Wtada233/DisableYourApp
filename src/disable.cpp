#include <windows.h>
#include <stdio.h>

#define ID_EDIT 101
#define ID_BUTTON 102

BOOL SetImageHijack(LPCSTR lpszTargetExe, LPCSTR lpszTargetPath) {
    HKEY hKey;
    CHAR szRegPath[MAX_PATH];
    
    sprintf(szRegPath, 
        "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\%s", 
        lpszTargetExe);

    LONG lResult = RegCreateKeyExA(
        HKEY_LOCAL_MACHINE,
        szRegPath,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_WRITE,
        NULL,
        &hKey,
        NULL);

    if (lResult != ERROR_SUCCESS) {
        return FALSE;
    }

    lResult = RegSetValueExA(
        hKey,
        "Debugger",
        0,
        REG_SZ,
        (const BYTE*)lpszTargetPath,
        strlen(lpszTargetPath) + 1);

    RegCloseKey(hKey);
    return (lResult == ERROR_SUCCESS);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CREATE: {
            CreateWindowA("EDIT", "", 
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                10, 10, 200, 25, 
                hWnd, (HMENU)ID_EDIT, NULL, NULL);
            
            CreateWindowA("BUTTON", "���ó���", 
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                220, 10, 80, 25, 
                hWnd, (HMENU)ID_BUTTON, NULL, NULL);
            break;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_BUTTON) {
                CHAR szTargetExe[MAX_PATH] = {0};
                CHAR szTargetPath[MAX_PATH] = {0};
        
                GetWindowTextA(GetDlgItem(hWnd, ID_EDIT), szTargetExe, MAX_PATH);
        
                if (szTargetExe[0] == '\0') {
                    MessageBoxA(hWnd, "������Ҫ���õĳ�����", "����", MB_ICONERROR);
                    break;
                }

                DWORD dwResult = SearchPathA(
                    NULL,
                    szTargetExe,
                    ".exe",
                    MAX_PATH,
                    szTargetPath,
                    NULL
                );

                if (dwResult == 0) {
                    MessageBoxA(hWnd, "δ�ҵ�Ŀ�������������Ƿ���PATH����������", "����", MB_ICONERROR);
                    break;
                }

                // ����ӳ��ٳ�
                if(SetImageHijack(szTargetExe, szTargetPath)) {
                    MessageBoxA(hWnd, "ӳ��ٳ����óɹ���", "��ʾ", MB_OK);
                } else {
                    MessageBoxA(hWnd, "����ʧ�ܣ����ù���ԱȨ�����г���", "����", MB_ICONERROR);
                }
            }
            break;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "HijackClass";
    RegisterClassA(&wc);

    HWND hWnd = CreateWindowA("HijackClass", "������ù��� By Wtada233", 
        WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
        CW_USEDEFAULT, CW_USEDEFAULT, 320, 100, 
        NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}