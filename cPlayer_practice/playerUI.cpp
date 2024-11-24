#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "musicPlayer.h"
#include "dynamicArray.h"
#include "main.h"

enum UpdateType {
    UpdateClear,
    UpdateAll,
    UpdateTime,
    UpdateStatus,
    UpdateList,
};

COORD coord = { 0, 0 };
HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);

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

void gotoxy(int x, int y) {
    coord.X = x; // 열 위치
    coord.Y = y; // 행 위치
    SetConsoleCursorPosition(outputHandle, coord);
}

void uprintf(int line,const wchar_t* str,...) {
    wchar_t buffer[240];
    DWORD dwBytesWritten;

    va_list args;
    va_start(args, str);  

    vswprintf(buffer, 240, str, args);
    gotoxy(0, line);
    WriteConsoleOutputCharacterW(outputHandle, buffer, wcslen(buffer), coord, &dwBytesWritten);

    va_end(args);
}

void clearLine(int line) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(outputHandle, &csbi)) {
        printf("Error getting console buffer info.\n");
        return;
    }
    DWORD lineWidth = csbi.dwSize.X;
    DWORD charsWritten;
    // 공백으로 덮을 문자열 생성
    wchar_t* blankLine = (wchar_t*)malloc((lineWidth + 1) * sizeof(wchar_t));
    for (DWORD i = 0; i < lineWidth; i++) {
        blankLine[i] = L' ';
    }
    blankLine[lineWidth] = L'\0'; // 문자열 끝 지정
    gotoxy(0, line);
    WriteConsoleOutputCharacterW(outputHandle, blankLine, lineWidth, coord, &charsWritten);
    free(blankLine);
}

const wchar_t* GetMP3Name(const wchar_t* name) {
    if (name == NULL) {
        return NULL; // 유효하지 않은 입력 처리
    }

    const wchar_t* lastSlash = wcsrchr(name, L'\\');
    if (lastSlash == NULL) {
        return name; // 경로 구분자가 없으면 전체 문자열 반환
    }

    return lastSlash + 1;
}

void readInput(INPUT_RECORD* rec,DWORD* eventWritten) {
    // 콘솔 입력 모드 설정
    DWORD prev_mode;
    GetConsoleMode(inputHandle, &prev_mode);
    SetConsoleMode(inputHandle, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT);
    while (1) {
        ReadConsoleInput(inputHandle, rec, 1, eventWritten);
        if (*eventWritten > 0) break;
    }
    // 이전 콘솔 모드 복원
    SetConsoleMode(inputHandle, prev_mode);

}

void PrintList(StringArray* array) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(outputHandle, &csbi)) {
        printf("Error getting console buffer info.\n");
        return;
    }
    DWORD lineWidth = csbi.dwSize.X - 1;

    int firstLine = 8;
    int lastLine = getStringArrayLength(array)+firstLine;

    wchar_t* divideLine = (wchar_t*)malloc((lineWidth + 1) * sizeof(wchar_t));
    for (DWORD i = 1; i < lineWidth - 1; i++) divideLine[i] = L'─'; divideLine[0] = L'┌'; divideLine[lineWidth - 1] = L'┐'; divideLine[lineWidth] = L'\0';
    clearLine(7); uprintf(7, L"%ls",divideLine);
    for (int i = firstLine; i < lastLine; i++) {
        const wchar_t* musicPath = getStringArrayValue(array, i - firstLine);
        const wchar_t* musicName = GetMP3Name(musicPath);
        const wchar_t* marker = player.playIndex == i - firstLine 
            ? L">>" 
            : checkIntArrayValue(&player.playedArray,i-firstLine) != -1
             ? //&& (checkIntArrayValue(&player.playedArray, player.playIndex) != getIntArrayLength(&player.playedArray)-1)
            L"[V]" : L"  ";
        clearLine(i); uprintf(i, L"│%ls %ls", marker ,musicName);
    }
    for (DWORD i = 1; i < lineWidth - 1; i++) divideLine[i] = L'─'; divideLine[0] = L'└'; divideLine[lineWidth - 1] = L'┘'; divideLine[lineWidth] = L'\0';
    clearLine(lastLine); uprintf(lastLine, L"%ls", divideLine);
    free(divideLine);
}

void UpdateUI(UpdateType type) {

    wchar_t progressBar[32] = L"";
    const int barWidth = 30;

    int length = GetStatus(MCI_STATUS_LENGTH);
    int position = GetStatus(MCI_STATUS_POSITION);
    int pos = (((double)position / length) * barWidth);

    wchar_t* currentTime = MsToTime(position);
    wchar_t* totalTime = MsToTime(length);
    wchar_t* title = wcsrchr(getStringArrayValue(&player.pathArray,player.playIndex), L'\\') +1;

    for (int i = 0; i < barWidth; i++) {
            if (i < pos)         wcscat(progressBar, L"━");
            else if (i == pos) wcscat(progressBar, L"●");
            else                  wcscat(progressBar, L"━");
    }

    SetConsoleTitleW(title);
    
    switch (type) {
    case UpdateClear:
        for (int i = 0; i < 40; i++)
            clearLine(i);
    case UpdateAll:
        clearLine(2); uprintf(2,L"%43ls", title);
        clearLine(3); uprintf(3,L"%20ls %ls %ls", currentTime, progressBar, totalTime);
        clearLine(4); uprintf(4,L"%25ls %5ls %5ls %5ls %ls", L"⇆", L"❚◁", (player.paused ? L" ▶" : L" ||"), L"▷❚", L"↻");
        PrintList(&player.pathArray);
        break;
    case UpdateTime:
        uprintf(3,L"%20ls %ls %ls", currentTime, progressBar, totalTime);
        break;
    case UpdateStatus:
        uprintf(4,L"%25ls %5ls %5ls %5ls %ls", L"⇆", L"❚◁", (player.paused ? L" ▶" : L"||"), L"▷❚", L"↻");
        break;
    case UpdateList:
        PrintList(&player.pathArray);
        break;
    }
    
}