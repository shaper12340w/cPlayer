#define _CRT_SECURE_NO_WARNINGS // 보안 경고를 무시하기 위한 매크로 정의
#include <windows.h> // Windows API 사용을 위한 헤더 파일 포함
#include <stdio.h> // 표준 입출력 함수 사용을 위한 헤더 파일 포함
#include <stdlib.h> // 표준 라이브러리 함수 사용을 위한 헤더 파일 포함
#include <shlobj.h> // Shell API 사용을 위한 헤더 파일 포함
#include <shobjidl.h> // Shell Object API 사용을 위한 헤더 파일 포함
#include <tchar.h> // 유니코드 및 멀티바이트 문자 집합 매크로 사용을 위한 헤더 파일 포함

#include "playerUI.h" // 사용자 정의 헤더 파일 포함
#include "musicPlayer.h" // 사용자 정의 헤더 파일 포함
#include "dynamicArray.h" // 사용자 정의 헤더 파일 포함

int stack = 0; // 전역 변수 stack 초기화

void DeleteTempFile() { // 임시 파일 삭제 함수 정의
    char temp[260]; // 임시 파일 경로를 저장할 배열 선언
    for (int i = 0; i < getIntArrayLength(&player.deletedArray); i++) { // 삭제할 파일 배열의 길이만큼 반복
        wcstombs(temp, getStringArrayValue(&player.pathArray, getIntArrayValue(&player.deletedArray, i)), sizeof(temp)); // 경로를 멀티바이트 문자열로 변환
        remove(temp); // 파일 삭제
    }
}

int SetFileHidden(const wchar_t* path) { // 파일을 숨김 속성으로 설정하는 함수 정의
    if (!SetFileAttributesW(path, FILE_ATTRIBUTE_HIDDEN)) { // 파일 속성 설정 실패 시
        printf("Cannot set file hidden\n"); // 오류 메시지 출력
        return 1; // 오류 코드 반환
    }
    return 0; // 성공 코드 반환
}

int RemoveTag(const wchar_t* originPath, wchar_t* tempPathOut, size_t stringSize) { // ID3 태그를 제거하는 함수 정의
    wchar_t tempPath[MAX_PATH] = L""; // 임시 파일 경로를 저장할 배열 선언 및 초기화
    wchar_t appPath[MAX_PATH]; // 애플리케이션 경로를 저장할 배열 선언
    const wchar_t* fileName = wcsrchr(originPath, L'\\'); // 원본 경로에서 파일 이름 추출
    if (!fileName) { // 파일 이름이 없을 경우
        printf("Invalid file path\n"); // 오류 메시지 출력
        return 1; // 오류 코드 반환
    }
    GetModuleFileNameW(NULL, appPath, MAX_PATH); // 애플리케이션 경로 가져오기
    *wcsrchr(appPath, L'\\') = L'\0'; // 경로에서 마지막 '\\' 뒤를 잘라내기

    wcscat(tempPath, appPath); // 임시 경로에 애플리케이션 경로 추가
    wcscat(tempPath, fileName); // 임시 경로에 파일 이름 추가
    wcscpy_s(tempPathOut, stringSize, tempPath); // 임시 경로를 출력 경로에 복사

    FILE* inputFile = _wfopen(originPath, L"rb"); // 원본 파일 열기
    if (!inputFile) { // 파일 열기 실패 시
        printf("Cannot open file: %ls\n", originPath); // 오류 메시지 출력
        return 1; // 오류 코드 반환
    }

    unsigned char header[10]; // 파일 헤더를 저장할 배열 선언
    fread(header, 1, 10, inputFile); // 파일 헤더 읽기

    if (strncmp((char*)header, "ID3", 3) == 0) { // ID3v2 태그 확인
        unsigned int tagSize = (header[6] << 21) | (header[7] << 14) | (header[8] << 7) | header[9]; // 태그 크기 계산
        FILE* outputFile = _wfopen(tempPath, L"wb"); // 출력 파일 열기
        if (!outputFile) { // 파일 열기 실패 시
            printf("Cannot create output file: %ls\n", tempPath); // 오류 메시지 출력
            if (stack < 5) { // 재시도 횟수가 5회 미만일 경우
                char temp[260]; // 임시 파일 경로를 저장할 배열 선언
                wcstombs(temp, tempPath, MAX_PATH); // 경로를 멀티바이트 문자열로 변환
                if (remove(temp)) // 파일 삭제
                    exit(1); // 프로그램 종료
                else
                    RemoveTag(originPath, tempPathOut, stringSize); // 태그 제거 재시도
                fclose(inputFile); // 입력 파일 닫기
                stack++; // 재시도 횟수 증가
                return 0; // 성공 코드 반환
            }
            else {
                fclose(inputFile); // 입력 파일 닫기
                return 1; // 오류 코드 반환
            }
        }

        fseek(inputFile, 10 + tagSize, SEEK_SET); // 파일 포인터를 태그 이후로 이동
        unsigned char buffer[1024]; // 버퍼 선언
        size_t bytesRead; // 읽은 바이트 수 저장할 변수 선언
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), inputFile)) > 0) { // 파일 읽기
            fwrite(buffer, 1, bytesRead, outputFile); // 출력 파일에 쓰기
        }

        fclose(inputFile); // 입력 파일 닫기
        fclose(outputFile); // 출력 파일 닫기
    }
    else {
        printf("No ID3 tag found in the file\n"); // 태그가 없을 경우 메시지 출력
        fclose(inputFile); // 입력 파일 닫기
        return 1; // 오류 코드 반환
    }
    return 0; // 성공 코드 반환
}

