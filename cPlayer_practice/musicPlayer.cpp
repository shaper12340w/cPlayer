#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <mmsystem.h>
#include "playerUI.h"
#include "dynamicArray.h"
#include "fileMannger.h"
#include "Digitalv.h"
#pragma comment (lib, "winmm.lib")

typedef struct {
	MCI_OPEN_PARMSW openParam;
	MCI_PLAY_PARMS playParam;
	MCI_DGV_STATUS_PARMSW pauseParam;
	MCI_STATUS_PARMS statusParam;
	MCI_SEEK_PARMS seekParam;
} MCI_PARAM;

typedef struct {
	int deviceID;
	int paused;
	StringArray pathArray;
} PlayerData;

MCI_PARAM param;
PlayerData player;

void Ready();
void Play();
void ResetParam();

void ExecuteMCI(MCIDEVICEID id, UINT msg, DWORD_PTR command, DWORD_PTR param) {
	DWORD result = mciSendCommandW(id, msg, command, param);
	if (result != 0) {
		WCHAR reason[256];
		mciGetErrorStringW(result, reason, 256);
		switch (msg) {
		case MCI_OPEN:
			if (result == MCIERR_INTERNAL) {
				wchar_t tempPath[MAX_PATH] = L"";
				SaveTemporaryMP3(getArrayValue(&player.pathArray, 0), tempPath, MAX_PATH);
				changeArrayValue(&player.pathArray, tempPath, 0);
				printf("\nWarning : MP3 tag error\nPlay with temp File..\n");
				Ready();
				return;
			}
			else
				wprintf(L"Error Opening Music : \n%ls\nCode : %d", reason,result);
			break;
		case MCI_PLAY:
			wprintf(L"Error Playing Music : \n%ls\nCode : %d", reason, result);
			break;
		case MCI_PAUSE:
			wprintf(L"Error Pausing Music : \n%ls\nCode : %d", reason, result);
			break;
		case MCI_STATUS:
			wprintf(L"Error Getting Information of Music : \n%ls\nCode : %d", reason, result);
			break;
		case MCI_STOP:
			wprintf(L"Error Stoping Music : \n%ls\nCode : %d", reason, result);
			break;
		case MCI_CLOSE:
			wprintf(L"Error Closing Music : \n%ls\nCode : %d", reason, result);
			break;
		default:
			wprintf(L"Unknown Error : \n%ls\nCode : %d", reason, result);
			break;
		}
		exit(1);
	}
	
}

void ResetParam() {
	memset(&param, 0, sizeof(MCI_PARAM)); //사용 후 파라미터 초기화
}

void InitValue() {
	memset(&param, 0, sizeof(MCI_PARAM));
	memset(&player, 0, sizeof(PlayerData));
	initArray(&player.pathArray,4,MAX_PATH);
}

int GetStatus(DWORD dwStatus) {
	param.statusParam.dwItem = dwStatus;
	ExecuteMCI(player.deviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&param.statusParam);
	int result = param.statusParam.dwReturn;
	ResetParam();
	return result;
}

void Ready() {
	param.openParam.lpstrElementName = getArrayValue(&player.pathArray,0); //경로 지정
	param.openParam.lpstrDeviceType = L"mpegvideo"; //코덱 타입
	ExecuteMCI(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD_PTR)&param.openParam);
	wprintf(L"\nReady to play Music : %ls\n", param.openParam.lpstrElementName);
}

void Add(const wchar_t* path) {
	addArrayValue(&player.pathArray, path);
	printf("\nSong Added!\n");
}

void InitPlayer() {
	InitValue();
	printf("\nInitialize complete!\n");
}

void Play() {
	player.deviceID = (int)param.openParam.wDeviceID;
	ExecuteMCI(player.deviceID, MCI_PLAY, 0, (DWORD_PTR)&param.playParam);
	printf("\nMusic is Playing\n");
	ResetParam();
}

void Pause() {
	if (!player.paused) {
		ExecuteMCI(player.deviceID, MCI_PAUSE, 0, (DWORD_PTR)&param.pauseParam);
		player.paused = 1;
	}
	else {
		ExecuteMCI(player.deviceID, MCI_PLAY, 0, (DWORD_PTR)&param.playParam);
		player.paused = 0;
	}
	ResetParam();
}

void MoveTo(int time) {
	int length = GetStatus(MCI_STATUS_LENGTH);
	int position = GetStatus(MCI_STATUS_POSITION);
	int moveTime = 0;

	if ((position + time) < 0) 
		moveTime = 0;
	else if ((position + time) > length) 
		moveTime = length;
	else 
		moveTime = position + time;

	param.seekParam.dwTo = moveTime;
	player.paused = 1;
	ExecuteMCI(player.deviceID, MCI_SEEK, MCI_TO, (DWORD_PTR)&param.seekParam);
	ExecuteMCI(player.deviceID, MCI_PLAY, 0, (DWORD_PTR)&param.playParam);
	player.paused = 0;
	ResetParam();
}

int CheckPlayer() {
	param.statusParam.dwItem = MCI_STATUS_MODE;
	ExecuteMCI(player.deviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&param.statusParam);
	if (param.statusParam.dwReturn == MCI_MODE_STOP && !player.paused) {
		if (getArrayLength(&player.pathArray) > 0) {
			removeArrayValue(&player.pathArray, 0);
			ExecuteMCI(player.deviceID, MCI_CLOSE, 0, 0);
			printf("\nMusic Stopped!\n");
			ResetParam();
			Ready(); Play();
			printf("\nPlay Next Music..\n");
			UpdateUI(UpdateAll);
		}
		else {
			ExecuteMCI(player.deviceID, MCI_CLOSE, 0, 0);
			printf("\nPlay Ended\n");
			ResetParam();
			Sleep(1500);
			return 0;
		}
	}
	return 1;
}

