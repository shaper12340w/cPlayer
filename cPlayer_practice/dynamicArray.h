#pragma once // 헤더 파일이 여러 번 포함되지 않도록 방지

// StringArray 구조체 정의
typedef struct {
	int currentValue; // 현재 값의 인덱스
	size_t capacity; // 배열의 용량
	size_t wordSize; // 단어의 크기
	wchar_t** array; // 문자열 배열에 대한 포인터
} StringArray;

// IntArray 구조체 정의
typedef struct {
	int currentValue; // 현재 값의 인덱스
	size_t capacity; // 배열의 용량
	int* array; // 정수 배열에 대한 포인터
} IntArray;

// StringArray 초기화 함수 선언
void initStringArray(StringArray* arr, int cap, int size); // StringArray를 초기화하는 함수
void resizeStringArray(StringArray* arr); // StringArray의 크기를 조정하는 함수
void addStringArrayValue(StringArray* arr, const wchar_t* str); // StringArray에 값을 추가하는 함수
void changeStringArrayValue(StringArray* arr, const wchar_t* str, int index); // StringArray의 값을 변경하는 함수
void removeStringArrayValue(StringArray* arr, int index); // StringArray에서 값을 제거하는 함수
void removeStringArray(StringArray* arr); // StringArray를 제거하는 함수
int getStringArrayLength(StringArray* arr); // StringArray의 길이를 반환하는 함수
int checkStringArrayValue(StringArray* arr, const wchar_t* value); // StringArray에 특정 값이 있는지 확인하는 함수
wchar_t* getStringArrayValue(StringArray* arr, int index); // StringArray에서 특정 인덱스의 값을 반환하는 함수

// IntArray 초기화 함수 선언
void initIntArray(IntArray* arr, int cap); // IntArray를 초기화하는 함수
void resizeIntArray(IntArray* arr); // IntArray의 크기를 조정하는 함수
void addIntArrayValue(IntArray* arr, int val); // IntArray에 값을 추가하는 함수
void changeIntArrayValue(IntArray* arr, int val, int index); // IntArray의 값을 변경하는 함수
void removeIntArrayValue(IntArray* arr, int index); // IntArray에서 값을 제거하는 함수
void removeIntArray(IntArray* arr); // IntArray를 제거하는 함수
int getIntArrayLength(IntArray* arr); // IntArray의 길이를 반환하는 함수
int checkIntArrayValue(IntArray* arr, int value); // IntArray에 특정 값이 있는지 확인하는 함수
int getIntArrayValue(IntArray* arr, int index); // IntArray에서 특정 인덱스의 값을 반환하는 함수