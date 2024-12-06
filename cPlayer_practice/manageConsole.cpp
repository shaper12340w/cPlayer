#include <tchar.h>  // 유니코드 및 멀티바이트 문자 관련 함수 및 매크로 정의
#include <windows.h>  // Windows API 함수 및 데이터 타입 정의
#include <psapi.h>  // 프로세스 상태 API 함수 정의
#include <stdio.h>  // 표준 입출력 함수 정의
#include "playerUI.h"  // 사용자 정의 헤더 파일, playerUI 관련 함수 및 데이터 타입 정의
#include "musicPlayer.h"  // 사용자 정의 헤더 파일, musicPlayer 관련 함수 및 데이터 타입 정의

int isTerminal = -1;  // 터미널 여부를 저장하는 전역 변수, 초기값은 -1

typedef struct {  // EnumData 구조체 정의
   HWND* targetHwnd;  // 찾은 윈도우 핸들을 저장할 포인터
   const TCHAR* windowName;  // 찾고자 하는 윈도우 이름
} EnumData;


/*
* 콘솔 창의 크기를 조정하기 위해, 특히 윈도우의 "터미널" 앱의 창의 크기를 조절하기 위해 창의 핸들(정보)를
* 가져오는 콜백(정보를 가져왔을때 리턴) 하는 함수입니다. (아닐 경우 TRUE 반환하여 계속 탐색)
* 
* 윈도우의 제목을 가져오는 이유 : 음악을 재생할 때 창의 이름을 음악의 재목과 일치하도록 설정하여
* 다른 터미널 창과 겹치지 않도록 하기 위함입니다.
* 
* GetWindowThreadProcessId : 윈도우 프로세스의 아이디를 가져와서
* OpenProcess : 정보와 메모리에 접근 권한 요청(PROCESS_VM_READ)
* 
* 프로세스 이름을 가져와서 터미널일 경우에 hwnd 인수에 값을 넘겨주고 함수를 종료합니다
*/

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {  // EnumWindows 함수에 전달될 콜백 함수
   EnumData* data = (EnumData*)lParam;  // lParam을 EnumData 구조체로 캐스팅
   DWORD processId;  // 프로세스 ID를 저장할 변수
   TCHAR windowTitle[MAX_PATH] = { 0 };  // 윈도우 제목을 저장할 배열

   GetWindowText(hwnd, windowTitle, MAX_PATH);  // hwnd에 해당하는 윈도우의 제목을 가져옴

   if (data->windowName != NULL && _tcsstr(windowTitle, data->windowName) == NULL) {  // 윈도우 이름 비교
       return TRUE;  // 윈도우 이름이 다르면 계속 탐색
   }

   GetWindowThreadProcessId(hwnd, &processId);  // 윈도우의 프로세스 ID를 가져옴
   HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);  // 프로세스 핸들을 엶

   if (processHandle) {  // 프로세스 핸들이 유효한 경우
       TCHAR processName[MAX_PATH] = { 0 };  // 프로세스 이름을 저장할 배열
       if (GetModuleBaseName(processHandle, NULL, processName, MAX_PATH)) {  // 프로세스 이름을 가져옴
           if (_tcsicmp(processName, _T("WindowsTerminal.exe")) == 0) {  // 프로세스 이름이 "WindowsTerminal.exe"인지 비교
               *(data->targetHwnd) = hwnd;  // 발견한 핸들을 저장
               CloseHandle(processHandle);  // 프로세스 핸들을 닫음
               return FALSE;  // 탐색 종료
           }
       }
       CloseHandle(processHandle);  // 프로세스 핸들을 닫음
   }
   return TRUE;  // 계속 탐색
}

