#pragma once // 헤더 파일이 여러 번 포함되는 것을 방지

void OpenFileDialog(wchar_t* filePath); // 파일 열기 대화상자를 여는 함수 선언
void OpenFolderDialog(wchar_t* path); // 폴더 열기 대화상자를 여는 함수 선언
void CleanupTempFolder(); // 임시 폴더를 정리하는 함수 선언
void SaveTemporaryMP3(const wchar_t* path, wchar_t* tempPathOut, size_t stringSize); // 임시 MP3 파일을 저장하는 함수 선언
void ListMP3FilesInFolder(const wchar_t* folderPath, StringArray* array); // 폴더 내 MP3 파일 목록을 나열하는 함수 선언