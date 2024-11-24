// 필요한 헤더 파일 포함
#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>

// 정수 배열 구조체 정의
typedef struct {
	int currentValue; // 현재 값의 개수
	size_t capacity; // 배열의 용량
	int* array; // 정수 배열 포인터
} IntArray;

// 문자열 배열 구조체 정의
typedef struct  {
	int currentValue; // 현재 값의 개수
	size_t capacity; // 배열의 용량
	size_t wordSize; // 단어의 크기
	wchar_t** array; // 문자열 배열 포인터
} StringArray;

// 정수 배열 초기화 함수
void initIntArray(IntArray* arr, int cap) {
	arr->currentValue = 0; // 현재 값의 개수를 0으로 초기화
	arr->capacity = (size_t)cap; // 용량 설정
	arr->array = (int*)malloc(arr->capacity * sizeof(int)); // 배열 메모리 할당
}

// 문자열 배열 초기화 함수
void initStringArray(StringArray* arr, int cap, int size) {
	arr->currentValue = 0; // 현재 값의 개수를 0으로 초기화
	arr->capacity = (size_t)cap; // 용량 설정
	arr->wordSize = (size_t)size; // 단어 크기 설정
	arr->array = (wchar_t**)malloc(arr->capacity * sizeof(wchar_t*)); // 배열 메모리 할당
	for (int i = 0; i < cap; i++) // 각 문자열에 메모리 할당
		arr->array[i] = (wchar_t*)malloc(arr->wordSize * sizeof(wchar_t));
}

// 정수 배열 크기 조정 함수
void resizeIntArray(IntArray* arr) {
	arr->capacity *= 2; // 용량을 두 배로 증가
	arr->array = (int*)realloc(arr->array, arr->capacity * sizeof(int)); // 배열 메모리 재할당
}

// 문자열 배열 크기 조정 함수
void resizeStringArray(StringArray* arr) {
    size_t oldCapacity = arr->capacity; // 이전 용량 저장
    arr->capacity *= 2; // 용량을 두 배로 증가
    arr->array = (wchar_t**)realloc(arr->array, arr->capacity * sizeof(wchar_t*)); // 배열 메모리 재할당
    if (arr->array == NULL) { // 메모리 재할당 실패 시
        printf("Error: Memory reallocation failed.\n"); // 오류 메시지 출력
        exit(1); // 프로그램 종료
    }

    for (size_t i = oldCapacity; i < arr->capacity; i++) { // 새로운 슬롯에 메모리 할당
        arr->array[i] = (wchar_t*)malloc(arr->wordSize * sizeof(wchar_t));
        if (arr->array[i] == NULL) { // 메모리 할당 실패 시
            printf("Error: Memory allocation failed for new slot %zu.\n", i); // 오류 메시지 출력
            exit(1); // 프로그램 종료
        }
    }
}

// 정수 배열에 값 추가 함수
void addIntArrayValue(IntArray* arr, int val) {
	if (arr->currentValue >= arr->capacity) // 현재 값의 개수가 용량을 초과하면
		resizeIntArray(arr); // 배열 크기 조정
	arr->array[arr->currentValue] = val; // 배열에 값 추가
	arr->currentValue++; // 현재 값의 개수 증가
}

// 문자열 배열에 값 추가 함수
void addStringArrayValue(StringArray* arr, const wchar_t* str) {
	if (arr->currentValue >= arr->capacity) // 현재 값의 개수가 용량을 초과하면
		resizeStringArray(arr); // 배열 크기 조정
	if (arr->array[arr->currentValue] != NULL && wcslen(str) < arr->wordSize) { // 문자열 길이가 단어 크기보다 작으면
		wcscpy_s(arr->array[arr->currentValue], arr->wordSize, str); // 문자열 복사
		arr->currentValue++; // 현재 값의 개수 증가
	}
	else { // 문자열 길이가 단어 크기보다 크면
		printf("Invalid operation: buffer too small or memory allocation failed.\n"); // 오류 메시지 출력
		exit(1); // 프로그램 종료
	}
}

