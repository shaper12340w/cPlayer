#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <shlobj.h>

wchar_t rmPath[MAX_PATH];
int stack = 0;

int SetFileHidden(const wchar_t* path) {
	if (!SetFileAttributesW(path, FILE_ATTRIBUTE_HIDDEN)) {
		printf("Cannot set file hidden\n");
		return 1;
	}
	return 0;
}

int RemoveTag(const wchar_t* originPath,wchar_t* tempPathOut,size_t stringSize) {

	wchar_t tempPath[MAX_PATH] = L"";
	wchar_t appPath[MAX_PATH];
	const wchar_t* fileName = wcsrchr(originPath, L'\\');
	GetModuleFileNameW(NULL, appPath, MAX_PATH); //현재 실행중인 앱 위치 저장
	*wcsrchr(appPath, L'\\') = L'\0'; //글자 자르기

	
	wcscat(tempPath, appPath);
	wcscat(tempPath, fileName);
	wcscpy_s(tempPathOut, stringSize, tempPath);

	FILE* inputFile = _wfopen(originPath, L"rb");	
	if(!inputFile){
		printf("Cannot open file\n");
		return 1;
	}

	unsigned char header[10];
	fread(header, 1, 10, inputFile);
	
	if (strncmp((char*)header, "ID3",2) == 0) { //ID3v2 (mp3태그)가 저장되어있는 정보의 시작점 = 해더 확인
		unsigned int tagSize = (header[6] << 21) | (header[7] << 14) | (header[8] << 7) | header[9]; //header의 6~9까지의 값 합산 -> 전체 태그 크기
		FILE* outputFile = _wfopen(tempPath, L"wb"); //쓰기 모드로 열기
		if (!outputFile) {
			printf("Cannot make file\n");
			fclose(inputFile);
			if (stack < 5) {
				_wremove(tempPath);
				RemoveTag(originPath, tempPathOut, stringSize);
				stack++;
			}
			else
				exit(0);
			return 1;
		}
		fseek(inputFile, 10 + tagSize, SEEK_SET); //파일 포인터(위치)값을 해더 크기+태그 크기로 옮기기

		unsigned char buffer[1024];
		size_t bytesRead;
		while ((bytesRead = fread(buffer, 1, sizeof(buffer), inputFile)) > 0){ //한번 fread를 실행하면 자동으로 그만큼 포인터가 이동한다
			fwrite(buffer, 1, bytesRead, outputFile); //fwrite도 마찬가지
		}

		fclose(inputFile);
		fclose(outputFile);
	}
	else {
		printf("Cannot Find Tag\n");
		fclose(inputFile);
		return 1;
	}
	return 0;
}

void DeleteTempFile() {
	char temp[260];
	wcstombs(temp, rmPath, sizeof(temp));
	remove(temp);
}

void SaveTemporaryMP3(const wchar_t* path, wchar_t* tempPathOut, size_t stringSize) {
	wcscpy_s(rmPath, stringSize, tempPathOut);
	atexit(DeleteTempFile);
	RemoveTag(path, tempPathOut, stringSize);
	SetFileHidden(tempPathOut);
}

void OpenFileDialog(wchar_t* filePath) {

	OPENFILENAMEW ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.hwndOwner = NULL; 
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"MP3 Files\0*.mp3\0";
	ofn.nFileOffset = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameW(&ofn)) 
		wprintf(L"Selected File : %ls\n", filePath);
	else
		printf("Canceled\n");
}