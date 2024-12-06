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

// MCI (Media Control Interface) 관련 파라미터 구조체 정의
// MCI 관련 문서 참고 : https://learn.microsoft.com/ko-kr/windows/win32/multimedia/mci
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

// MCI 명령 실행 함수 선언
void ExecuteMCI(MCIDEVICEID id, UINT msg, DWORD_PTR command, DWORD_PTR param);

// 플레이어 초기화 함수 선언
void InitPlayer();

// 상태 가져오기 함수 선언
int GetStatus(DWORD dwStatus);

// 곡 추가 함수 선언
void Add(const wchar_t* path);

// 준비 함수 선언
void Ready();

// 재생 함수 선언
void Play();

// 일시 정지 함수 선언
void Pause();

// 다음 곡 재생 함수 선언
void Next();

// 이전 곡 재생 함수 선언
void Prev();

// 특정 인덱스의 곡 재생 함수 선언
void PlayIndex(int index);

// 셔플 재생 함수 선언
void Shuffle();

// 반복 재생 함수 선언
void Repeat();

// 특정 시간으로 이동 함수 선언
void MoveTo(int time);

// 플레이어 상태 확인 함수 선언
int CheckPlayer();

// MCI 명령 실행 함수
void ExecuteMCI(MCIDEVICEID id, UINT msg, DWORD_PTR command, DWORD_PTR param) {
    DWORD result = mciSendCommandW(id, msg, command, param); // MCI 명령 전송
    if (result != 0) { // 오류 발생 시
        WCHAR reason[256]; // 오류 메시지 저장 버퍼
        mciGetErrorStringW(result, reason, 256); // 오류 메시지 가져오기
        switch (msg) { // 명령 종류에 따라 오류 메시지 출력
        case MCI_OPEN: // MCI_OPEN 명령일 때
            if (result == MCIERR_INTERNAL) { // 내부 오류 발생 시
                wchar_t tempPath[MAX_PATH] = L""; // 임시 파일 경로 버퍼
                SaveTemporaryMP3(getStringArrayValue(&player.pathArray, player.playIndex), tempPath, MAX_PATH); // 임시 MP3 파일 저장
                changeStringArrayValue(&player.pathArray, tempPath, player.playIndex); // 경로 배열의 값을 임시 파일 경로로 변경
                addIntArrayValue(&player.deletedArray, player.playIndex); // 삭제된 항목 배열에 현재 인덱스 추가
                uprintf(6, L"Warning : MP3 tag error\nPlay with temp File.."); // 경고 메시지 출력
                Ready(); // 준비 함수 호출
                return; // 함수 종료
            }
            else // 다른 오류 발생 시
                uprintf(6, L"Error Opening Music : \n%ls\nCode : %d", reason, result); // 오류 메시지 출력
            break;
        case MCI_PLAY:  // MCI_PLAY 명령일 때
            uprintf(6, L"Error Playing Music : \n%ls\nCode : %d", reason, result); // 오류 메시지 출력
            break;
        case MCI_PAUSE: // MCI_PAUSE 명령일 때
            uprintf(6, L"Error Pausing Music : \n%ls\nCode : %d", reason, result); // 오류 메시지 출력
            break;
        case MCI_STATUS: // MCI_STATUS 명령일 때
            uprintf(6, L"Error Getting Information of Music : \n%ls\nCode : %d", reason, result); // 오류 메시지 출력
            break;
        case MCI_STOP: // MCI_STOP 명령일 때
            uprintf(6, L"Error Stopping Music : \n%ls\nCode : %d", reason, result); // 오류 메시지 출력
            break;
        case MCI_CLOSE: // MCI_CLOSE 명령일 때
            uprintf(6, L"Error Closing Music : \n%ls\nCode : %d", reason, result); // 오류 메시지 출력
            break;
        default: // 그 외 명령일 때
            uprintf(6, L"Player Error : \n%ls\nCode : %d", reason, result); // 오류 메시지 출력
            break;
        }
        exit(1); // 프로그램 종료
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
    if (!player.paused) { // 일시 정지 상태가 아닐 때
        clearLine(6); uprintf(6, L"Paused"); // 일시 정지 메시지 출력
        ExecuteMCI(player.deviceID, MCI_PAUSE, 0, (DWORD_PTR)&param.genericParam); // MCI 일시 정지 명령 실행
        player.paused = 1; // 일시 정지 상태로 설정
    }
    else { // 일시 정지 상태일 때
        clearLine(6); uprintf(6, L"Resumed"); // 재개 메시지 출력
        ExecuteMCI(player.deviceID, MCI_RESUME, 0, (DWORD_PTR)&param.genericParam); // MCI 재개 명령 실행
        player.paused = 0; // 일시 정지 해제
    }
    ResetParam(); // 파라미터 초기화
}

// 다음 곡 재생 함수
void Next() {
    if (player.paused) player.paused = 0; // 일시 정지 상태 해제
    if (checkIntArrayValue(&player.playedArray, player.playIndex) == -1) {
        addIntArrayValue(&player.playedArray, player.playIndex); // 재생된 항목 배열에 추가
    }
    ExecuteMCI(player.deviceID, MCI_STOP, 0, (DWORD_PTR)&param.genericParam); // MCI 정지 명령 실행
}

// 이전 곡 재생 함수
void Prev() {
    if (player.repeat == 2) {
        MoveTo(-GetStatus(MCI_STATUS_POSITION));
        return;
    }
    if (player.playIndex > 0) { // 현재 재생 인덱스가 0보다 클 때
        int currentIndex = player.playIndex; // 현재 인덱스 저장
        player.playIndex = getIntArrayValue(&player.playedArray, getIntArrayLength(&player.playedArray) - 2); // 이전 인덱스 설정
        player.paused = 1; // 일시 정지 상태로 설정

        ExecuteMCI(player.deviceID, MCI_STOP, 0, (DWORD_PTR)&param.genericParam); // MCI 정지 명령 실행
        ExecuteMCI(player.deviceID, MCI_CLOSE, 0, 0); // MCI 닫기 명령 실행

        int indexPlayed = checkIntArrayValue(&player.playedArray, currentIndex); // 현재 인덱스 확인
        int indexToPlay = checkIntArrayValue(&player.playedArray, player.playIndex); // 이전 인덱스 확인

        if (indexPlayed != -1)
            removeIntArrayValue(&player.playedArray, indexPlayed); // 현재 인덱스 제거
        if (indexToPlay != -1)
            removeIntArrayValue(&player.playedArray, indexToPlay); // 이전 인덱스 제거

        Ready(); // 준비 함수 호출
        Play(); // 재생 함수 호출
        clearLine(6);
        uprintf(6, L"Play Prev Music.."); // 이전 곡 재생 메시지 출력
        player.paused = 0; // 일시 정지 해제
    }
}

void PlayIndex(int index) {
    int currentIndex = player.playIndex;
    int indexToPlay = checkIntArrayValue(&player.playedArray, index);
    if (indexToPlay != -1)
        removeIntArrayValue(&player.playedArray, indexToPlay); // 이전 인덱스 제거
    player.paused = 1;
    player.playIndex = index;
    ExecuteMCI(player.deviceID, MCI_STOP, 0, (DWORD_PTR)&param.genericParam);
    ExecuteMCI(player.deviceID, MCI_CLOSE, 0, 0); // MCI 닫기 명령 실행
    player.paused = 0;
    ResetParam();
    Ready(); Play(); // 준비 및 재생 함수 호출
}

// 셔플 재생 함수
void Shuffle() {
    if (!player.shuffle) { // 셔플 상태가 아닐 때
        player.shuffle = 1; // 셔플 상태로 설정
        clearLine(6);
        uprintf(6, L"Shuffle Enabled"); // 셔플 활성화 메시지 출력
    }
    else { // 셔플 상태일 때
        player.shuffle = 0; // 셔플 해제
        clearLine(6);
        uprintf(6, L"Shuffle Disabled"); // 셔플 비활성화 메시지 출력
    }
}

// 반복 재생 함수
void Repeat() {
    player.repeat++; // 반복 상태 증가
    if (player.repeat == 3) player.repeat = 0; // 반복 상태 초기화
    switch (player.repeat) {
    case 0:
        clearLine(6); uprintf(6, L"Repeat Mode : Not Repeat"); break; // 반복 없음 메시지 출력
    case 1:
        clearLine(6); uprintf(6, L"Repeat Mode : Repeat ALL"); break; // 전체 반복 메시지 출력
    case 2:
        clearLine(6); uprintf(6, L"Repeat Mode : Repeat One"); break; // 한 곡 반복 메시지 출력
    }
}

// 특정 시간으로 이동 함수
void MoveTo(int time) {
    int length = GetStatus(MCI_STATUS_LENGTH); // 곡 길이 가져오기
    int position = GetStatus(MCI_STATUS_POSITION); // 현재 위치 가져오기
    int moveTime = 0;

    if ((position + time) < 0)
        moveTime = 0; // 이동 시간이 0보다 작을 때
    else if ((position + time) > length)
        moveTime = length; // 이동 시간이 곡 길이보다 클 때
    else
        moveTime = position + time; // 이동 시간 설정
    uprintf(6, L"Moved to %ls", MsToTime(moveTime)); // 이동 메시지 출력
    param.seekParam.dwTo = moveTime; // 이동 시간 설정
    player.paused = 1; // 일시 정지 상태로 설정
    ExecuteMCI(player.deviceID, MCI_SEEK, MCI_TO, (DWORD_PTR)&param.seekParam); // MCI 탐색 명령 실행
    ExecuteMCI(player.deviceID, MCI_PLAY, 0, (DWORD_PTR)&param.playParam); // MCI 재생 명령 실행
    player.paused = 0; // 일시 정지 해제
    ResetParam(); // 파라미터 초기화
}

// 플레이어 상태 확인 함수
int CheckPlayer() {
    param.statusParam.dwItem = MCI_STATUS_MODE; // 상태 항목 설정
    ExecuteMCI(player.deviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&param.statusParam); // MCI 상태 명령 실행
    if (param.statusParam.dwReturn == MCI_MODE_STOP && !player.paused) { // 정지 상태이고 일시 정지 상태가 아닐 때
        if (getStringArrayLength(&player.pathArray) > getIntArrayLength(&player.playedArray)) { // 재생할 곡이 남아 있을 때
            
            IntArray leftArray;
            initIntArray(&leftArray, 4); // 초기 용량 설정
            for (int i = 0; i < getStringArrayLength(&player.pathArray); i++) {
                addIntArrayValue(&leftArray, i); // 남은 곡 배열에 추가
            }
            for (int i = 0; i < getIntArrayLength(&player.playedArray); i++) {
                int indexToRemove = checkIntArrayValue(&leftArray, getIntArrayValue(&player.playedArray, i));
                if (indexToRemove != -1) {
                    removeIntArrayValue(&leftArray, indexToRemove); // 이미 재생된 곡 제거
                }
            }

            if (player.repeat == 2) {
                removeIntArrayValue(&player.playedArray, player.playIndex); // 현재 인덱스 제거
            }
            else if (player.shuffle) {
                srand((unsigned int)time(NULL)); // 랜덤 시드 설정

                if (getIntArrayLength(&leftArray) >= 0) {
                    int randValue = rand() % getIntArrayLength(&leftArray); // 랜덤 인덱스 설정
                    player.playIndex = getIntArrayValue(&leftArray, randValue); // 랜덤 인덱스 설정
                }
                else {
                    printf("Error: No songs left to shuffle.\n"); // 셔플할 곡이 없을 때
                }
                removeIntArray(&leftArray); // 남은 곡 배열 제거
            }
            else if (checkIntArrayValue(&leftArray, player.playIndex + 1) != -1) {
                player.playIndex++;
            }
            else
                player.playIndex = getIntArrayValue(&leftArray, 0); // 재생 안한 곡 중 다음 곡 인덱스 설정
            ExecuteMCI(player.deviceID, MCI_CLOSE, 0, 0); // MCI 닫기 명령 실행
            ResetParam(); // 파라미터 초기화
            Ready(); Play(); // 준비 및 재생 함수 호출
            clearLine(6); uprintf(6, L"Play Next Music.."); // 다음 곡 재생 메시지 출력
            UpdateUI(UpdateAll); // UI 업데이트
        }
        else {
            if (player.repeat == 1) { // 전체 반복 상태일 때
                ExecuteMCI(player.deviceID, MCI_CLOSE, 0, 0); // MCI 닫기 명령 실행
                removeIntArray(&player.playedArray); // 재생된 항목 배열 제거
                initIntArray(&player.playedArray, 4); // 재생된 항목 배열 초기화
                player.playIndex = 0; // 인덱스를 0으로 초기화
                ResetParam(); // 파라미터 초기화
                Ready(); Play(); // 준비 및 재생 함수 호출
                return 1; // 함수 종료
            }
            ExecuteMCI(player.deviceID, MCI_CLOSE, 0, 0); // MCI 닫기 명령 실행
            uprintf(6, L"Play Ended"); // 재생 종료 메시지 출력
            ResetParam(); // 파라미터 초기화
            CleanupTempFolder(); // 임시 파일 삭제
            system("pause"); // 일시 정지
            return 0; // 함수 종료
        }
    }
    return 1; // 함수 종료
}
