#define _CRT_SECURE_NO_WARNINGS
#include <conio.h>
#include <stdio.h>
#include <locale.h>
#include "musicPlayer.h"
#include "fileMannger.h"
#include "playerUI.h"

                    // 클릭한 위치를 음악 시간으로 변환
                    int newPosition = (mousePosition * length) / barWidth;

void Open() {
	wchar_t file[MAX_PATH] = L"";
	OpenFileDialog(file);
	if (file[0] != L'\0') {
		Add(file);
	}
}

void ManageKeyboard() {
	if (_kbhit()) {
		switch (_getch()) {
		case ' ':
			Pause();
			break;
		case 'o':
			Open();
			break;
		case 224: //방향키
			switch (_getch()) {
			case 75: //왼쪽
				MoveTo(-1000);
				break;
			case 77:
				MoveTo(1000);
				break;
			}
		}
		UpdateUI(UpdateAll);
	}
}

void Update() {
	wchar_t currentTime[MAX_PATH];
	UpdateUI(UpdateAll);
	while (1) {
		if (!CheckPlayer()) break;
		if (wcscmp(currentTime, MsToTime(GetStatus(MCI_STATUS_POSITION))) != 0) {
			wcscpy(currentTime, MsToTime(GetStatus(MCI_STATUS_POSITION)));
			UpdateUI(UpdateTime);
		}
		else
			Sleep(10);
		ManageKeyboard();
	}
}

// 초기화 함수
void Init() {
	_wsetlocale(LC_ALL, L"Korean");
	system("mode con cols=80 lines=5");
	InitPlayer();
	Open();
	Ready(); Play();
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