#define _CRT_SECURE_NO_WARNINGS // ���� ��� �����ϱ� ���� ��ũ�� ����
#include <windows.h> // Windows API ����� ���� ��� ���� ����
#include <stdio.h> // ǥ�� ����� �Լ� ����� ���� ��� ���� ����
#include <stdlib.h> // ǥ�� ���̺귯�� �Լ� ����� ���� ��� ���� ����
#include <shlobj.h> // Shell API ����� ���� ��� ���� ����
#include <shobjidl.h> // Shell Object API ����� ���� ��� ���� ����
#include <tchar.h> // �����ڵ� �� ��Ƽ����Ʈ ���� ���� ��ũ�� ����� ���� ��� ���� ����

#include "playerUI.h" // ����� ���� ��� ���� ����
#include "musicPlayer.h" // ����� ���� ��� ���� ����
#include "dynamicArray.h" // ����� ���� ��� ���� ����

int SetFileHidden(const wchar_t* path) { // ������ ���� �Ӽ����� �����ϴ� �Լ� ����
    if (!SetFileAttributesW(path, FILE_ATTRIBUTE_HIDDEN)) { // ���� �Ӽ� ���� ���� ��
        printf("Cannot set file hidden\n"); // ���� �޽��� ���
        return 1; // ���� �ڵ� ��ȯ
    }
    return 0; // ���� �ڵ� ��ȯ
}


