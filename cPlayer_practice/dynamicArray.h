#pragma once
typedef struct {
	int currentValue;
	size_t capacity;
	size_t wordSize;
	wchar_t** array;
} StringArray;

void initArray(StringArray* arr, int cap, int size);
void resizeArray(StringArray* arr);
void addArrayValue(StringArray* arr, const wchar_t* str);
void changeArrayValue(StringArray* arr, const wchar_t* str, int index);
void removeArrayValue(StringArray* arr, int index);
void removeArray(StringArray* arr);
int getArrayLength(StringArray* arr);
wchar_t* getArrayValue(StringArray* arr, int index);
void inspectStringArray(const StringArray* arr);