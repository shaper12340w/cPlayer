#define _CRT_SECURE_NO_WARNINGS // 보안 경고 비활성화
#include <stdio.h> // 표준 입출력 함수
#include <stdarg.h> // 가변 인수 함수
#include <string.h> // 문자열 처리 함수
#include "musicPlayer.h" // 음악 플레이어 관련 헤더
#include "dynamicArray.h" // 동적 배열 관련 헤더

enum UpdateType { // UI 업데이트 타입 열거형
    UpdateClear, // 화면 지우기
    UpdateAll, // 전체 업데이트
    UpdateTime, // 시간 업데이트
    UpdateStatus, // 상태 업데이트
    UpdateList, // 리스트 업데이트
};

COORD coord = { 0, 0 }; // 콘솔 커서 위치 구조체 초기화
HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE); // 출력 핸들 가져오기
HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE); // 입력 핸들 가져오기

wchar_t* MsToTime(int ms) { // 밀리초를 시간 형식으로 변환
    wchar_t* buffer = (wchar_t*)malloc(sizeof(wchar_t) * 20); // 시간 문자열 버퍼 할당
    int total_seconds = ms / 1000; // 총 초 계산
    int seconds = total_seconds % 60; // 초 계산
    int minutes = (total_seconds / 60) % 60; // 분 계산
    int hours = total_seconds / 3600; // 시간 계산

    if (hours > 0) { // 시간이 0보다 크면
        swprintf(buffer, 20, L"%02d:%02d:%02d", hours, minutes, seconds); // 시간 형식으로 문자열 생성
    }
    else { // 시간이 0이면
        swprintf(buffer, 20, L"%02d:%02d", minutes, seconds); // 분:초 형식으로 문자열 생성
    }
    return buffer; // 시간 문자열 반환
}

/*
* gotoxy 함수와 uprintf와 관련된 추가 주석
* 
* 보통 콘솔에 출력을 할 땐 printf등을 사용합니다.
* 그러나 제가 가져오고 싶은건 "음악"의 제목 출력 등등이므로
* 예기치 않게 유니코드가 포함된 문자열이 사용될 수 있습니다.
* 그러므로 wprintf함수와 wchar_t를 이용하여 유니코드 문자열을 처리할 수 있습니다만,
* 
* 윈도우에서 콘솔에 유니코드 문자열은 UTF-16까지로 몇몇 유니코드 문자열은 출력이 되지만
* UTF-32에 해당하는 유니코드 문자열 ( 아래 코드의 재생 다음곡 등등의 문자열 ) 은 기본적으로 wprintf로
* 출력이 안되는 것을 확인했습니다.
* 
* 이를 해결하기 위해 콘솔에 직접 접근하여 조종할 수 있는 COORD와 stdio의 라이브러리의 각종 함수를
* 사용했습니다.
*/

void gotoxy(int x, int y) { // 콘솔 커서 위치 설정
    coord.X = x; // 열 위치 설정
    coord.Y = y; // 행 위치 설정
    SetConsoleCursorPosition(outputHandle, coord); // 콘솔 커서 위치 이동
}

void uprintf(int line, const wchar_t* str, ...) { // 콘솔에 형식화된 문자열 출력
    wchar_t buffer[240]; // 출력 버퍼
    DWORD dwBytesWritten; // 쓰여진 바이트 수

    va_list args; // 가변 인수 리스트
    va_start(args, str); // 가변 인수 리스트 초기화

    vswprintf(buffer, 240, str, args); // 형식화된 문자열 생성
    gotoxy(0, line); // 커서 위치 설정
    WriteConsoleOutputCharacterW(outputHandle, buffer, wcslen(buffer), coord, &dwBytesWritten); // 콘솔에 문자열 출력

    va_end(args); // 가변 인수 리스트 종료
}

void clearLine(int line) { // 콘솔의 한 줄 지우기
    CONSOLE_SCREEN_BUFFER_INFO csbi; // 콘솔 화면 버퍼 정보 구조체
    if (!GetConsoleScreenBufferInfo(outputHandle, &csbi)) { // 콘솔 화면 버퍼 정보 가져오기 실패 시
        printf("Error getting console buffer info.\n"); // 에러 메시지 출력
        return; // 함수 종료
    }
    DWORD lineWidth = csbi.dwSize.X; // 콘솔 화면 너비
    DWORD charsWritten; // 쓰여진 문자 수
    wchar_t* blankLine = (wchar_t*)malloc((lineWidth + 1) * sizeof(wchar_t)); // 공백 문자열 버퍼 할당
    for (DWORD i = 0; i < lineWidth; i++) { // 화면 너비만큼 반복
        blankLine[i] = L' '; // 공백 문자로 채우기
    }
    blankLine[lineWidth] = L'\0'; // 문자열 끝 지정
    gotoxy(0, line); // 커서 위치 설정
    WriteConsoleOutputCharacterW(outputHandle, blankLine, lineWidth, coord, &charsWritten); // 콘솔에 공백 문자열 출력
    free(blankLine); // 공백 문자열 버퍼 해제
}

