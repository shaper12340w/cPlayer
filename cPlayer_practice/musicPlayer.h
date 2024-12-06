#pragma once // 헤더 파일이 여러 번 포함되는 것을 방지
#include <stdio.h> // 표준 입출력 함수 사용을 위한 헤더 파일 포함
#include <windows.h> // Windows API 함수 사용을 위한 헤더 파일 포함
#include <mmsystem.h> // 멀티미디어 시스템 함수 사용을 위한 헤더 파일 포함
#include "dynamicArray.h" // 사용자 정의 동적 배열 헤더 파일 포함
#include "DigitalV.h" // 사용자 정의 디지털 비디오 헤더 파일 포함
#pragma comment (lib, "winmm.lib") // winmm.lib 라이브러리 링크

typedef struct { // MCI_PARAM 구조체 정의 시작
    MCI_OPEN_PARMSW openParam; // MCI 열기 파라미터
    MCI_PLAY_PARMS playParam; // MCI 재생 파라미터
    MCI_GENERIC_PARMS genericParam; // MCI 일반 파라미터
    MCI_DGV_STATUS_PARMSW pauseParam; // MCI 일시정지 상태 파라미터
    MCI_STATUS_PARMS statusParam; // MCI 상태 파라미터
} MCI_PARAM; // MCI_PARAM 구조체 정의 끝

typedef struct { // PlayerData 구조체 정의 시작
    int deviceID; // 장치 ID
    int paused; // 일시정지 상태
    int repeat; // 반복 재생 여부
    int shuffle; // 셔플 재생 여부
    int playIndex; // 재생 인덱스
    StringArray pathArray; // 경로 배열
    IntArray deletedArray; // 삭제된 항목 배열
    IntArray playedArray; // 재생된 항목 배열
} PlayerData; // PlayerData 구조체 정의 끝

extern MCI_PARAM param; // 외부에서 선언된 MCI_PARAM 변수 참조
extern PlayerData player; // 외부에서 선언된 PlayerData 변수 참조

void ExecuteMCI(MCIDEVICEID id, UINT msg, DWORD_PTR command, DWORD_PTR param); // MCI 명령 실행 함수 선언
void InitPlayer(); // 플레이어 초기화 함수 선언
int GetStatus(DWORD dwStatus); // 상태 가져오기 함수 선언
void Add(const wchar_t* path); // 경로 추가 함수 선언
void Ready(); // 준비 함수 선언
void Play(); // 재생 함수 선언
void Pause(); // 일시정지 함수 선언
void Next(); // 다음 곡 재생 함수 선언
void Prev(); // 이전 곡 재생 함수 선언
void PlayIndex(int index); // 특정 인덱스 재생 함수 선언
void Shuffle(); // 셔플 재생 함수 선언
void Repeat(); // 반복 재생 함수 선언
void MoveTo(int time); // 특정 시간으로 이동 함수 선언
int CheckPlayer(); // 플레이어 상태 확인 함수 선언
// 헤더 내용
