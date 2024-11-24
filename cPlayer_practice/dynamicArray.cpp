// �ʿ��� ��� ���� ����
#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>

// ���� �迭 ����ü ����
typedef struct {
	int currentValue; // ���� ���� ����
	size_t capacity; // �迭�� �뷮
	int* array; // ���� �迭 ������
} IntArray;

// ���ڿ� �迭 ����ü ����
typedef struct  {
	int currentValue; // ���� ���� ����
	size_t capacity; // �迭�� �뷮
	size_t wordSize; // �ܾ��� ũ��
	wchar_t** array; // ���ڿ� �迭 ������
} StringArray;

// ���� �迭 �ʱ�ȭ �Լ�
void initIntArray(IntArray* arr, int cap) {
	arr->currentValue = 0; // ���� ���� ������ 0���� �ʱ�ȭ
	arr->capacity = (size_t)cap; // �뷮 ����
	arr->array = (int*)malloc(arr->capacity * sizeof(int)); // �迭 �޸� �Ҵ�
}

// ���ڿ� �迭 �ʱ�ȭ �Լ�
void initStringArray(StringArray* arr, int cap, int size) {
	arr->currentValue = 0; // ���� ���� ������ 0���� �ʱ�ȭ
	arr->capacity = (size_t)cap; // �뷮 ����
	arr->wordSize = (size_t)size; // �ܾ� ũ�� ����
	arr->array = (wchar_t**)malloc(arr->capacity * sizeof(wchar_t*)); // �迭 �޸� �Ҵ�
	for (int i = 0; i < cap; i++) // �� ���ڿ��� �޸� �Ҵ�
		arr->array[i] = (wchar_t*)malloc(arr->wordSize * sizeof(wchar_t));
}

// ���� �迭 ũ�� ���� �Լ�
void resizeIntArray(IntArray* arr) {
	arr->capacity *= 2; // �뷮�� �� ��� ����
	arr->array = (int*)realloc(arr->array, arr->capacity * sizeof(int)); // �迭 �޸� ���Ҵ�
}

// ���ڿ� �迭 ũ�� ���� �Լ�
void resizeStringArray(StringArray* arr) {
    size_t oldCapacity = arr->capacity; // ���� �뷮 ����
    arr->capacity *= 2; // �뷮�� �� ��� ����
    arr->array = (wchar_t**)realloc(arr->array, arr->capacity * sizeof(wchar_t*)); // �迭 �޸� ���Ҵ�
    if (arr->array == NULL) { // �޸� ���Ҵ� ���� ��
        printf("Error: Memory reallocation failed.\n"); // ���� �޽��� ���
        exit(1); // ���α׷� ����
    }

    for (size_t i = oldCapacity; i < arr->capacity; i++) { // ���ο� ���Կ� �޸� �Ҵ�
        arr->array[i] = (wchar_t*)malloc(arr->wordSize * sizeof(wchar_t));
        if (arr->array[i] == NULL) { // �޸� �Ҵ� ���� ��
            printf("Error: Memory allocation failed for new slot %zu.\n", i); // ���� �޽��� ���
            exit(1); // ���α׷� ����
        }
    }
}

// ���� �迭�� �� �߰� �Լ�
void addIntArrayValue(IntArray* arr, int val) {
	if (arr->currentValue >= arr->capacity) // ���� ���� ������ �뷮�� �ʰ��ϸ�
		resizeIntArray(arr); // �迭 ũ�� ����
	arr->array[arr->currentValue] = val; // �迭�� �� �߰�
	arr->currentValue++; // ���� ���� ���� ����
}

// ���ڿ� �迭�� �� �߰� �Լ�
void addStringArrayValue(StringArray* arr, const wchar_t* str) {
	if (arr->currentValue >= arr->capacity) // ���� ���� ������ �뷮�� �ʰ��ϸ�
		resizeStringArray(arr); // �迭 ũ�� ����
	if (arr->array[arr->currentValue] != NULL && wcslen(str) < arr->wordSize) { // ���ڿ� ���̰� �ܾ� ũ�⺸�� ������
		wcscpy_s(arr->array[arr->currentValue], arr->wordSize, str); // ���ڿ� ����
		arr->currentValue++; // ���� ���� ���� ����
	}
	else { // ���ڿ� ���̰� �ܾ� ũ�⺸�� ũ��
		printf("Invalid operation: buffer too small or memory allocation failed.\n"); // ���� �޽��� ���
		exit(1); // ���α׷� ����
	}
}

