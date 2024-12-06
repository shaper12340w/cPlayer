#include <stdio.h> // ǥ�� ����� �Լ� ���
#include <windows.h> // Windows API �Լ� ���
#include <string.h> // ���ڿ� ó�� �Լ� ���
#include <time.h> // �ð� ���� �Լ� ���
#include <stdlib.h> // ǥ�� ���̺귯�� �Լ� ���
#include <mmsystem.h> // ��Ƽ�̵�� �ý��� �Լ� ���
#include "playerUI.h" // ����� �������̽� ���� �Լ� �� ���� ����
#include "dynamicArray.h" // ���� �迭 ���� �Լ� �� ���� ����
#include "fileMannger.h" // ���� ���� ���� �Լ� �� ���� ����
#include "Digitalv.h" // ������ ���� ���� �Լ� ���
#pragma comment (lib, "winmm.lib") // winmm ���̺귯�� ��ũ

typedef struct {
    MCI_OPEN_PARMSW openParam; // MCI ���� �Ķ����
    MCI_PLAY_PARMS playParam; // MCI ��� �Ķ����
    MCI_GENERIC_PARMS genericParam; // MCI �Ϲ� �Ķ����
    MCI_STATUS_PARMS statusParam; // MCI ���� �Ķ����
    MCI_SEEK_PARMS seekParam; // MCI Ž�� �Ķ����
} MCI_PARAM;

// �÷��̾� ������ ����ü ����
typedef struct {
    int deviceID; // ��ġ ID
    int paused; // �Ͻ� ���� ����
    int repeat; // �ݺ� ��� ����
    int shuffle; // ���� ��� ����
    int playIndex; // ���� ��� �ε���
    StringArray pathArray; // ��� �迭
    IntArray deletedArray; // ������ �׸� �迭
    IntArray playedArray; // ����� �׸� �迭
} PlayerData;

MCI_PARAM param; // MCI �Ķ���� ����
PlayerData player; // �÷��̾� ������ ����

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

// �Ķ���� �ʱ�ȭ �Լ�
void ResetParam() {
    memset(&param, 0, sizeof(MCI_PARAM)); // �Ķ���� ����ü �ʱ�ȭ
}

// �ʱ� �� ���� �Լ�
void InitValue() {
    memset(&param, 0, sizeof(MCI_PARAM)); // MCI �Ķ���� �ʱ�ȭ
    memset(&player, 0, sizeof(PlayerData)); // �÷��̾� ������ �ʱ�ȭ
    initStringArray(&player.pathArray, 4, MAX_PATH); // ��� �迭 �ʱ�ȭ
    initIntArray(&player.playedArray, 4); // ����� �׸� �迭 �ʱ�ȭ
    initIntArray(&player.deletedArray, 4); // ������ �׸� �迭 �ʱ�ȭ
}

// ���� �������� �Լ�
int GetStatus(DWORD dwStatus) {
    param.statusParam.dwItem = dwStatus; // ���� �׸� ����
    ExecuteMCI(player.deviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&param.statusParam); // MCI ���� ��� ����
    int result = param.statusParam.dwReturn; // ��� �� ����
    ResetParam(); // �Ķ���� �ʱ�ȭ
    return result; // ��� �� ��ȯ
}

// �غ� �Լ�
void Ready() {
    param.openParam.lpstrElementName = getStringArrayValue(&player.pathArray, player.playIndex); // ��� ����
    param.openParam.lpstrDeviceType = L"mpegvideo"; // �ڵ� Ÿ�� ����
    ExecuteMCI(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD_PTR)&param.openParam); // MCI ���� ��� ����
}

// �� �߰� �Լ�
void Add(const wchar_t* path) {
    clearLine(6); uprintf(6, L"Song Added! : %ls", GetMP3Name(path)); // �� �߰� �޽��� ���
    addStringArrayValue(&player.pathArray, path); // ��� �迭�� �� �߰�
}

// �÷��̾� �ʱ�ȭ �Լ�
void InitPlayer() {
    InitValue(); // �ʱ� �� ����
    uprintf(6, L"Initialize complete!"); // �ʱ�ȭ �Ϸ� �޽��� ���
}

// ��� �Լ�
void Play() {
    player.deviceID = (int)param.openParam.wDeviceID; // ��ġ ID ����
    ExecuteMCI(player.deviceID, MCI_PLAY, 0, (DWORD_PTR)&param.playParam); // MCI ��� ��� ����
    addIntArrayValue(&player.playedArray, player.playIndex); // ����� �׸� �迭�� �߰�
    clearLine(6); uprintf(6, L"Music is Playing"); // ��� �� �޽��� ���
    ResetParam(); // �Ķ���� �ʱ�ȭ
}

// �Ͻ� ���� �Լ�
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

// Ư�� �ð����� �̵� �Լ�
void MoveTo(int time) {
    int length = GetStatus(MCI_STATUS_LENGTH); // �� ���� ��������
    int position = GetStatus(MCI_STATUS_POSITION); // ���� ��ġ ��������
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
