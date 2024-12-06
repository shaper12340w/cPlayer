#pragma once // ��� ������ ���� �� ���ԵǴ� ���� ����
#include "dynamicArray.h" // dynamicArray.h ������ ����

// UpdateType ������ ����
enum UpdateType {
    UpdateClear, // ȭ���� ����� ������Ʈ
    UpdateAll, // ��� ���� ������Ʈ
    UpdateTime, // �ð��� ������Ʈ
    UpdateStatus, // ���¸� ������Ʈ
    UpdateList, // ����Ʈ�� ������Ʈ
};

// �Լ� ����
wchar_t* MsToTime(int ms); // �и��ʸ� �ð� �������� ��ȯ�ϴ� �Լ�
const wchar_t* GetMP3Name(const wchar_t* name); // MP3 ���� �̸��� �������� �Լ�
void readInput(INPUT_RECORD* rec, DWORD* eventWritten); // �Է��� �д� �Լ�
void clearLine(int line); // Ư�� ���� ����� �Լ�
void uprintf(int line, const wchar_t* str, ...); // Ư�� �ٿ� ���ڿ��� ����ϴ� �Լ�
void PrintList(StringArray* array); // ���ڿ� �迭�� ����ϴ� �Լ�
void UpdateUI(UpdateType type); // UI�� ������Ʈ�ϴ� �Լ�
