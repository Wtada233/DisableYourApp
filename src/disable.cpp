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
            
            CreateWindowA("BUTTON", "禁用程序", 
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
                    MessageBoxA(hWnd, "请输入要禁用的程序名", "错误", MB_ICONERROR);
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
                    MessageBoxA(hWnd, "未找到目标程序，请检查程序是否在PATH环境变量中", "错误", MB_ICONERROR);
                    break;
                }

                // 设置映像劫持
                if(SetImageHijack(szTargetExe, szTargetPath)) {
                    MessageBoxA(hWnd, "映像劫持设置成功！", "提示", MB_OK);
                } else {
                    MessageBoxA(hWnd, "设置失败，请用管理员权限运行程序", "错误", MB_ICONERROR);
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

    HWND hWnd = CreateWindowA("HijackClass", "程序禁用工具 By Wtada233", 
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