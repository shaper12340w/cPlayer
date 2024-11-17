
#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include "dynamicArray.h"
#include "DigitalV.h"
#pragma comment (lib, "winmm.lib")

typedef struct {
	MCI_OPEN_PARMSW openParam;
	MCI_PLAY_PARMS playParam;
	MCI_DGV_STATUS_PARMSW pauseParam;
	MCI_STATUS_PARMS statusParam;
} MCI_PARAM;

typedef struct {
	int deviceID;
	int paused;
	StringArray pathArray;
} PlayerData;

extern MCI_PARAM param;
extern PlayerData player;

void ExecuteMCI(MCIDEVICEID id, UINT msg, DWORD_PTR command, DWORD_PTR param);
void InitPlayer();
int GetStatus(DWORD dwStatus);
void Add(const wchar_t* path);
void Ready();
void Play();
void Pause();
void MoveTo(int time);
int CheckPlayer();
// 헤더 내용

#endif 