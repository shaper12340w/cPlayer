#include <tchar.h>
#include <windows.h>
#include <psapi.h>
#include <stdio.h>
#include "playerUI.h"
#include "musicPlayer.h"

int isTerminal = -1;

typedef struct {
    HWND* targetHwnd;
    const TCHAR* windowName;  // 찾고자 하는 윈도우 이름
} EnumData;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    EnumData* data = (EnumData*)lParam;
    DWORD processId;
    TCHAR windowTitle[MAX_PATH] = { 0 };

    // 윈도우 이름 가져오기
    GetWindowText(hwnd, windowTitle, MAX_PATH);

    // 윈도우 이름 비교
    if (data->windowName != NULL && _tcsstr(windowTitle, data->windowName) == NULL) {
        return TRUE;  // 윈도우 이름이 다르면 계속 탐색
    }

    GetWindowThreadProcessId(hwnd, &processId);
    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);

    if (processHandle) {
        TCHAR processName[MAX_PATH] = { 0 };
        if (GetModuleBaseName(processHandle, NULL, processName, MAX_PATH)) {
            if (_tcsicmp(processName, _T("WindowsTerminal.exe")) == 0) {
                *(data->targetHwnd) = hwnd;  // 발견한 핸들을 저장
                CloseHandle(processHandle);
                return FALSE;  // 탐색 종료
            }
        }
        CloseHandle(processHandle);
    }
    return TRUE;  // 계속 탐색
}

HWND FindTerminalWindow(const TCHAR* windowName) {
    HWND hwnd = NULL;
    EnumData data = { &hwnd, windowName };
    EnumWindows(EnumWindowsProc, (LPARAM)&data);
    return hwnd;
}

int CheckConsoleRegistry() {
    HKEY hkey;
    DWORD dwSize;
    wchar_t value[256];

    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("CONSOLE\\%%Startup"), 0, KEY_READ, &hkey) != ERROR_SUCCESS) {
        clearLine(6); uprintf(6, L"레지스트리를 여는 과정에서 오류가 발생했습니다");
        return -1;
    }

    dwSize = sizeof(value);

    if (RegQueryValueExW(hkey, TEXT("DelegationConsole"), NULL, NULL, (LPBYTE)value, &dwSize) == ERROR_SUCCESS) {
        if (wcscmp(value, L"{2EACA947-7F5F-4CFA-BA87-8F7FBEEFBE69}") == 0) 
            return 0; //yes
        else
            return 1; //no
    }
    else
        return -1;
}

void resizeWindow(int width, int height) {
    
    if (isTerminal == -1) isTerminal = CheckConsoleRegistry();
    HWND targetHwnd = FindTerminalWindow(player.pathArray.array != NULL ? wcsrchr(getStringArrayValue(&player.pathArray, player.playIndex), L'\\') + 1 : NULL);
    switch (isTerminal) {
    case 0:
        
        if (targetHwnd == NULL) break;
        RECT rect;
        GetWindowRect(targetHwnd, &rect);
        MoveWindow(targetHwnd, rect.left, rect.top, width*10, height*23, TRUE);
        break;
    case 1:
        char buffer[256];
        sprintf_s(buffer,"mode con cols=%d lines=%d",width,height);
        system(buffer);
        break;

    }
}

