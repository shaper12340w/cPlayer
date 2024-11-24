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

int stack = 0; // ���� ���� stack �ʱ�ȭ

void DeleteTempFile() { // �ӽ� ���� ���� �Լ� ����
    char temp[260]; // �ӽ� ���� ��θ� ������ �迭 ����
    for (int i = 0; i < getIntArrayLength(&player.deletedArray); i++) { // ������ ���� �迭�� ���̸�ŭ �ݺ�
        wcstombs(temp, getStringArrayValue(&player.pathArray, getIntArrayValue(&player.deletedArray, i)), sizeof(temp)); // ��θ� ��Ƽ����Ʈ ���ڿ��� ��ȯ
        remove(temp); // ���� ����
    }
}

int SetFileHidden(const wchar_t* path) { // ������ ���� �Ӽ����� �����ϴ� �Լ� ����
    if (!SetFileAttributesW(path, FILE_ATTRIBUTE_HIDDEN)) { // ���� �Ӽ� ���� ���� ��
        printf("Cannot set file hidden\n"); // ���� �޽��� ���
        return 1; // ���� �ڵ� ��ȯ
    }
    return 0; // ���� �ڵ� ��ȯ
}

int RemoveTag(const wchar_t* originPath, wchar_t* tempPathOut, size_t stringSize) { // ID3 �±׸� �����ϴ� �Լ� ����
    wchar_t tempPath[MAX_PATH] = L""; // �ӽ� ���� ��θ� ������ �迭 ���� �� �ʱ�ȭ
    wchar_t appPath[MAX_PATH]; // ���ø����̼� ��θ� ������ �迭 ����
    const wchar_t* fileName = wcsrchr(originPath, L'\\'); // ���� ��ο��� ���� �̸� ����
    if (!fileName) { // ���� �̸��� ���� ���
        printf("Invalid file path\n"); // ���� �޽��� ���
        return 1; // ���� �ڵ� ��ȯ
    }
    GetModuleFileNameW(NULL, appPath, MAX_PATH); // ���ø����̼� ��� ��������
    *wcsrchr(appPath, L'\\') = L'\0'; // ��ο��� ������ '\\' �ڸ� �߶󳻱�

    wcscat(tempPath, appPath); // �ӽ� ��ο� ���ø����̼� ��� �߰�
    wcscat(tempPath, fileName); // �ӽ� ��ο� ���� �̸� �߰�
    wcscpy_s(tempPathOut, stringSize, tempPath); // �ӽ� ��θ� ��� ��ο� ����

    FILE* inputFile = _wfopen(originPath, L"rb"); // ���� ���� ����
    if (!inputFile) { // ���� ���� ���� ��
        printf("Cannot open file: %ls\n", originPath); // ���� �޽��� ���
        return 1; // ���� �ڵ� ��ȯ
    }

    unsigned char header[10]; // ���� ����� ������ �迭 ����
    fread(header, 1, 10, inputFile); // ���� ��� �б�

    if (strncmp((char*)header, "ID3", 3) == 0) { // ID3v2 �±� Ȯ��
        unsigned int tagSize = (header[6] << 21) | (header[7] << 14) | (header[8] << 7) | header[9]; // �±� ũ�� ���
        FILE* outputFile = _wfopen(tempPath, L"wb"); // ��� ���� ����
        if (!outputFile) { // ���� ���� ���� ��
            printf("Cannot create output file: %ls\n", tempPath); // ���� �޽��� ���
            if (stack < 5) { // ��õ� Ƚ���� 5ȸ �̸��� ���
                char temp[260]; // �ӽ� ���� ��θ� ������ �迭 ����
                wcstombs(temp, tempPath, MAX_PATH); // ��θ� ��Ƽ����Ʈ ���ڿ��� ��ȯ
                if (remove(temp)) // ���� ����
                    exit(1); // ���α׷� ����
                else
                    RemoveTag(originPath, tempPathOut, stringSize); // �±� ���� ��õ�
                fclose(inputFile); // �Է� ���� �ݱ�
                stack++; // ��õ� Ƚ�� ����
                return 0; // ���� �ڵ� ��ȯ
            }
            else {
                fclose(inputFile); // �Է� ���� �ݱ�
                return 1; // ���� �ڵ� ��ȯ
            }
        }

        fseek(inputFile, 10 + tagSize, SEEK_SET); // ���� �����͸� �±� ���ķ� �̵�
        unsigned char buffer[1024]; // ���� ����
        size_t bytesRead; // ���� ����Ʈ �� ������ ���� ����
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), inputFile)) > 0) { // ���� �б�
            fwrite(buffer, 1, bytesRead, outputFile); // ��� ���Ͽ� ����
        }

        fclose(inputFile); // �Է� ���� �ݱ�
        fclose(outputFile); // ��� ���� �ݱ�
    }
    else {
        printf("No ID3 tag found in the file\n"); // �±װ� ���� ��� �޽��� ���
        fclose(inputFile); // �Է� ���� �ݱ�
        return 1; // ���� �ڵ� ��ȯ
    }
    return 0; // ���� �ڵ� ��ȯ
}

void SaveTemporaryMP3(const wchar_t* path, wchar_t* tempPathOut, size_t stringSize) { // �ӽ� MP3 ���� ���� �Լ� ����
    if(RemoveTag(path, tempPathOut, stringSize) != 0) exit(1); // �±� ���� ���� �� ���α׷� ����
    SetFileHidden(tempPathOut); // ���� ���� �Ӽ� ����
}

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