// 정수 배열의 값 변경 함수
void changeIntArrayValue(IntArray* arr, int val, int index) {
	if (index < 0 || index >= arr->currentValue) { // 인덱스가 유효하지 않으면
		printf("Wrong Value!\n"); // 오류 메시지 출력
		return; // 함수 종료
	}
	arr->array[index] = val; // 배열의 값 변경
}

// 문자열 배열의 값 변경 함수
void changeStringArrayValue(StringArray* arr, const wchar_t* str, int index) {
    if (index < 0 || index >= arr->currentValue) { // 인덱스가 유효하지 않으면
        printf("Wrong Value!\n"); // 오류 메시지 출력
        return; // 함수 종료
    }
    wcscpy_s(arr->array[index], arr->wordSize, str); // 문자열 복사
}

// 정수 배열의 값 제거 함수
void removeIntArrayValue(IntArray* arr, int index) {
	if (index < 0 || index >= arr->currentValue) { // 인덱스가 유효하지 않으면
		printf("Wrong Value!\n"); // 오류 메시지 출력
		return; // 함수 종료
	}

	for (int i = index; i < arr->currentValue - 1; i++) { // 값을 한 칸씩 앞으로 이동
		arr->array[i] = arr->array[i + 1];
	}
	arr->currentValue--; // 현재 값의 개수 감소
}

// 문자열 배열의 값 제거 함수
void removeStringArrayValue(StringArray* arr, int index) {
    if (index < 0 || index >= arr->currentValue) { // 인덱스가 유효하지 않으면
        printf("Wrong Value!\n"); // 오류 메시지 출력
        return; // 함수 종료
    }
   
    for (int i = index; i < arr->currentValue - 1; i++) { // 값을 한 칸씩 앞으로 이동
        arr->array[i] = arr->array[i + 1];
    }
    arr->array[arr->currentValue - 1] = (wchar_t*)malloc(arr->wordSize * sizeof(wchar_t)); // 마지막 슬롯에 메모리 할당
    arr->currentValue--; // 현재 값의 개수 감소
}

// 정수 배열 메모리 해제 함수
void removeIntArray(IntArray* arr) {
	free(arr->array); // 배열 메모리 해제
}

// 문자열 배열 메모리 해제 함수
void removeStringArray(StringArray* arr) {
	for (int i = 0; i < arr->capacity; i++) // 각 문자열 메모리 해제
		free(arr->array[i]);
	free(arr->array); // 배열 메모리 해제
}

// 정수 배열의 길이 반환 함수
int getIntArrayLength(IntArray* arr) {
	return arr->currentValue; // 현재 값의 개수 반환
}

// 문자열 배열의 길이 반환 함수
int getStringArrayLength(StringArray* arr) {
	return arr->currentValue; // 현재 값의 개수 반환
}

// 정수 배열에서 값의 위치 확인 함수
int checkIntArrayValue(IntArray* arr, int value) {
	for (int i = 0; i < arr->capacity; i++) { // 배열을 순회하며 값 확인
		if (arr->array[i] == value) return i; // 값이 일치하면 인덱스 반환
	}
	return -1; // 값이 없으면 -1 반환
}

// 문자열 배열에서 값의 위치 확인 함수
int checkStringArrayValue(StringArray* arr, const wchar_t* value) {
	for (int i = 0; i < arr->capacity; i++) { // 배열을 순회하며 값 확인
		if (wcscmp(arr->array[i], value)) return i; // 값이 일치하면 인덱스 반환
	}
	return -1; // 값이 없으면 -1 반환
}

// 정수 배열에서 값 반환 함수
int getIntArrayValue(IntArray* arr, int index) {
	if (index < 0 || index >= arr->currentValue) { // 인덱스가 유효하지 않으면
		printf("Error: Invalid index (%d). Current size: %d\n", index, arr->currentValue); // 오류 메시지 출력
		return NULL; // NULL 반환
	}
	return arr->array[index]; // 값 반환
}

// 문자열 배열에서 값 반환 함수
wchar_t* getStringArrayValue(StringArray* arr, int index) {
	if (index < 0 || index >= arr->currentValue) { // 인덱스가 유효하지 않으면
		printf("Error: Invalid index (%d). Current size: %d\n", index, arr->currentValue); // 오류 메시지 출력
		return NULL; // NULL 반환
	}
	return arr->array[index]; // 값 반환
}

