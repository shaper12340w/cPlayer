#pragma once
void OpenFileDialog(wchar_t* filePath);
void DeleteTempFile();
void SaveTemporaryMP3(const wchar_t* path, wchar_t* tempPathOut, size_t stringSize);