const wchar_t* GetMP3Name(const wchar_t* name) { // MP3 파일 이름 추출
    if (name == NULL) { // 입력이 NULL이면
        return NULL; // NULL 반환
    }

    const wchar_t* lastSlash = wcsrchr(name, L'\\'); // 마지막 경로 구분자 위치 찾기
    if (lastSlash == NULL) { // 경로 구분자가 없으면
        return name; // 전체 문자열 반환
    }

    return lastSlash + 1; // 경로 구분자 다음 문자 반환
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

/*
* main.cpp 참고..
* 코드 참고 : 
*/

void readInput(INPUT_RECORD* rec, DWORD* eventWritten) { // 콘솔 입력 읽기
    DWORD prev_mode; // 이전 콘솔 모드
    GetConsoleMode(inputHandle, &prev_mode); // 현재 콘솔 모드 가져오기
    SetConsoleMode(inputHandle, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT); // 콘솔 모드 설정
    while (1) { // 무한 루프
        ReadConsoleInput(inputHandle, rec, 1, eventWritten); // 콘솔 입력 읽기
        if (*eventWritten > 0) break; // 이벤트가 발생하면 루프 종료
    }
    SetConsoleMode(inputHandle, prev_mode); // 이전 콘솔 모드 복원
}

void PrintList(StringArray* array) { // 음악 리스트 출력
    CONSOLE_SCREEN_BUFFER_INFO csbi; // 콘솔 화면 버퍼 정보 구조체
    if (!GetConsoleScreenBufferInfo(outputHandle, &csbi)) { // 콘솔 화면 버퍼 정보 가져오기 실패 시
        printf("Error getting console buffer info.\n"); // 에러 메시지 출력
        return; // 함수 종료
    }
    DWORD lineWidth = csbi.dwSize.X - 1; // 콘솔 화면 너비

    int firstLine = 8; // 첫 번째 줄 번호
    int lastLine = getStringArrayLength(array) + firstLine; // 마지막 줄 번호

    wchar_t* divideLine = (wchar_t*)malloc((lineWidth + 1) * sizeof(wchar_t)); // 구분선 문자열 버퍼 할당
    for (DWORD i = 1; i < lineWidth - 1; i++) divideLine[i] = L'─'; // 구분선 문자로 채우기
    divideLine[0] = L'┌'; // 구분선 시작 문자
    divideLine[lineWidth - 1] = L'┐'; // 구분선 끝 문자
    divideLine[lineWidth] = L'\0'; // 문자열 끝 지정
    uprintf(7, L"%ls", divideLine); // 구분선 출력
    for (int i = firstLine; i < lastLine; i++) { // 리스트 항목 출력
        const wchar_t* musicPath = getStringArrayValue(array, i - firstLine); // 음악 경로 가져오기
        const wchar_t* musicName = GetMP3Name(musicPath); // 음악 이름 추출
        const wchar_t* marker = player.playIndex == i - firstLine // 현재 재생 중인 항목 표시
            ? L">>" 
            : checkIntArrayValue(&player.playedArray, i - firstLine) != -1 // 재생 완료 항목 표시
            ? L"[V]" : L"   ";
        uprintf(i, L"│%ls %ls", marker, musicName); // 리스트 항목 출력
    }
    for (DWORD i = 1; i < lineWidth - 1; i++) divideLine[i] = L'─'; // 구분선 문자로 채우기
    divideLine[0] = L'└'; // 구분선 시작 문자
    divideLine[lineWidth - 1] = L'┘'; // 구분선 끝 문자
    divideLine[lineWidth] = L'\0'; // 문자열 끝 지정
    uprintf(lastLine, L"%ls", divideLine); // 구분선 출력
    free(divideLine); // 구분선 문자열 버퍼 해제
}

void UpdateUI(UpdateType type) { // UI 업데이트 함수

    wchar_t progressBar[32] = L""; // 진행 바 문자열 초기화
    const int barWidth = 30; // 진행 바 너비

    int length = GetStatus(MCI_STATUS_LENGTH); // 음악 길이 가져오기
    int position = GetStatus(MCI_STATUS_POSITION); // 현재 재생 위치 가져오기
    int pos = (((double)position / length) * barWidth); // 진행 바 위치 계산

    wchar_t* currentTime = MsToTime(position); // 현재 시간 문자열 생성
    wchar_t* totalTime = MsToTime(length); // 총 시간 문자열 생성
    wchar_t* title = wcsrchr(getStringArrayValue(&player.pathArray, player.playIndex), L'\\') + 1; // 현재 재생 중인 음악 제목 추출

    for (int i = 0; i < barWidth; i++) { // 진행 바 문자열 생성
        if (i < pos) wcscat(progressBar, L"━"); // 진행된 부분
        else if (i == pos) wcscat(progressBar, L"●"); // 현재 위치
        else wcscat(progressBar, L"━"); // 진행되지 않은 부분
    }

    SetConsoleTitleW(title); // 콘솔 제목 설정
    
    switch (type) { // 업데이트 타입에 따른 처리
    case UpdateClear: // 화면 지우기
        for (int i = 0; i < 40; i++)
            clearLine(i);
    case UpdateAll: // 전체 업데이트
        clearLine(2); uprintf(2, L"%43ls", title); // 제목 출력
        clearLine(3); uprintf(3, L"%20ls %ls %ls", currentTime, progressBar, totalTime); // 진행 바 출력
        clearLine(4); uprintf(4, L"%25ls %5ls %5ls %5ls %ls", L"⇆", L"❚◁", (player.paused ? L" ▶" : L" ||"), L"▷❚", L"↻"); // 제어 버튼 출력
        PrintList(&player.pathArray); // 리스트 출력
        break;
    case UpdateTime: // 시간 업데이트
        uprintf(3, L"%20ls %ls %ls", currentTime, progressBar, totalTime); // 진행 바 출력
        break;
    case UpdateStatus: // 상태 업데이트
        uprintf(4, L"%25ls %5ls %5ls %5ls %ls", L"⇆", L"❚◁", (player.paused ? L" ▶" : L"||"), L"▷❚", L"↻"); // 제어 버튼 출력
        break;
    case UpdateList: // 리스트 업데이트
        PrintList(&player.pathArray); // 리스트 출력
        break;
    }
}