#pragma once
#include "dynamicArray.h"
enum UpdateType {
    UpdateClear,
    UpdateAll,
    UpdateTime,
    UpdateStatus,
    UpdateList,
};

wchar_t* MsToTime(int ms);
const wchar_t* GetMP3Name(const wchar_t* name);
void readInput(INPUT_RECORD* rec,DWORD* eventWritten);
void clearLine(int line);
void uprintf(int line, const wchar_t* str, ...);
void PrintList(StringArray* array);
void UpdateUI(UpdateType type);
