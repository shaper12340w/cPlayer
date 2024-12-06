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

// MCI (Media Control Interface) ���� �Ķ���� ����ü ����
// MCI ���� ���� ���� : https://learn.microsoft.com/ko-kr/windows/win32/multimedia/mci
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

// MCI ��� ���� �Լ� ����
void ExecuteMCI(MCIDEVICEID id, UINT msg, DWORD_PTR command, DWORD_PTR param);

// �÷��̾� �ʱ�ȭ �Լ� ����
void InitPlayer();

// ���� �������� �Լ� ����
int GetStatus(DWORD dwStatus);

// �� �߰� �Լ� ����
void Add(const wchar_t* path);

// �غ� �Լ� ����
void Ready();

// ��� �Լ� ����
void Play();

// �Ͻ� ���� �Լ� ����
void Pause();

// ���� �� ��� �Լ� ����
void Next();

// ���� �� ��� �Լ� ����
void Prev();

// Ư�� �ε����� �� ��� �Լ� ����
void PlayIndex(int index);

// ���� ��� �Լ� ����
void Shuffle();

// �ݺ� ��� �Լ� ����
void Repeat();

// Ư�� �ð����� �̵� �Լ� ����
void MoveTo(int time);

// �÷��̾� ���� Ȯ�� �Լ� ����
int CheckPlayer();

