#pragma once // ��� ������ ���� �� ���ԵǴ� ���� ����

void OpenFileDialog(wchar_t* filePath); // ���� ���� ��ȭ���ڸ� ���� �Լ� ����
void OpenFolderDialog(wchar_t* path); // ���� ���� ��ȭ���ڸ� ���� �Լ� ����
void CleanupTempFolder(); // �ӽ� ������ �����ϴ� �Լ� ����
void SaveTemporaryMP3(const wchar_t* path, wchar_t* tempPathOut, size_t stringSize); // �ӽ� MP3 ������ �����ϴ� �Լ� ����
void ListMP3FilesInFolder(const wchar_t* folderPath, StringArray* array); // ���� �� MP3 ���� ����� �����ϴ� �Լ� ����