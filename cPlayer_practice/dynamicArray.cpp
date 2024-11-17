#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>

typedef struct  {
	int currentValue;
	size_t capacity;
	size_t wordSize;
	wchar_t** array;
} StringArray;

void inspectStringArray(const StringArray* arr);

void initArray(StringArray* arr, int cap, int size) {
	arr->currentValue = 0;
	arr->capacity = (size_t)cap;
	arr->wordSize = (size_t)size;
	arr->array = (wchar_t**)malloc(arr->capacity * sizeof(wchar_t*));
	for (int i = 0;i < cap;i++)
		arr->array[i] = (wchar_t*)malloc(arr->wordSize * sizeof(wchar_t));
}

void resizeArray(StringArray* arr) {
    size_t oldCapacity = arr->capacity;
    arr->capacity *= 2;
    arr->array = (wchar_t**)realloc(arr->array, arr->capacity * sizeof(wchar_t*));
    if (arr->array == NULL) {
        printf("Error: Memory reallocation failed.\n");
        exit(1);
    }

    for (size_t i = oldCapacity; i < arr->capacity; i++) {
        arr->array[i] = (wchar_t*)malloc(arr->wordSize * sizeof(wchar_t));
        if (arr->array[i] == NULL) {
            printf("Error: Memory allocation failed for new slot %zu.\n", i);
            exit(1);
        }
    }
}

void addArrayValue(StringArray* arr, const wchar_t* str) {
	if (arr->currentValue >= arr->capacity)
		resizeArray(arr);
	if (arr->array[arr->currentValue] != NULL && wcslen(str) < arr->wordSize) {
		wcscpy_s(arr->array[arr->currentValue], arr->wordSize, str);
		arr->currentValue++;
	}
	else {
		printf("Invalid operation: buffer too small or memory allocation failed.\n");
		exit(1);
	}
    
}

void changeArrayValue(StringArray* arr, const wchar_t* str, int index) {
    if (index < 0 || index >= arr->currentValue) {
        printf("Wrong Value!\n");
        return;
    }
    wcscpy_s(arr->array[index], arr->wordSize, str);
}

void removeArrayValue(StringArray* arr, int index) {
    if (index < 0 || index >= arr->currentValue) {
        printf("Wrong Value!\n");
        return;
    }
   
    for (int i = index; i < arr->currentValue - 1; i++) {
        arr->array[i] = arr->array[i + 1];
    }
    arr->array[arr->currentValue - 1] = (wchar_t*)malloc(arr->wordSize * sizeof(wchar_t));
    arr->currentValue--;
}

void removeArray(StringArray* arr) {
	for (int i = 0;i < arr->capacity;i++)
		free(arr->array[i]);
	free(arr->array);
}

int getArrayLength(StringArray* arr) {
	return arr->currentValue-1;
}

wchar_t* getArrayValue(StringArray* arr, int index) {
	if (index < 0 || index >= arr->currentValue) {
		printf("Error: Invalid index (%d). Current size: %d\n", index, arr->currentValue);
		return NULL;
	}
	return arr->array[index];
}

