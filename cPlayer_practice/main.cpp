#define _CRT_SECURE_NO_WARNINGS
#include <conio.h>
#include <stdio.h>
#include <locale.h>
#include "musicPlayer.h"
#include "fileMannger.h"
#include "playerUI.h"


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
			case 80: //왼쪽
				MoveTo(-1000);
				break;
			case 77:
				MoveTo(1000);
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

void Init() {
	_wsetlocale(LC_ALL, L"Korean");
	system("mode con cols=80 lines=5");
	InitPlayer();
	Open();
	Ready(); Play();
}

int main() {
	Init();
	Update();
	return 0;
}