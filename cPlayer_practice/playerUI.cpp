#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "musicPlayer.h"
#include "dynamicArray.h"

enum UpdateType {
    UpdateAll,
    UpdateTime,
    UpdateStatus,
};

wchar_t* MsToTime(int ms) {
    wchar_t* buffer = (wchar_t*)malloc(sizeof(wchar_t) * 20);
    int total_seconds = ms / 1000;
    int seconds = total_seconds % 60;
    int minutes = (total_seconds / 60) % 60;
    int hours = total_seconds / 3600;

    if (hours > 0) {
        swprintf(buffer, 20, L"%02d:%02d:%02d", hours, minutes, seconds);
    }
    else {
        swprintf(buffer, 20, L"%02d:%02d", minutes, seconds);
    }
    return buffer;
}

void gotoxy(COORD* coord,HANDLE handle,int x, int y) {
    coord->X = x; // 열 위치
    coord->Y = y; // 행 위치
    SetConsoleCursorPosition(handle, *coord);
}

void printUnicode(COORD* coord, HANDLE handle, const wchar_t* str,...) {
    wchar_t buffer[240];
    DWORD dwBytesWritten;

    va_list args;
    va_start(args, str);  

    vswprintf(buffer, 240, str, args);
    WriteConsoleOutputCharacterW(handle, buffer, wcslen(buffer), *coord, &dwBytesWritten);

    va_end(args);
}

void UpdateUI(UpdateType type) {

    wchar_t progressBar[32] = L"";
    const int barWidth = 30;

    int length = GetStatus(MCI_STATUS_LENGTH);
    int position = GetStatus(MCI_STATUS_POSITION);
    int pos = (((double)position / length) * barWidth);

    wchar_t* currentTime = MsToTime(position);
    wchar_t* totalTime = MsToTime(length);
    wchar_t* title = wcsrchr(getArrayValue(&player.pathArray,0), L'\\') +1;

    for (int i = 0; i < barWidth; i++) {
            if (i < pos)         wcscat(progressBar, L"━");
            else if (i == pos) wcscat(progressBar, L"●");
            else                  wcscat(progressBar, L"━");
    }

    SetConsoleTitle(title);

    COORD coord = { 0, 0 };
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

    switch (type) {
    case UpdateAll:
        system("cls");
        gotoxy(&coord, handle, 0, 1); printUnicode(&coord,handle,L"%43ls", title);
        gotoxy(&coord, handle, 0, 2); printUnicode(&coord, handle,L"%20ls %ls %ls", currentTime, progressBar, totalTime);
        gotoxy(&coord, handle, 0, 3); printUnicode(&coord, handle,L"%25ls %5ls %5ls %5ls %ls", L"⇆", L"❚◁", (player.paused ? L"▶" : L" ||"), L"▷❚", L"↻");
        break;
    case UpdateTime:
        gotoxy(&coord,handle,0, 2); printUnicode(&coord, handle,L"%20ls %ls %ls", currentTime, progressBar, totalTime);
        break;
    case UpdateStatus:
        gotoxy(&coord, handle, 0, 3); printUnicode(&coord, handle,L"%25ls %5ls %5ls %5ls %ls", L"⇆", L"❚◁", (player.paused ? L"▶" : L" ||"), L"▷❚", L"↻");
        break;
    }
    
}