void SaveTemporaryMP3(const wchar_t* path, wchar_t* tempPathOut, size_t stringSize) { // 임시 MP3 파일 저장 함수 정의
    if(RemoveTag(path, tempPathOut, stringSize) != 0) exit(1); // 태그 제거 실패 시 프로그램 종료
    SetFileHidden(tempPathOut); // 파일 숨김 속성 설정
}

void OpenFileDialog(wchar_t* filePath) { // 파일 열기 대화 상자 함수 정의
    OPENFILENAMEW ofn; // 파일 열기 대화 상자 구조체 선언
    ZeroMemory(&ofn, sizeof(ofn)); // 구조체 초기화

    ofn.hwndOwner = NULL; // 소유자 윈도우 핸들 설정
    ofn.lStructSize = sizeof(ofn); // 구조체 크기 설정
    ofn.lpstrFile = filePath; // 파일 경로 설정
    ofn.nMaxFile = MAX_PATH; // 파일 경로 최대 길이 설정
    ofn.lpstrFilter = L"MP3 Files\0*.mp3\0"; // 파일 필터 설정
    ofn.nFileOffset = 1; // 파일 오프셋 설정
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; // 대화 상자 옵션 설정

    if (!GetOpenFileNameW(&ofn)) { // 파일 열기 대화 상자 표시
        clearLine(6); uprintf(6, L"Canceled\n"); // 취소 시 메시지 출력
    }
}

void OpenFolderDialog(wchar_t* path) { // 폴더 열기 대화 상자 함수 정의
    if (FAILED(CoInitialize(NULL))) { // COM 초기화 실패 시
        printf("Failed to initialize COM library.\n"); // 오류 메시지 출력
        return; // 함수 종료
    }

    IFileDialog* pFileDialog = NULL; // 파일 대화 상자 인터페이스 포인터 선언

    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_IFileDialog, (void**)&pFileDialog); // 파일 대화 상자 인스턴스 생성
    if (SUCCEEDED(hr)) { // 인스턴스 생성 성공 시
        DWORD options = 0; // 옵션 변수 선언
        pFileDialog->GetOptions(&options); // 옵션 가져오기
        pFileDialog->SetOptions(options | FOS_PICKFOLDERS); // 폴더 선택 옵션 설정

        hr = pFileDialog->Show(NULL); // 대화 상자 표시
        if (SUCCEEDED(hr)) { // 대화 상자 표시 성공 시
            IShellItem* pItem = NULL; // 셸 아이템 인터페이스 포인터 선언

            hr = pFileDialog->GetResult(&pItem); // 선택된 아이템 가져오기
            if (SUCCEEDED(hr)) { // 아이템 가져오기 성공 시
                PWSTR folderPath = NULL; // 폴더 경로 포인터 선언
                wchar_t returnPath[MAX_PATH]; // 반환 경로 배열 선언

                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &folderPath); // 선택된 폴더 경로 가져오기
                if (SUCCEEDED(hr)) { // 경로 가져오기 성공 시
                    wcscpy_s(path, MAX_PATH, folderPath); // 경로 복사
                    wprintf(L"Selected Folder: %s\n", folderPath); // 선택된 폴더 경로 출력
                    CoTaskMemFree(folderPath); // 경로 메모리 해제
                }

                pItem->Release(); // 셸 아이템 해제
            }
        }
        else {
            printf("Folder selection canceled or failed.\n"); // 폴더 선택 취소 또는 실패 시 메시지 출력
        }

        pFileDialog->Release(); // 파일 대화 상자 해제
    }
    else {
        printf("Failed to create IFileDialog instance.\n"); // 인스턴스 생성 실패 시 메시지 출력
    }

    CoUninitialize(); // COM 해제
}

void ListMP3FilesInFolder(const wchar_t* folderPath, StringArray* array) { // 폴더 내 MP3 파일 목록 함수 정의
    WIN32_FIND_DATAW findData; // 파일 찾기 데이터 구조체 선언
    HANDLE hFind = INVALID_HANDLE_VALUE; // 파일 찾기 핸들 선언 및 초기화

    wchar_t searchPath[MAX_PATH]; // 검색 경로 배열 선언
    swprintf(searchPath, MAX_PATH, L"%s\\*", folderPath); // 모든 파일 검색 경로 설정

    hFind = FindFirstFileW(searchPath, &findData); // 첫 번째 파일 찾기

    if (hFind == INVALID_HANDLE_VALUE) { // 파일 찾기 실패 시
        wprintf(L"No files or folders found in folder: %s\n", folderPath); // 메시지 출력
        return; // 함수 종료
    }

    do {
        if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0) // 현재 폴더 또는 상위 폴더일 경우
            continue; // 다음 파일로 넘어가기

        wchar_t filePath[MAX_PATH]; // 파일 경로 배열 선언
        swprintf(filePath, MAX_PATH, L"%s\\%s", folderPath, findData.cFileName); // 파일 경로 설정

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // 디렉토리일 경우
            ListMP3FilesInFolder(filePath, array); // 재귀 호출하여 하위 폴더 검색
        else {
            const wchar_t* extension = wcsrchr(findData.cFileName, L'.'); // 파일 확장자 추출
            if (extension && _wcsicmp(extension, L".mp3") == 0) // MP3 파일일 경우
                addStringArrayValue(array, filePath); // 배열에 파일 경로 추가
        }
    } while (FindNextFileW(hFind, &findData) != 0); // 다음 파일 찾기
    FindClose(hFind); // 핸들 닫기
}