HWND FindTerminalWindow(const TCHAR* windowName) {  // 터미널 윈도우를 찾는 함수
   HWND hwnd = NULL;  // 윈도우 핸들을 저장할 변수 초기화
   EnumData data = { &hwnd, windowName };  // EnumData 구조체 초기화
   EnumWindows(EnumWindowsProc, (LPARAM)&data);  // 모든 윈도우를 열거하여 콜백 함수 호출
   return hwnd;  // 찾은 윈도우 핸들 반환
}

/*
* Windows, 특히 윈도우 11에는 콘솔이 두가지가 있습니다
* 1. 기본 콘솔 앱 호스트 (conhost.exe or cmd.exe)
* 2. 터미널 (WindowsTerminal.exe)
* 
* 터미널은 기존 콘솔의 상위호환 개념으로 유저들이 사용하기 편하도록 여러 부가 기능을 넣은 콘솔입니다만
* 아쉽게도 system함수를 이용해서 mode 콘솔 명령어로 창 크기 변경이 불가하더군요
* 
* 그러므로 강제로 창의 핸들을 가져와서 창 크기 변경을 시키는 방식을 사용했습니다
* 그러나 기본 콘솔이 어떤 앱인지 저장되어있는 곳이 레지스트리(윈도우의 설정값) 에 저장이 되어있는데
* 이를 읽어서 콘솔 앱 호스트가 기본으로 열리는지 아님 터미널이 열리는지 감지해서 알맞게 창 크기를
* 조정하도록 하려고 값을 가져오는 함수입니다
* 
* 만약 테스트 하신다면 저는 기본 콘솔 앱을 추천합니다. 업데이트도 빠르고 기존에 제가 원하는 방식에
* 가장 부합하도록 작동되기 때문입니다.
*/

int CheckConsoleRegistry() {  // 콘솔 레지스트리를 확인하는 함수
   HKEY hkey;  // 레지스트리 키 핸들
   DWORD dwSize;  // 데이터 크기
   wchar_t value[256];  // 레지스트리 값을 저장할 배열

   if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("CONSOLE\\%%Startup"), 0, KEY_READ, &hkey) != ERROR_SUCCESS) {  // 레지스트리 키 열기
       clearLine(6); uprintf(6, L"레지스트리를 여는 과정에서 오류가 발생했습니다");  // 오류 메시지 출력
       return -1;  // 오류 반환
   }

   dwSize = sizeof(value);  // 데이터 크기 설정

   if (RegQueryValueExW(hkey, TEXT("DelegationConsole"), NULL, NULL, (LPBYTE)value, &dwSize) == ERROR_SUCCESS) {  // 레지스트리 값 조회
       if (wcscmp(value, L"{2EACA947-7F5F-4CFA-BA87-8F7FBEEFBE69}") == 0)  // 값 비교
           return 0; // yes
       else
           return 1; // no
   }
   else
       return -1;  // 오류 반환
}

void resizeWindow(int width, int height) {  // 윈도우 크기를 조정하는 함수
   
   if (isTerminal == -1) isTerminal = CheckConsoleRegistry();  // 터미널 여부 확인
   HWND targetHwnd = FindTerminalWindow(player.pathArray.array != NULL ? wcsrchr(getStringArrayValue(&player.pathArray, player.playIndex), L'\\') + 1 : NULL);  // 터미널 윈도우 찾기
   switch (isTerminal) {  // 터미널 여부에 따른 분기
   case 0:
       
       if (targetHwnd == NULL) break;  // 윈도우 핸들이 NULL인 경우 종료
       RECT rect;  // 윈도우 위치와 크기를 저장할 구조체
       GetWindowRect(targetHwnd, &rect);  // 윈도우 위치와 크기 가져오기
       MoveWindow(targetHwnd, rect.left, rect.top, width*10, height*23, TRUE);  // 윈도우 크기 조정
       break;

   case 1:

       char buffer[256];  // 명령어를 저장할 배열
       sprintf_s(buffer,"mode con cols=%d lines=%d",width,height);  // 명령어 생성
       system(buffer);  // 명령어 실행
       break;

   }
}