// MCI ��� ���� �Լ�
void ExecuteMCI(MCIDEVICEID id, UINT msg, DWORD_PTR command, DWORD_PTR param) {
    DWORD result = mciSendCommandW(id, msg, command, param); // MCI ��� ����
    if (result != 0) { // ���� �߻� ��
        WCHAR reason[256]; // ���� �޽��� ���� ����
        mciGetErrorStringW(result, reason, 256); // ���� �޽��� ��������
        switch (msg) { // ��� ������ ���� ���� �޽��� ���
        case MCI_OPEN: // MCI_OPEN ����� ��
            if (result == MCIERR_INTERNAL) { // ���� ���� �߻� ��
                wchar_t tempPath[MAX_PATH] = L""; // �ӽ� ���� ��� ����
                SaveTemporaryMP3(getStringArrayValue(&player.pathArray, player.playIndex), tempPath, MAX_PATH); // �ӽ� MP3 ���� ����
                changeStringArrayValue(&player.pathArray, tempPath, player.playIndex); // ��� �迭�� ���� �ӽ� ���� ��η� ����
                addIntArrayValue(&player.deletedArray, player.playIndex); // ������ �׸� �迭�� ���� �ε��� �߰�
                uprintf(6, L"Warning : MP3 tag error\nPlay with temp File.."); // ��� �޽��� ���
                Ready(); // �غ� �Լ� ȣ��
                return; // �Լ� ����
            }
            else // �ٸ� ���� �߻� ��
                uprintf(6, L"Error Opening Music : \n%ls\nCode : %d", reason, result); // ���� �޽��� ���
            break;
        case MCI_PLAY:  // MCI_PLAY ����� ��
            uprintf(6, L"Error Playing Music : \n%ls\nCode : %d", reason, result); // ���� �޽��� ���
            break;
        case MCI_PAUSE: // MCI_PAUSE ����� ��
            uprintf(6, L"Error Pausing Music : \n%ls\nCode : %d", reason, result); // ���� �޽��� ���
            break;
        case MCI_STATUS: // MCI_STATUS ����� ��
            uprintf(6, L"Error Getting Information of Music : \n%ls\nCode : %d", reason, result); // ���� �޽��� ���
            break;
        case MCI_STOP: // MCI_STOP ����� ��
            uprintf(6, L"Error Stopping Music : \n%ls\nCode : %d", reason, result); // ���� �޽��� ���
            break;
        case MCI_CLOSE: // MCI_CLOSE ����� ��
            uprintf(6, L"Error Closing Music : \n%ls\nCode : %d", reason, result); // ���� �޽��� ���
            break;
        default: // �� �� ����� ��
            uprintf(6, L"Player Error : \n%ls\nCode : %d", reason, result); // ���� �޽��� ���
            break;
        }
        exit(1); // ���α׷� ����
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
    if (!player.paused) { // �Ͻ� ���� ���°� �ƴ� ��
        clearLine(6); uprintf(6, L"Paused"); // �Ͻ� ���� �޽��� ���
        ExecuteMCI(player.deviceID, MCI_PAUSE, 0, (DWORD_PTR)&param.genericParam); // MCI �Ͻ� ���� ��� ����
        player.paused = 1; // �Ͻ� ���� ���·� ����
    }
    else { // �Ͻ� ���� ������ ��
        clearLine(6); uprintf(6, L"Resumed"); // �簳 �޽��� ���
        ExecuteMCI(player.deviceID, MCI_RESUME, 0, (DWORD_PTR)&param.genericParam); // MCI �簳 ��� ����
        player.paused = 0; // �Ͻ� ���� ����
    }
    ResetParam(); // �Ķ���� �ʱ�ȭ
}

// ���� �� ��� �Լ�
void Next() {
    if (player.paused) player.paused = 0; // �Ͻ� ���� ���� ����
    if (checkIntArrayValue(&player.playedArray, player.playIndex) == -1) {
        addIntArrayValue(&player.playedArray, player.playIndex); // ����� �׸� �迭�� �߰�
    }
    ExecuteMCI(player.deviceID, MCI_STOP, 0, (DWORD_PTR)&param.genericParam); // MCI ���� ��� ����
}

// ���� �� ��� �Լ�
void Prev() {
    if (player.repeat == 2) {
        MoveTo(-GetStatus(MCI_STATUS_POSITION));
        return;
    }
    if (player.playIndex > 0) { // ���� ��� �ε����� 0���� Ŭ ��
        int currentIndex = player.playIndex; // ���� �ε��� ����
        player.playIndex = getIntArrayValue(&player.playedArray, getIntArrayLength(&player.playedArray) - 2); // ���� �ε��� ����
        player.paused = 1; // �Ͻ� ���� ���·� ����

        ExecuteMCI(player.deviceID, MCI_STOP, 0, (DWORD_PTR)&param.genericParam); // MCI ���� ��� ����
        ExecuteMCI(player.deviceID, MCI_CLOSE, 0, 0); // MCI �ݱ� ��� ����

        int indexPlayed = checkIntArrayValue(&player.playedArray, currentIndex); // ���� �ε��� Ȯ��
        int indexToPlay = checkIntArrayValue(&player.playedArray, player.playIndex); // ���� �ε��� Ȯ��

        if (indexPlayed != -1)
            removeIntArrayValue(&player.playedArray, indexPlayed); // ���� �ε��� ����
        if (indexToPlay != -1)
            removeIntArrayValue(&player.playedArray, indexToPlay); // ���� �ε��� ����

        Ready(); // �غ� �Լ� ȣ��
        Play(); // ��� �Լ� ȣ��
        clearLine(6);
        uprintf(6, L"Play Prev Music.."); // ���� �� ��� �޽��� ���
        player.paused = 0; // �Ͻ� ���� ����
    }
}

void PlayIndex(int index) {
    int currentIndex = player.playIndex;
    int indexToPlay = checkIntArrayValue(&player.playedArray, index);
    if (indexToPlay != -1)
        removeIntArrayValue(&player.playedArray, indexToPlay); // ���� �ε��� ����
    player.paused = 1;
    player.playIndex = index;
    ExecuteMCI(player.deviceID, MCI_STOP, 0, (DWORD_PTR)&param.genericParam);
    ExecuteMCI(player.deviceID, MCI_CLOSE, 0, 0); // MCI �ݱ� ��� ����
    player.paused = 0;
    ResetParam();
    Ready(); Play(); // �غ� �� ��� �Լ� ȣ��
}

// ���� ��� �Լ�
void Shuffle() {
    if (!player.shuffle) { // ���� ���°� �ƴ� ��
        player.shuffle = 1; // ���� ���·� ����
        clearLine(6);
        uprintf(6, L"Shuffle Enabled"); // ���� Ȱ��ȭ �޽��� ���
    }
    else { // ���� ������ ��
        player.shuffle = 0; // ���� ����
        clearLine(6);
        uprintf(6, L"Shuffle Disabled"); // ���� ��Ȱ��ȭ �޽��� ���
    }
}

// �ݺ� ��� �Լ�
void Repeat() {
    player.repeat++; // �ݺ� ���� ����
    if (player.repeat == 3) player.repeat = 0; // �ݺ� ���� �ʱ�ȭ
    switch (player.repeat) {
    case 0:
        clearLine(6); uprintf(6, L"Repeat Mode : Not Repeat"); break; // �ݺ� ���� �޽��� ���
    case 1:
        clearLine(6); uprintf(6, L"Repeat Mode : Repeat ALL"); break; // ��ü �ݺ� �޽��� ���
    case 2:
        clearLine(6); uprintf(6, L"Repeat Mode : Repeat One"); break; // �� �� �ݺ� �޽��� ���
    }
}

// Ư�� �ð����� �̵� �Լ�
void MoveTo(int time) {
    int length = GetStatus(MCI_STATUS_LENGTH); // �� ���� ��������
    int position = GetStatus(MCI_STATUS_POSITION); // ���� ��ġ ��������
    int moveTime = 0;

    if ((position + time) < 0)
        moveTime = 0; // �̵� �ð��� 0���� ���� ��
    else if ((position + time) > length)
        moveTime = length; // �̵� �ð��� �� ���̺��� Ŭ ��
    else
        moveTime = position + time; // �̵� �ð� ����
    uprintf(6, L"Moved to %ls", MsToTime(moveTime)); // �̵� �޽��� ���
    param.seekParam.dwTo = moveTime; // �̵� �ð� ����
    player.paused = 1; // �Ͻ� ���� ���·� ����
    ExecuteMCI(player.deviceID, MCI_SEEK, MCI_TO, (DWORD_PTR)&param.seekParam); // MCI Ž�� ��� ����
    ExecuteMCI(player.deviceID, MCI_PLAY, 0, (DWORD_PTR)&param.playParam); // MCI ��� ��� ����
    player.paused = 0; // �Ͻ� ���� ����
    ResetParam(); // �Ķ���� �ʱ�ȭ
}

// �÷��̾� ���� Ȯ�� �Լ�
int CheckPlayer() {
    param.statusParam.dwItem = MCI_STATUS_MODE; // ���� �׸� ����
    ExecuteMCI(player.deviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&param.statusParam); // MCI ���� ��� ����
    if (param.statusParam.dwReturn == MCI_MODE_STOP && !player.paused) { // ���� �����̰� �Ͻ� ���� ���°� �ƴ� ��
        if (getStringArrayLength(&player.pathArray) > getIntArrayLength(&player.playedArray)) { // ����� ���� ���� ���� ��
            
            IntArray leftArray;
            initIntArray(&leftArray, 4); // �ʱ� �뷮 ����
            for (int i = 0; i < getStringArrayLength(&player.pathArray); i++) {
                addIntArrayValue(&leftArray, i); // ���� �� �迭�� �߰�
            }
            for (int i = 0; i < getIntArrayLength(&player.playedArray); i++) {
                int indexToRemove = checkIntArrayValue(&leftArray, getIntArrayValue(&player.playedArray, i));
                if (indexToRemove != -1) {
                    removeIntArrayValue(&leftArray, indexToRemove); // �̹� ����� �� ����
                }
            }

            if (player.repeat == 2) {
                removeIntArrayValue(&player.playedArray, player.playIndex); // ���� �ε��� ����
            }
            else if (player.shuffle) {
                srand((unsigned int)time(NULL)); // ���� �õ� ����

                if (getIntArrayLength(&leftArray) >= 0) {
                    int randValue = rand() % getIntArrayLength(&leftArray); // ���� �ε��� ����
                    player.playIndex = getIntArrayValue(&leftArray, randValue); // ���� �ε��� ����
                }
                else {
                    printf("Error: No songs left to shuffle.\n"); // ������ ���� ���� ��
                }
                removeIntArray(&leftArray); // ���� �� �迭 ����
            }
            else if (checkIntArrayValue(&leftArray, player.playIndex + 1) != -1) {
                player.playIndex++;
            }
            else
                player.playIndex = getIntArrayValue(&leftArray, 0); // ��� ���� �� �� ���� �� �ε��� ����
            ExecuteMCI(player.deviceID, MCI_CLOSE, 0, 0); // MCI �ݱ� ��� ����
            ResetParam(); // �Ķ���� �ʱ�ȭ
            Ready(); Play(); // �غ� �� ��� �Լ� ȣ��
            clearLine(6); uprintf(6, L"Play Next Music.."); // ���� �� ��� �޽��� ���
            UpdateUI(UpdateAll); // UI ������Ʈ
        }
        else {
            if (player.repeat == 1) { // ��ü �ݺ� ������ ��
                ExecuteMCI(player.deviceID, MCI_CLOSE, 0, 0); // MCI �ݱ� ��� ����
                removeIntArray(&player.playedArray); // ����� �׸� �迭 ����
                initIntArray(&player.playedArray, 4); // ����� �׸� �迭 �ʱ�ȭ
                player.playIndex = 0; // �ε����� 0���� �ʱ�ȭ
                ResetParam(); // �Ķ���� �ʱ�ȭ
                Ready(); Play(); // �غ� �� ��� �Լ� ȣ��
                return 1; // �Լ� ����
            }
            ExecuteMCI(player.deviceID, MCI_CLOSE, 0, 0); // MCI �ݱ� ��� ����
            uprintf(6, L"Play Ended"); // ��� ���� �޽��� ���
            ResetParam(); // �Ķ���� �ʱ�ȭ
            CleanupTempFolder(); // �ӽ� ���� ����
            system("pause"); // �Ͻ� ����
            return 0; // �Լ� ����
        }
    }
    return 1; // �Լ� ����
}
