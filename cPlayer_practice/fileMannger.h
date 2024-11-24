#pragma once
void OpenFileDialog(wchar_t* filePath);
void OpenFolderDialog(wchar_t* path);
void DeleteTempFile();
void SaveTemporaryMP3(const wchar_t* path, wchar_t* tempPathOut, size_t stringSize);
void ListMP3FilesInFolder(const wchar_t* folderPath, StringArray* array);