/*
* �߰� ����
* MP3 ������ ���� ID3v2��� mp3�� ������ ��� �ִ� �±׸� ������ �ֽ��ϴ�
* MP3�� �ý�Ʈ �������� ���� �ٷ� �� �κп� ���� ID3�� ���� ������
* �� �ڿ��� ID3v2�� �±��� ������ ��� �ֽ��ϴ� ( �̹���, ��Ƽ��Ʈ ��� )
* �� �ڵ带 �ۼ��� ������ MCI�� ID3v2�� �±��� �̹����� ���� ���� ������ �浹�� ����Ű�� ��찡 �־�
* �ش� �±׸� ���� �� �ӽ������� �ӽ������� �����ϴ� �������� �ۼ��߽��ϴ�.
* (ID3v2 �±׸� �����ص� MP3�� ���������� ����˴ϴ�)
* #���� : https://clansim.tistory.com/99
*/
int RemoveTag(const wchar_t* originPath, wchar_t* tempPathOut, size_t stringSize) { // ID3 �±׸� �����ϴ� �Լ� ����
    wchar_t tempPath[MAX_PATH] = L""; // �ӽ� ���� ��θ� ������ �迭 �ʱ�ȭ
    wchar_t tempDir[MAX_PATH] = L""; // �ӽ� ���丮 ��θ� ������ �迭 �ʱ�ȭ
    FILE* inputFile = NULL, * outputFile = NULL; // ���� ������ �ʱ�ȭ
    int result = 0; // ��� ���� �ʱ�ȭ

    // %temp% ���丮 ��� ��������
    if (GetEnvironmentVariableW(L"TEMP", tempDir, MAX_PATH) == 0) { // ȯ�� �������� TEMP ���丮 ��θ� ������
        printf("Failed to get %%temp%% directory\n"); // ���� �� ���� �޽��� ���
        return 1; // ���� �ڵ� ��ȯ
    }

    // temp_mp3 ���� ��� ����
    wcscat_s(tempDir, MAX_PATH, L"\\temp_mp3"); // TEMP ���丮�� temp_mp3 ���� ��� �߰�

    // temp_mp3 ������ ������ ����
    if (_waccess(tempDir, 0) == -1) { // temp_mp3 ������ �������� ������
        if (_wmkdir(tempDir) == -1) { // temp_mp3 ���� ���� �õ�
            printf("Failed to create temp_mp3 directory\n"); // ���� �� ���� �޽��� ���
            return 1; // ���� �ڵ� ��ȯ
        }
    }

    // ���� ���ϸ� ����
    const wchar_t* fileName = wcsrchr(originPath, L'\\'); // ���� ��ο��� ���ϸ� ����
    if (!fileName) { // ���ϸ��� ������
        printf("Invalid file path\n"); // ���� �޽��� ���
        return 1; // ���� �ڵ� ��ȯ
    }
    fileName++; // '\\' ���� ���ڷ� �̵�

    // �ӽ� ���� ��ü ��� ����
    swprintf(tempPath, MAX_PATH, L"%s\\%s", tempDir, fileName); // �ӽ� ���� ��� ����
    wcscpy_s(tempPathOut, stringSize, tempPath); // �ӽ� ���� ��θ� ��� ������ ����

    inputFile = _wfopen(originPath, L"rb"); // ���� ������ �б� ���� ����
    if (!inputFile) { // ���� ���� ���� ��
        printf("Cannot open file: %ls\n", originPath); // ���� �޽��� ���
        return 1; // ���� �ڵ� ��ȯ
    }

    unsigned char header[10]; // ID3 ����� ������ �迭
    if (fread(header, 1, 10, inputFile) != 10) { // ��� �б� ���� ��
        printf("Failed to read header\n"); // ���� �޽��� ���
        fclose(inputFile); // ���� �ݱ�
        return 1; // ���� �ڵ� ��ȯ
    }

    if (strncmp((char*)header, "ID3", 3) == 0) { // ID3 �±װ� �����ϸ�
        unsigned int tagSize = (header[6] << 21) | (header[7] << 14) | (header[8] << 7) | header[9]; // �±� ũ�� ���

        // ���� ���� ���� ���� ������ �Ӽ��� ����
        SetFileAttributesW(tempPath, FILE_ATTRIBUTE_NORMAL); // �ӽ� ���� �Ӽ��� �Ϲ� ���Ϸ� ����

        outputFile = _wfopen(tempPath, L"wb"); // �ӽ� ������ ���� ���� ����
        if (!outputFile) { // ���� ���� ���� ��
            // ���� ���� ���� �� ��õ� ����
            int retryCount = 0; // ��õ� Ƚ�� �ʱ�ȭ
            const int MAX_RETRIES = 5; // �ִ� ��õ� Ƚ��

            while (retryCount < MAX_RETRIES && !outputFile) { // �ִ� ��õ� Ƚ������ �ݺ�
                // ���� ������ �ִٸ� ���� �õ�
                if (_wremove(tempPath) == 0) { // �ӽ� ���� ���� �õ�
                    Sleep(100);  // ���� �ý��ۿ� �ð� ������ ��
                    outputFile = _wfopen(tempPath, L"wb"); // �ӽ� ������ �ٽ� ���� ���� ���� �õ�
                }
                retryCount++; // ��õ� Ƚ�� ����
                if (retryCount < MAX_RETRIES && !outputFile) { // ��õ� Ƚ���� �����ְ� ���� ���� ���� ��
                    Sleep(200);  // ���� �õ� �� ���
                }
            }

            if (!outputFile) { // ���� ���� ���� ���� ��
                printf("Cannot create output file after %d attempts: %ls\n", MAX_RETRIES, tempPath); // ���� �޽��� ���
                fclose(inputFile); // ���� ���� �ݱ�
                return 1; // ���� �ڵ� ��ȯ
            }
        }

        // ���� ���� �۾�
        fseek(inputFile, 10 + tagSize, SEEK_SET); // ���� ���Ͽ��� �±� ���ķ� �̵�
        unsigned char buffer[4096];  // ���� ũ�� ����
        size_t bytesRead; // ���� ����Ʈ �� ���� ����

        while ((bytesRead = fread(buffer, 1, sizeof(buffer), inputFile)) > 0) { // ���� ���Ͽ��� �б�
            if (fwrite(buffer, 1, bytesRead, outputFile) != bytesRead) { // �ӽ� ���Ͽ� ���� ���� ��
                printf("Write error occurred\n"); // ���� �޽��� ���
                result = 1; // ���� �ڵ� ����
                break; // �ݺ��� ����
            }
        }

        // ���� �ڵ� ����
        if (inputFile) fclose(inputFile); // ���� ���� �ݱ�
        if (outputFile) fclose(outputFile); // �ӽ� ���� �ݱ�

        // ���� �߻� �� �ӽ� ���� ����
        if (result != 0) { // ���� �߻� ��
            SetFileAttributesW(tempPath, FILE_ATTRIBUTE_NORMAL); // �ӽ� ���� �Ӽ��� �Ϲ� ���Ϸ� ����
            int deleteRetry = 0; // ���� ��õ� Ƚ�� �ʱ�ȭ
            while (deleteRetry < 5) { // �ִ� 5ȸ ��õ�
                if (_wremove(tempPath) == 0) break; // �ӽ� ���� ���� ���� �� �ݺ��� ����
                Sleep(100); // ���� �õ� �� ���
                deleteRetry++; // ��õ� Ƚ�� ����
            }
        }
    }
    else { // ID3 �±װ� ������
        printf("No ID3 tag found in the file\n"); // �޽��� ���
        if (inputFile) fclose(inputFile); // ���� ���� �ݱ�
        result = 1; // ���� �ڵ� ����
    }

    return result; // ��� ��ȯ
}

