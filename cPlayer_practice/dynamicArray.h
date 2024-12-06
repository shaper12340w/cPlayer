#pragma once // ��� ������ ���� �� ���Ե��� �ʵ��� ����

// StringArray ����ü ����
typedef struct {
	int currentValue; // ���� ���� �ε���
	size_t capacity; // �迭�� �뷮
	size_t wordSize; // �ܾ��� ũ��
	wchar_t** array; // ���ڿ� �迭�� ���� ������
} StringArray;

// IntArray ����ü ����
typedef struct {
	int currentValue; // ���� ���� �ε���
	size_t capacity; // �迭�� �뷮
	int* array; // ���� �迭�� ���� ������
} IntArray;

// StringArray �ʱ�ȭ �Լ� ����
void initStringArray(StringArray* arr, int cap, int size); // StringArray�� �ʱ�ȭ�ϴ� �Լ�
void resizeStringArray(StringArray* arr); // StringArray�� ũ�⸦ �����ϴ� �Լ�
void addStringArrayValue(StringArray* arr, const wchar_t* str); // StringArray�� ���� �߰��ϴ� �Լ�
void changeStringArrayValue(StringArray* arr, const wchar_t* str, int index); // StringArray�� ���� �����ϴ� �Լ�
void removeStringArrayValue(StringArray* arr, int index); // StringArray���� ���� �����ϴ� �Լ�
void removeStringArray(StringArray* arr); // StringArray�� �����ϴ� �Լ�
int getStringArrayLength(StringArray* arr); // StringArray�� ���̸� ��ȯ�ϴ� �Լ�
int checkStringArrayValue(StringArray* arr, const wchar_t* value); // StringArray�� Ư�� ���� �ִ��� Ȯ���ϴ� �Լ�
wchar_t* getStringArrayValue(StringArray* arr, int index); // StringArray���� Ư�� �ε����� ���� ��ȯ�ϴ� �Լ�

// IntArray �ʱ�ȭ �Լ� ����
void initIntArray(IntArray* arr, int cap); // IntArray�� �ʱ�ȭ�ϴ� �Լ�
void resizeIntArray(IntArray* arr); // IntArray�� ũ�⸦ �����ϴ� �Լ�
void addIntArrayValue(IntArray* arr, int val); // IntArray�� ���� �߰��ϴ� �Լ�
void changeIntArrayValue(IntArray* arr, int val, int index); // IntArray�� ���� �����ϴ� �Լ�
void removeIntArrayValue(IntArray* arr, int index); // IntArray���� ���� �����ϴ� �Լ�
void removeIntArray(IntArray* arr); // IntArray�� �����ϴ� �Լ�
int getIntArrayLength(IntArray* arr); // IntArray�� ���̸� ��ȯ�ϴ� �Լ�
int checkIntArrayValue(IntArray* arr, int value); // IntArray�� Ư�� ���� �ִ��� Ȯ���ϴ� �Լ�
int getIntArrayValue(IntArray* arr, int index); // IntArray���� Ư�� �ε����� ���� ��ȯ�ϴ� �Լ