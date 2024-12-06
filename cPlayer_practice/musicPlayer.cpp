#include <stdio.h> // 표준 입출력 함수 사용
#include <windows.h> // Windows API 함수 사용
#include <string.h> // 문자열 처리 함수 사용
#include <time.h> // 시간 관련 함수 사용
#include <stdlib.h> // 표준 라이브러리 함수 사용
#include <mmsystem.h> // 멀티미디어 시스템 함수 사용
#include "playerUI.h" // 사용자 인터페이스 관련 함수 및 정의 포함
#include "dynamicArray.h" // 동적 배열 관련 함수 및 정의 포함
#include "fileMannger.h" // 파일 관리 관련 함수 및 정의 포함
#include "Digitalv.h" // 디지털 비디오 관련 함수 사용
#pragma comment (lib, "winmm.lib") // winmm 라이브러리 링크

typedef struct {
    MCI_OPEN_PARMSW openParam; // MCI 열기 파라미터
    MCI_PLAY_PARMS playParam; // MCI 재생 파라미터
    MCI_GENERIC_PARMS genericParam; // MCI 일반 파라미터
    MCI_STATUS_PARMS statusParam; // MCI 상태 파라미터
    MCI_SEEK_PARMS seekParam; // MCI 탐색 파라미터
} MCI_PARAM;

// 플레이어 데이터 구조체 정의
typedef struct {
    int deviceID; // 장치 ID
    int paused; // 일시 정지 상태
    int repeat; // 반복 재생 상태
    int shuffle; // 셔플 재생 상태
    int playIndex; // 현재 재생 인덱스
    StringArray pathArray; // 경로 배열
    IntArray deletedArray; // 삭제된 항목 배열
    IntArray playedArray; // 재생된 항목 배열
} PlayerData;

MCI_PARAM param; // MCI 파라미터 변수
PlayerData player; // 플레이어 데이터 변수

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

// 파라미터 초기화 함수
void ResetParam() {
    memset(&param, 0, sizeof(MCI_PARAM)); // 파라미터 구조체 초기화
}

// 초기 값 설정 함수
void InitValue() {
    memset(&param, 0, sizeof(MCI_PARAM)); // MCI 파라미터 초기화
    memset(&player, 0, sizeof(PlayerData)); // 플레이어 데이터 초기화
    initStringArray(&player.pathArray, 4, MAX_PATH); // 경로 배열 초기화
    initIntArray(&player.playedArray, 4); // 재생된 항목 배열 초기화
    initIntArray(&player.deletedArray, 4); // 삭제된 항목 배열 초기화
}

// 상태 가져오기 함수
int GetStatus(DWORD dwStatus) {
    param.statusParam.dwItem = dwStatus; // 상태 항목 설정
    ExecuteMCI(player.deviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&param.statusParam); // MCI 상태 명령 실행
    int result = param.statusParam.dwReturn; // 결과 값 저장
    ResetParam(); // 파라미터 초기화
    return result; // 결과 값 반환
}

// 준비 함수
void Ready() {
    param.openParam.lpstrElementName = getStringArrayValue(&player.pathArray, player.playIndex); // 경로 설정
    param.openParam.lpstrDeviceType = L"mpegvideo"; // 코덱 타입 설정
    ExecuteMCI(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD_PTR)&param.openParam); // MCI 열기 명령 실행
}

// 곡 추가 함수
void Add(const wchar_t* path) {
    clearLine(6); uprintf(6, L"Song Added! : %ls", GetMP3Name(path)); // 곡 추가 메시지 출력
    addStringArrayValue(&player.pathArray, path); // 경로 배열에 곡 추가
}

// 플레이어 초기화 함수
void InitPlayer() {
    InitValue(); // 초기 값 설정
    uprintf(6, L"Initialize complete!"); // 초기화 완료 메시지 출력
}

// 재생 함수
void Play() {
    player.deviceID = (int)param.openParam.wDeviceID; // 장치 ID 설정
    ExecuteMCI(player.deviceID, MCI_PLAY, 0, (DWORD_PTR)&param.playParam); // MCI 재생 명령 실행
    addIntArrayValue(&player.playedArray, player.playIndex); // 재생된 항목 배열에 추가
    clearLine(6); uprintf(6, L"Music is Playing"); // 재생 중 메시지 출력
    ResetParam(); // 파라미터 초기화
}

// 일시 정지 함수
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

// 특정 시간으로 이동 함수
void MoveTo(int time) {
    int length = GetStatus(MCI_STATUS_LENGTH); // 곡 길이 가져오기
    int position = GetStatus(MCI_STATUS_POSITION); // 현재 위치 가져오기
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
			DeleteTempFile();
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