// ���α׷� ���� �� ȣ���� ���� �Լ�
void CleanupTempFolder() {
    wchar_t tempDir[MAX_PATH] = L""; // �ӽ� ���丮 ��θ� ������ �迭 �ʱ�ȭ
    WIN32_FIND_DATAW findData; // ���� ã�� ������ ����ü ����
    HANDLE hFind = INVALID_HANDLE_VALUE; // ���� ã�� �ڵ� ���� �� �ʱ�ȭ

    // %temp% ���丮 ��� ��������
    if (GetEnvironmentVariableW(L"TEMP", tempDir, MAX_PATH) == 0) { // ȯ�� �������� TEMP ���丮 ��θ� ������
        return; // ���� �� �Լ� ����
    }

    // temp_mp3 ���� ��� ����
    wcscat_s(tempDir, MAX_PATH, L"\\temp_mp3\\*"); // TEMP ���丮�� temp_mp3 ���� ��� �߰�

    // ���� �� ��� ���� ����
    hFind = FindFirstFileW(tempDir, &findData); // temp_mp3 ���� �� ù ��° ���� ã��
    if (hFind != INVALID_HANDLE_VALUE) { // ���� ã�� ���� ��
        do {
            if (wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0) { // ���� ���� �Ǵ� ���� ������ �ƴϸ�
                wchar_t filePath[MAX_PATH]; // ���� ��� �迭 ����
                wcscpy_s(filePath, MAX_PATH, tempDir); // tempDir ��θ� filePath�� ����
                *wcsrchr(filePath, L'\\') = L'\0';  // '*' ����
                wcscat_s(filePath, MAX_PATH, L"\\"); // ��ο� '\' �߰�
                wcscat_s(filePath, MAX_PATH, findData.cFileName); // ���ϸ��� ��ο� �߰�

                SetFileAttributesW(filePath, FILE_ATTRIBUTE_NORMAL); // ���� �Ӽ��� �Ϲ� ���Ϸ� ����
                _wremove(filePath); // ���� ����
            }
        } while (FindNextFileW(hFind, &findData) != 0); // ���� ���� ã��
        FindClose(hFind); // �ڵ� �ݱ�
    }

    // temp_mp3 ���� ���� �õ�
    *wcsrchr(tempDir, L'\\') = L'\0';  // '*' ����
    _wrmdir(tempDir); // temp_mp3 ���� ����
}

void SaveTemporaryMP3(const wchar_t* path, wchar_t* tempPathOut, size_t stringSize) { // �ӽ� MP3 ���� ���� �Լ� ����
    if(RemoveTag(path, tempPathOut, stringSize) != 0) exit(1); // �±� ���� ���� �� ���α׷� ����
    SetFileHidden(tempPathOut); // ���� ���� �Ӽ� ����
}

/*
* GetOpenFileName(W) �� ���� GUI ȯ�濡 ������ ������� �����Ǽ�
* hwndOwnder(�ڵ� ������) �� ������ �ִ°̴ϴ�
* ������� ������ �����߽��ϴ� : https://learn.microsoft.com/ko-kr/dotnet/api/system.windows.forms.openfiledialog?view=windowsdesktop-9.0
*/
void OpenFileDialog(wchar_t* filePath) { // ���� ���� ��ȭ ���� �Լ� ����
    OPENFILENAMEW ofn; // ���� ���� ��ȭ ���� ����ü ����
    ZeroMemory(&ofn, sizeof(ofn)); // ����ü �ʱ�ȭ

    ofn.hwndOwner = NULL; // ������ ������ �ڵ� ����
    ofn.lStructSize = sizeof(ofn); // ����ü ũ�� ����
    ofn.lpstrFile = filePath; // ���� ��� ����
    ofn.nMaxFile = MAX_PATH; // ���� ��� �ִ� ���� ����
    ofn.lpstrFilter = L"MP3 Files\0*.mp3\0"; // ���� ���� ����
    ofn.nFileOffset = 1; // ���� ������ ����
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; // ��ȭ ���� �ɼ� ����

    if (!GetOpenFileNameW(&ofn)) { // ���� ���� ��ȭ ���� ǥ��
        clearLine(6); uprintf(6, L"Canceled\n"); // ��� �� �޽��� ���
    }
}

