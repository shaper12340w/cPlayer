#define _CRT_SECURE_NO_WARNINGS // 보안 경고를 무시하기 위한 매크로 정의
#include <conio.h> // 콘솔 입출력 함수 사용을 위한 헤더 파일
#include <stdio.h> // 표준 입출력 함수 사용을 위한 헤더 파일
#include <threads.h> // 스레드 함수 사용을 위한 헤더 파일
#include <locale.h> // 로케일 설정을 위한 헤더 파일
#include "musicPlayer.h" // 음악 플레이어 관련 함수 사용을 위한 헤더 파일
#include "fileMannger.h" // 파일 관리 관련 함수 사용을 위한 헤더 파일
#include "playerUI.h" // 플레이어 UI 관련 함수 사용을 위한 헤더 파일
#include "manageConsole.h" // 콘솔 관리 관련 함수 사용을 위한 헤더 파일
#include "dynamicArray.h" // 동적 배열 관련 함수 사용을 위한 헤더 파일

INPUT_RECORD input; // 입력 기록을 저장하기 위한 구조체 변수
CRITICAL_SECTION g_cs; // 임계 구역을 위한 변수

// 파일 열기 함수
void OpenFile() {
    wchar_t file[MAX_PATH] = L""; // 파일 경로를 저장할 변수 초기화
    clearLine(6); // 6번째 줄을 지움
    uprintf(6, L"Open File"); // 6번째 줄에 "Open File" 출력
    OpenFileDialog(file); // 파일 열기 대화상자 호출
    if (file[0] != L'\0') { // 파일 경로가 비어있지 않으면
        Add(file); // 파일 추가
    }
}

// 폴더 열기 함수
void OpenFolder() {
    wchar_t folder[MAX_PATH] = L""; // 폴더 경로를 저장할 변수 초기화
    StringArray fileArray; // 파일 배열 구조체 변수
    initStringArray(&fileArray, 10, MAX_PATH); // 파일 배열 초기화
    OpenFolderDialog(folder); // 폴더 열기 대화상자 호출
    if (folder[0] != L'\0') { // 폴더 경로가 비어있지 않으면
        ListMP3FilesInFolder(folder, &fileArray); // 폴더 내 MP3 파일 목록 생성
        for (int i = 0; i < getStringArrayLength(&fileArray); i++) // 파일 배열 길이만큼 반복
            Add(getStringArrayValue(&fileArray, i)); // 파일 추가
        removeStringArray(&fileArray); // 파일 배열 제거
    }
}

// 입력 관리 함수
void ManageInput() {
    EnterCriticalSection(&g_cs); // 임계 구역 진입

    // 입력 이벤트가 있을 때
    if (input.EventType == MOUSE_EVENT) { // 마우스 이벤트일 경우
        if (input.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) { // 왼쪽 버튼이 눌렸을 경우
            int mouse_x = input.Event.MouseEvent.dwMousePosition.X; // 마우스 X 좌표
            int mouse_y = input.Event.MouseEvent.dwMousePosition.Y; // 마우스 Y 좌표
            /*
            y 3 | x 21~52 라인
            y 4 | x 24 shuffle x 29~31 이전 x 35~38 재생 x 42~43 다음 x 46~47 반복
            */
            if (mouse_y == 3) { // Y 좌표가 3일 경우
                if (mouse_x > 20 && mouse_x < 52) { // X 좌표가 21~52 사이일 경우
                    int mousePosition = mouse_x - 21; // 마우스 위치 계산
                    int barWidth = 31; // 전체 바 길이
                    int length = GetStatus(MCI_STATUS_LENGTH); // 음악 길이
                    int currentPosition = GetStatus(MCI_STATUS_POSITION); // 현재 위치

                    // 클릭한 위치를 음악 시간으로 변환
                    int newPosition = (mousePosition * length) / barWidth;

                    MoveTo(newPosition - currentPosition); // 새로운 위치로 이동
                }
            }
            if (mouse_y == 4) { // Y 좌표가 4일 경우
                if (mouse_x == 24) Shuffle(); // X 좌표가 24일 경우 셔플
                else if (mouse_x > 28 && mouse_x < 32) Prev(); // X 좌표가 29~31일 경우 이전
                else if (mouse_x > 34 && mouse_x < 39) Pause(); // X 좌표가 35~38일 경우 일시정지
                else if (mouse_x > 41 && mouse_x < 44) Next(); // X 좌표가 42~43일 경우 다음
                else if (mouse_x > 43 && mouse_x < 48) Repeat(); // X 좌표가 44~47일 경우 반복
            }
        }
    }
    else if (input.EventType == KEY_EVENT && input.Event.KeyEvent.bKeyDown) { // 키 이벤트일 경우
        switch (input.Event.KeyEvent.uChar.AsciiChar) { // 아스키 문자에 따라
        case ' ': Pause(); break; // 스페이스바일 경우 일시정지
        case 'o': OpenFile(); break; // 'o'일 경우 파일 열기
        case 'p': OpenFolder(); break; // 'p'일 경우 폴더 열기
        case '.': Next(); break; // '.'일 경우 다음
        case ',': Prev(); break; // ','일 경우 이전
        case 's': Shuffle(); break; // 's'일 경우 셔플
        case 'r': Repeat(); break; // 'r'일 경우 반복
        }
        switch (input.Event.KeyEvent.wVirtualKeyCode) { // 가상 키 코드에 따라
        case VK_LEFT: MoveTo(-5000); break; // 왼쪽 화살표일 경우 5초 뒤로
        case VK_RIGHT: MoveTo(5000); break; // 오른쪽 화살표일 경우 5초 앞으로
        }
    }
    UpdateUI(UpdateAll); // UI 업데이트
    LeaveCriticalSection(&g_cs); // 임계 구역 종료
}

