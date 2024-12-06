#pragma once // 헤더 파일이 여러 번 포함되는 것을 방지
#include "dynamicArray.h" // dynamicArray.h 파일을 포함

// UpdateType 열거형 정의
enum UpdateType {
    UpdateClear, // 화면을 지우는 업데이트
    UpdateAll, // 모든 것을 업데이트
    UpdateTime, // 시간을 업데이트
    UpdateStatus, // 상태를 업데이트
    UpdateList, // 리스트를 업데이트
};

// 함수 선언
wchar_t* MsToTime(int ms); // 밀리초를 시간 형식으로 변환하는 함수
const wchar_t* GetMP3Name(const wchar_t* name); // MP3 파일 이름을 가져오는 함수
void readInput(INPUT_RECORD* rec, DWORD* eventWritten); // 입력을 읽는 함수
void clearLine(int line); // 특정 줄을 지우는 함수
void uprintf(int line, const wchar_t* str, ...); // 특정 줄에 문자열을 출력하는 함수
void PrintList(StringArray* array); // 문자열 배열을 출력하는 함수
void UpdateUI(UpdateType type); // UI를 업데이트하는 함수