/*
* ó������ ���ͳݿ��� ã�ƺ� ��� shell.dll? �� ����ؼ� ������ �� api�� ����Ͽ�
* ���� ���� â�� ���� �ڵ尡 �־�����, ������������ �����Ƽ�
* �������� ActiveX?�� ���� ��ü ���� �ý����� COM ��ü�� ����߽��ϴ�
* ���� �������� ���� �ý��� ����? CLSID�� ���� ���� ���̵� �����Ҷ� �ַ� ����ϴµ� �մϴ�
* ���� ���� ������ ���� ���̵� �����Ͽ� â�� ���� ���� �������� �ڵ带 �ۼ��߽��ϴ�.
* https://learn.microsoft.com/ko-kr/windows/win32/com/component-object-model--com--portal
* https://learn.microsoft.com/ko-kr/windows/win32/shell/common-file-dialog
* 
*/
void OpenFolderDialog(wchar_t* path) { // ���� ���� ��ȭ ���� �Լ� ����
    if (FAILED(CoInitialize(NULL))) { // COM �ʱ�ȭ ���� ��
        printf("Failed to initialize COM library.\n"); // ���� �޽��� ���
        return; // �Լ� ����
    }

    IFileDialog* pFileDialog = NULL; // ���� ��ȭ ���� �������̽� ������ ����

    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_IFileDialog, (void**)&pFileDialog); // ���� ��ȭ ���� �ν��Ͻ� ����
    if (SUCCEEDED(hr)) { // �ν��Ͻ� ���� ���� ��
        DWORD options = 0; // �ɼ� ���� ����
        pFileDialog->GetOptions(&options); // �ɼ� ��������
        pFileDialog->SetOptions(options | FOS_PICKFOLDERS); // ���� ���� �ɼ� ����

        hr = pFileDialog->Show(NULL); // ��ȭ ���� ǥ��
        if (SUCCEEDED(hr)) { // ��ȭ ���� ǥ�� ���� ��
            IShellItem* pItem = NULL; // �� ������ �������̽� ������ ����

            hr = pFileDialog->GetResult(&pItem); // ���õ� ������ ��������
            if (SUCCEEDED(hr)) { // ������ �������� ���� ��
                PWSTR folderPath = NULL; // ���� ��� ������ ����
                wchar_t returnPath[MAX_PATH]; // ��ȯ ��� �迭 ����

                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &folderPath); // ���õ� ���� ��� ��������
                if (SUCCEEDED(hr)) { // ��� �������� ���� ��
                    wcscpy_s(path, MAX_PATH, folderPath); // ��� ����
                    wprintf(L"Selected Folder: %s\n", folderPath); // ���õ� ���� ��� ���
                    CoTaskMemFree(folderPath); // ��� �޸� ����
                }

                pItem->Release(); // �� ������ ����
            }
        }
        else {
            printf("Folder selection canceled or failed.\n"); // ���� ���� ��� �Ǵ� ���� �� �޽��� ���
        }

        pFileDialog->Release(); // ���� ��ȭ ���� ����
    }
    else {
        printf("Failed to create IFileDialog instance.\n"); // �ν��Ͻ� ���� ���� �� �޽��� ���
    }

    CoUninitialize(); // COM ����
}



void ListMP3FilesInFolder(const wchar_t* folderPath, StringArray* array) { // ���� �� MP3 ���� ��� �Լ� ����
    WIN32_FIND_DATAW findData; // ���� ã�� ������ ����ü ����
    HANDLE hFind = INVALID_HANDLE_VALUE; // ���� ã�� �ڵ� ���� �� �ʱ�ȭ

    wchar_t searchPath[MAX_PATH]; // �˻� ��� �迭 ����
    swprintf(searchPath, MAX_PATH, L"%s\\*", folderPath); // ��� ���� �˻� ��� ����

    hFind = FindFirstFileW(searchPath, &findData); // ù ��° ���� ã��

    if (hFind == INVALID_HANDLE_VALUE) { // ���� ã�� ���� ��
        wprintf(L"No files or folders found in folder: %s\n", folderPath); // �޽��� ���
        return; // �Լ� ����
    }

    do {
        if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0) // ���� ���� �Ǵ� ���� ������ ���
            continue; // ���� ���Ϸ� �Ѿ��

        wchar_t filePath[MAX_PATH]; // ���� ��� �迭 ����
        swprintf(filePath, MAX_PATH, L"%s\\%s", folderPath, findData.cFileName); // ���� ��� ����

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // ���丮�� ���
            ListMP3FilesInFolder(filePath, array); // ��� ȣ���Ͽ� ���� ���� �˻�
        else {
            const wchar_t* extension = wcsrchr(findData.cFileName, L'.'); // ���� Ȯ���� ����
            if (extension && _wcsicmp(extension, L".mp3") == 0) // MP3 ������ ���
                addStringArrayValue(array, filePath); // �迭�� ���� ��� �߰�
        }
    } while (FindNextFileW(hFind, &findData) != 0); // ���� ���� ã��
    FindClose(hFind); // �ڵ� �ݱ�
}