// 업데이트 관리 함수
void ManageUpadate(wchar_t* currentTime, int* count) {
    if (wcscmp(currentTime, MsToTime(GetStatus(MCI_STATUS_POSITION))) != 0) { // 현재 시간과 음악 위치가 다를 경우
        wcscpy(currentTime, MsToTime(GetStatus(MCI_STATUS_POSITION))); // 현재 시간 업데이트
        UpdateUI(UpdateAll); // UI 업데이트
        resizeWindow(75, getStringArrayLength(&player.pathArray) + 9); // 창 크기 조정

        DWORD CIN;
        DWORD mode;
        CIN = (DWORD)GetStdHandle(STD_INPUT_HANDLE); // 표준 입력 핸들 가져오기
        GetConsoleMode((HANDLE)CIN, &mode); // 콘솔 모드 가져오기
        SetConsoleMode((HANDLE)CIN, mode | ENABLE_MOUSE_INPUT); // 마우스 입력 활성화

        if (*count >= 3) { // 카운트가 3 이상일 경우
            *count = 0; // 카운트 초기화
            UpdateUI(UpdateClear); // UI 초기화
        }
        (*count)++; // 카운트 증가
    }
    else
        Sleep(100); // 100ms 대기
}

// 입력 업데이트 스레드 함수
DWORD WINAPI UpdateInput(void* arg) {
    INPUT_RECORD recData; // 입력 기록 데이터
    DWORD written; // 쓰기 결과

    while (1) {
        readInput(&recData, &written); // 입력 읽기
        if (written > 0) { // 읽기 성공 시
            EnterCriticalSection(&g_cs); // 임계 구역 진입
            memcpy(&input, &recData, sizeof(INPUT_RECORD)); // 입력 데이터 복사
            LeaveCriticalSection(&g_cs); // 임계 구역 종료
        }
    }
}

// 업데이트 함수
void Update() {
    int count = 0; // 카운트 초기화
    wchar_t currentTime[MAX_PATH]; // 현재 시간 변수
    UpdateUI(UpdateClear); // UI 초기화
    UpdateUI(UpdateAll); // UI 업데이트
    while (1) {
        if (!CheckPlayer()) break; // 플레이어 체크 실패 시 종료
        ManageUpadate(currentTime, &count); // 업데이트 관리
        ManageInput(); // 입력 관리
    }
}

// 종료 함수
void DeInit() {
    DeleteTempFile(); // 임시 파일 삭제
    removeStringArray(&player.pathArray); // 파일 배열 제거
}

// 초기화 함수
void Init() {
    InitializeCriticalSection(&g_cs); // 임계 구역 초기화
    _wsetlocale(LC_ALL, L"Korean"); // 로케일 설정
    resizeWindow(75, 40); // 창 크기 조정

    InitPlayer(); // 플레이어 초기화
    OpenFolder(); // 폴더 열기
    Ready(); Play(); // 준비 및 재생

    atexit(DeInit); // 종료 시 DeInit 호출
}

// 메인 함수
int main() {
    Init(); // 초기화
    HANDLE InputThread = CreateThread(NULL, 0, UpdateInput, NULL, 0, NULL); // 입력 업데이트 스레드 생성
    if (InputThread == NULL) return 1; // 스레드 생성 실패 시 종료
    Update(); // 업데이트
    WaitForSingleObject(InputThread, INFINITE); // 스레드 종료 대기
    CloseHandle(InputThread); // 스레드 핸들 닫기
    return 0; // 프로그램 종료
}