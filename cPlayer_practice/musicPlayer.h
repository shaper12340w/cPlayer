#pragma once // ��� ������ ���� �� ���ԵǴ� ���� ����
#include <stdio.h> // ǥ�� ����� �Լ� ����� ���� ��� ���� ����
#include <windows.h> // Windows API �Լ� ����� ���� ��� ���� ����
#include <mmsystem.h> // ��Ƽ�̵�� �ý��� �Լ� ����� ���� ��� ���� ����
#include "dynamicArray.h" // ����� ���� ���� �迭 ��� ���� ����
#include "DigitalV.h" // ����� ���� ������ ���� ��� ���� ����
#pragma comment (lib, "winmm.lib") // winmm.lib ���̺귯�� ��ũ

typedef struct { // MCI_PARAM ����ü ���� ����
    MCI_OPEN_PARMSW openParam; // MCI ���� �Ķ����
    MCI_PLAY_PARMS playParam; // MCI ��� �Ķ����
    MCI_GENERIC_PARMS genericParam; // MCI �Ϲ� �Ķ����
    MCI_DGV_STATUS_PARMSW pauseParam; // MCI �Ͻ����� ���� �Ķ����
    MCI_STATUS_PARMS statusParam; // MCI ���� �Ķ����
} MCI_PARAM; // MCI_PARAM ����ü ���� ��

typedef struct { // PlayerData ����ü ���� ����
    int deviceID; // ��ġ ID
    int paused; // �Ͻ����� ����
    int repeat; // �ݺ� ��� ����
    int shuffle; // ���� ��� ����
    int playIndex; // ��� �ε���
    StringArray pathArray; // ��� �迭
    IntArray deletedArray; // ������ �׸� �迭
    IntArray playedArray; // ����� �׸� �迭
} PlayerData; // PlayerData ����ü ���� ��

extern MCI_PARAM param; // �ܺο��� ����� MCI_PARAM ���� ����
extern PlayerData player; // �ܺο��� ����� PlayerData ���� ����

void ExecuteMCI(MCIDEVICEID id, UINT msg, DWORD_PTR command, DWORD_PTR param); // MCI ��� ���� �Լ� ����
void InitPlayer(); // �÷��̾� �ʱ�ȭ �Լ� ����
int GetStatus(DWORD dwStatus); // ���� �������� �Լ� ����
void Add(const wchar_t* path); // ��� �߰� �Լ� ����
void Ready(); // �غ� �Լ� ����
void Play(); // ��� �Լ� ����
void Pause(); // �Ͻ����� �Լ� ����
void Next(); // ���� �� ��� �Լ� ����
void Prev(); // ���� �� ��� �Լ� ����
void PlayIndex(int index); // Ư�� �ε��� ��� �Լ� ����
void Shuffle(); // ���� ��� �Լ� ����
void Repeat(); // �ݺ� ��� �Լ� ����
void MoveTo(int time); // Ư�� �ð����� �̵� �Լ� ����
int CheckPlayer(); // �÷��̾� ���� Ȯ�� �Լ� ����
// ��� ����
