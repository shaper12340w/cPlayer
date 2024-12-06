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

int SetFileHidden(const wchar_t* path) { // 파일을 숨김 속성으로 설정하는 함수 정의
    if (!SetFileAttributesW(path, FILE_ATTRIBUTE_HIDDEN)) { // 파일 속성 설정 실패 시
        printf("Cannot set file hidden\n"); // 오류 메시지 출력
        return 1; // 오류 코드 반환
    }
    return 0; // 성공 코드 반환
}


/*
* 추가 설명
* MP3 파일은 보통 ID3v2라는 mp3의 정보를 담고 있는 태그를 가지고 있습니다
* MP3를 택스트 형식으로 열면 바로 앞 부분에 보통 ID3라 적혀 있으며
* 그 뒤에는 ID3v2의 태그의 정보가 담겨 있습니다 ( 이미지, 아티스트 등등 )
* 이 코드를 작성한 이유는 MCI가 ID3v2의 태그의 이미지와 같은 일정 정보가 충돌을 일으키는 경우가 있어
* 해당 태그를 제거 후 임시폴더에 임시파일을 저장하는 형식으로 작성했습니다.
* (ID3v2 태그를 제거해도 MP3는 정상적으로 재생됩니다)
* #참고 : https://clansim.tistory.com/99
*/
int RemoveTag(const wchar_t* originPath, wchar_t* tempPathOut, size_t stringSize) { // ID3 태그를 제거하는 함수 정의
    wchar_t tempPath[MAX_PATH] = L""; // 임시 파일 경로를 저장할 배열 초기화
    wchar_t tempDir[MAX_PATH] = L""; // 임시 디렉토리 경로를 저장할 배열 초기화
    FILE* inputFile = NULL, * outputFile = NULL; // 파일 포인터 초기화
    int result = 0; // 결과 변수 초기화

    // %temp% 디렉토리 경로 가져오기
    if (GetEnvironmentVariableW(L"TEMP", tempDir, MAX_PATH) == 0) { // 환경 변수에서 TEMP 디렉토리 경로를 가져옴
        printf("Failed to get %%temp%% directory\n"); // 실패 시 오류 메시지 출력
        return 1; // 오류 코드 반환
    }

    // temp_mp3 폴더 경로 생성
    wcscat_s(tempDir, MAX_PATH, L"\\temp_mp3"); // TEMP 디렉토리에 temp_mp3 폴더 경로 추가

    // temp_mp3 폴더가 없으면 생성
    if (_waccess(tempDir, 0) == -1) { // temp_mp3 폴더가 존재하지 않으면
        if (_wmkdir(tempDir) == -1) { // temp_mp3 폴더 생성 시도
            printf("Failed to create temp_mp3 directory\n"); // 실패 시 오류 메시지 출력
            return 1; // 오류 코드 반환
        }
    }

    // 원본 파일명 추출
    const wchar_t* fileName = wcsrchr(originPath, L'\\'); // 원본 경로에서 파일명 추출
    if (!fileName) { // 파일명이 없으면
        printf("Invalid file path\n"); // 오류 메시지 출력
        return 1; // 오류 코드 반환
    }
    fileName++; // '\\' 다음 문자로 이동

    // 임시 파일 전체 경로 생성
    swprintf(tempPath, MAX_PATH, L"%s\\%s", tempDir, fileName); // 임시 파일 경로 생성
    wcscpy_s(tempPathOut, stringSize, tempPath); // 임시 파일 경로를 출력 변수에 복사

    inputFile = _wfopen(originPath, L"rb"); // 원본 파일을 읽기 모드로 열기
    if (!inputFile) { // 파일 열기 실패 시
        printf("Cannot open file: %ls\n", originPath); // 오류 메시지 출력
        return 1; // 오류 코드 반환
    }

    unsigned char header[10]; // ID3 헤더를 저장할 배열
    if (fread(header, 1, 10, inputFile) != 10) { // 헤더 읽기 실패 시
        printf("Failed to read header\n"); // 오류 메시지 출력
        fclose(inputFile); // 파일 닫기
        return 1; // 오류 코드 반환
    }

    if (strncmp((char*)header, "ID3", 3) == 0) { // ID3 태그가 존재하면
        unsigned int tagSize = (header[6] << 21) | (header[7] << 14) | (header[8] << 7) | header[9]; // 태그 크기 계산

        // 파일 생성 전에 기존 파일의 속성을 변경
        SetFileAttributesW(tempPath, FILE_ATTRIBUTE_NORMAL); // 임시 파일 속성을 일반 파일로 설정

        outputFile = _wfopen(tempPath, L"wb"); // 임시 파일을 쓰기 모드로 열기
        if (!outputFile) { // 파일 열기 실패 시
            // 파일 생성 실패 시 재시도 로직
            int retryCount = 0; // 재시도 횟수 초기화
            const int MAX_RETRIES = 5; // 최대 재시도 횟수

            while (retryCount < MAX_RETRIES && !outputFile) { // 최대 재시도 횟수까지 반복
                // 이전 파일이 있다면 삭제 시도
                if (_wremove(tempPath) == 0) { // 임시 파일 삭제 시도
                    Sleep(100);  // 파일 시스템에 시간 여유를 줌
                    outputFile = _wfopen(tempPath, L"wb"); // 임시 파일을 다시 쓰기 모드로 열기 시도
                }
                retryCount++; // 재시도 횟수 증가
                if (retryCount < MAX_RETRIES && !outputFile) { // 재시도 횟수가 남아있고 파일 열기 실패 시
                    Sleep(200);  // 다음 시도 전 대기
                }
            }

            if (!outputFile) { // 파일 열기 최종 실패 시
                printf("Cannot create output file after %d attempts: %ls\n", MAX_RETRIES, tempPath); // 오류 메시지 출력
                fclose(inputFile); // 원본 파일 닫기
                return 1; // 오류 코드 반환
            }
        }

        // 파일 복사 작업
        fseek(inputFile, 10 + tagSize, SEEK_SET); // 원본 파일에서 태그 이후로 이동
        unsigned char buffer[4096];  // 버퍼 크기 증가
        size_t bytesRead; // 읽은 바이트 수 저장 변수

        while ((bytesRead = fread(buffer, 1, sizeof(buffer), inputFile)) > 0) { // 원본 파일에서 읽기
            if (fwrite(buffer, 1, bytesRead, outputFile) != bytesRead) { // 임시 파일에 쓰기 실패 시
                printf("Write error occurred\n"); // 오류 메시지 출력
                result = 1; // 오류 코드 설정
                break; // 반복문 종료
            }
        }

        // 파일 핸들 정리
        if (inputFile) fclose(inputFile); // 원본 파일 닫기
        if (outputFile) fclose(outputFile); // 임시 파일 닫기

        // 에러 발생 시 임시 파일 삭제
        if (result != 0) { // 오류 발생 시
            SetFileAttributesW(tempPath, FILE_ATTRIBUTE_NORMAL); // 임시 파일 속성을 일반 파일로 설정
            int deleteRetry = 0; // 삭제 재시도 횟수 초기화
            while (deleteRetry < 5) { // 최대 5회 재시도
                if (_wremove(tempPath) == 0) break; // 임시 파일 삭제 성공 시 반복문 종료
                Sleep(100); // 다음 시도 전 대기
                deleteRetry++; // 재시도 횟수 증가
            }
        }
    }
    else { // ID3 태그가 없으면
        printf("No ID3 tag found in the file\n"); // 메시지 출력
        if (inputFile) fclose(inputFile); // 원본 파일 닫기
        result = 1; // 오류 코드 설정
    }

    return result; // 결과 반환
}

// 프로그램 종료 시 호출할 정리 함수
void CleanupTempFolder() {
    wchar_t tempDir[MAX_PATH] = L""; // 임시 디렉토리 경로를 저장할 배열 초기화
    WIN32_FIND_DATAW findData; // 파일 찾기 데이터 구조체 선언
    HANDLE hFind = INVALID_HANDLE_VALUE; // 파일 찾기 핸들 선언 및 초기화

    // %temp% 디렉토리 경로 가져오기
    if (GetEnvironmentVariableW(L"TEMP", tempDir, MAX_PATH) == 0) { // 환경 변수에서 TEMP 디렉토리 경로를 가져옴
        return; // 실패 시 함수 종료
    }

    // temp_mp3 폴더 경로 생성
    wcscat_s(tempDir, MAX_PATH, L"\\temp_mp3\\*"); // TEMP 디렉토리에 temp_mp3 폴더 경로 추가

    // 폴더 내 모든 파일 삭제
    hFind = FindFirstFileW(tempDir, &findData); // temp_mp3 폴더 내 첫 번째 파일 찾기
    if (hFind != INVALID_HANDLE_VALUE) { // 파일 찾기 성공 시
        do {
            if (wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0) { // 현재 폴더 또는 상위 폴더가 아니면
                wchar_t filePath[MAX_PATH]; // 파일 경로 배열 선언
                wcscpy_s(filePath, MAX_PATH, tempDir); // tempDir 경로를 filePath에 복사
                *wcsrchr(filePath, L'\\') = L'\0';  // '*' 제거
                wcscat_s(filePath, MAX_PATH, L"\\"); // 경로에 '\' 추가
                wcscat_s(filePath, MAX_PATH, findData.cFileName); // 파일명을 경로에 추가

                SetFileAttributesW(filePath, FILE_ATTRIBUTE_NORMAL); // 파일 속성을 일반 파일로 설정
                _wremove(filePath); // 파일 삭제
            }
        } while (FindNextFileW(hFind, &findData) != 0); // 다음 파일 찾기
        FindClose(hFind); // 핸들 닫기
    }

    // temp_mp3 폴더 삭제 시도
    *wcsrchr(tempDir, L'\\') = L'\0';  // '*' 제거
    _wrmdir(tempDir); // temp_mp3 폴더 삭제
}

void SaveTemporaryMP3(const wchar_t* path, wchar_t* tempPathOut, size_t stringSize) { // 임시 MP3 파일 저장 함수 정의
    if(RemoveTag(path, tempPathOut, stringSize) != 0) exit(1); // 태그 제거 실패 시 프로그램 종료
    SetFileHidden(tempPathOut); // 파일 숨김 속성 설정
}

/*
* GetOpenFileName(W) 는 기존 GUI 환경에 적합한 명령으로 추정되서
* hwndOwnder(핸들 소유자) 를 가지고 있는겁니다
* 사용방법은 다음을 참고했습니다 : https://learn.microsoft.com/ko-kr/dotnet/api/system.windows.forms.openfiledialog?view=windowsdesktop-9.0
*/
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

/*
* 처음에는 인터넷에서 찾아본 결과 shell.dll? 을 사용해서 윈도우 쉘 api를 사용하여
* 폴더 열기 창을 띄우는 코드가 있었지만, 디자인적으로 안좋아서
* 윈도우의 ActiveX?와 같은 개체 지향 시스템인 COM 객체를 사용했습니다
* 보통 윈도우의 고유 시스템 서버? CLSID와 같은 고유 아이디에 접근할때 주로 사용하는듯 합니다
* 이중 파일 열기의 고유 아이디에 접근하여 창을 띄우고 값을 가져오는 코드를 작성했습니다.
* https://learn.microsoft.com/ko-kr/windows/win32/com/component-object-model--com--portal
* https://learn.microsoft.com/ko-kr/windows/win32/shell/common-file-dialog
* 
*/
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
