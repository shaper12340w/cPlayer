#pragma once
typedef struct {
	int currentValue;
	size_t capacity;
	size_t wordSize;
	wchar_t** array;
} StringArray;

typedef struct {
	int currentValue;
	size_t capacity;
	int* array;
} IntArray;


void initStringArray(StringArray* arr, int cap, int size);
void resizeStringArray(StringArray* arr);
void addStringArrayValue(StringArray* arr, const wchar_t* str);
void changeStringArrayValue(StringArray* arr, const wchar_t* str, int index);
void removeStringArrayValue(StringArray* arr, int index);
void removeStringArray(StringArray* arr);
int getStringArrayLength(StringArray* arr);
int checkStringArrayValue(StringArray* arr, const wchar_t* value);
wchar_t* getStringArrayValue(StringArray* arr, int index);


void initIntArray(IntArray* arr, int cap);
void resizeIntArray(IntArray* arr);
void addIntArrayValue(IntArray* arr, int val);
void changeIntArrayValue(IntArray* arr, int val, int index);
void removeIntArrayValue(IntArray* arr, int index);
void removeIntArray(IntArray* arr);
int getIntArrayLength(IntArray* arr);
int checkIntArrayValue(IntArray* arr, int value);
int getIntArrayValue(IntArray* arr, int index);