// ���� �迭�� �� ���� �Լ�
void changeIntArrayValue(IntArray* arr, int val, int index) {
	if (index < 0 || index >= arr->currentValue) { // �ε����� ��ȿ���� ������
		printf("Wrong Value!\n"); // ���� �޽��� ���
		return; // �Լ� ����
	}
	arr->array[index] = val; // �迭�� �� ����
}

// ���ڿ� �迭�� �� ���� �Լ�
void changeStringArrayValue(StringArray* arr, const wchar_t* str, int index) {
    if (index < 0 || index >= arr->currentValue) { // �ε����� ��ȿ���� ������
        printf("Wrong Value!\n"); // ���� �޽��� ���
        return; // �Լ� ����
    }
    wcscpy_s(arr->array[index], arr->wordSize, str); // ���ڿ� ����
}

// ���� �迭�� �� ���� �Լ�
void removeIntArrayValue(IntArray* arr, int index) {
	if (index < 0 || index >= arr->currentValue) { // �ε����� ��ȿ���� ������
		printf("Wrong Value!\n"); // ���� �޽��� ���
		return; // �Լ� ����
	}

	for (int i = index; i < arr->currentValue - 1; i++) { // ���� �� ĭ�� ������ �̵�
		arr->array[i] = arr->array[i + 1];
	}
	arr->currentValue--; // ���� ���� ���� ����
}

// ���ڿ� �迭�� �� ���� �Լ�
void removeStringArrayValue(StringArray* arr, int index) {
    if (index < 0 || index >= arr->currentValue) { // �ε����� ��ȿ���� ������
        printf("Wrong Value!\n"); // ���� �޽��� ���
        return; // �Լ� ����
    }
   
    for (int i = index; i < arr->currentValue - 1; i++) { // ���� �� ĭ�� ������ �̵�
        arr->array[i] = arr->array[i + 1];
    }
    arr->array[arr->currentValue - 1] = (wchar_t*)malloc(arr->wordSize * sizeof(wchar_t)); // ������ ���Կ� �޸� �Ҵ�
    arr->currentValue--; // ���� ���� ���� ����
}

// ���� �迭 �޸� ���� �Լ�
void removeIntArray(IntArray* arr) {
	free(arr->array); // �迭 �޸� ����
}

// ���ڿ� �迭 �޸� ���� �Լ�
void removeStringArray(StringArray* arr) {
	for (int i = 0; i < arr->capacity; i++) // �� ���ڿ� �޸� ����
		free(arr->array[i]);
	free(arr->array); // �迭 �޸� ����
}

// ���� �迭�� ���� ��ȯ �Լ�
int getIntArrayLength(IntArray* arr) {
	return arr->currentValue; // ���� ���� ���� ��ȯ
}

// ���ڿ� �迭�� ���� ��ȯ �Լ�
int getStringArrayLength(StringArray* arr) {
	return arr->currentValue; // ���� ���� ���� ��ȯ
}

// ���� �迭���� ���� ��ġ Ȯ�� �Լ�
int checkIntArrayValue(IntArray* arr, int value) {
	for (int i = 0; i < arr->capacity; i++) { // �迭�� ��ȸ�ϸ� �� Ȯ��
		if (arr->array[i] == value) return i; // ���� ��ġ�ϸ� �ε��� ��ȯ
	}
	return -1; // ���� ������ -1 ��ȯ
}

// ���ڿ� �迭���� ���� ��ġ Ȯ�� �Լ�
int checkStringArrayValue(StringArray* arr, const wchar_t* value) {
	for (int i = 0; i < arr->capacity; i++) { // �迭�� ��ȸ�ϸ� �� Ȯ��
		if (wcscmp(arr->array[i], value)) return i; // ���� ��ġ�ϸ� �ε��� ��ȯ
	}
	return -1; // ���� ������ -1 ��ȯ
}

// ���� �迭���� �� ��ȯ �Լ�
int getIntArrayValue(IntArray* arr, int index) {
	if (index < 0 || index >= arr->currentValue) { // �ε����� ��ȿ���� ������
		printf("Error: Invalid index (%d). Current size: %d\n", index, arr->currentValue); // ���� �޽��� ���
		return NULL; // NULL ��ȯ
	}
	return arr->array[index]; // �� ��ȯ
}

// ���ڿ� �迭���� �� ��ȯ �Լ�
wchar_t* getStringArrayValue(StringArray* arr, int index) {
	if (index < 0 || index >= arr->currentValue) { // �ε����� ��ȿ���� ������
		printf("Error: Invalid index (%d). Current size: %d\n", index, arr->currentValue); // ���� �޽��� ���
		return NULL; // NULL ��ȯ
	}
	return arr->array[index]; // �� ��ȯ
}

