#include <tchar.h>  // �����ڵ� �� ��Ƽ����Ʈ ���� ���� �Լ� �� ��ũ�� ����
#include <windows.h>  // Windows API �Լ� �� ������ Ÿ�� ����
#include <psapi.h>  // ���μ��� ���� API �Լ� ����
#include <stdio.h>  // ǥ�� ����� �Լ� ����
#include "playerUI.h"  // ����� ���� ��� ����, playerUI ���� �Լ� �� ������ Ÿ�� ����
#include "musicPlayer.h"  // ����� ���� ��� ����, musicPlayer ���� �Լ� �� ������ Ÿ�� ����

int isTerminal = -1;  // �͹̳� ���θ� �����ϴ� ���� ����, �ʱⰪ�� -1

typedef struct {  // EnumData ����ü ����
   HWND* targetHwnd;  // ã�� ������ �ڵ��� ������ ������
   const TCHAR* windowName;  // ã���� �ϴ� ������ �̸�
} EnumData;


/*
* �ܼ� â�� ũ�⸦ �����ϱ� ����, Ư�� �������� "�͹̳�" ���� â�� ũ�⸦ �����ϱ� ���� â�� �ڵ�(����)��
* �������� �ݹ�(������ ���������� ����) �ϴ� �Լ��Դϴ�. (�ƴ� ��� TRUE ��ȯ�Ͽ� ��� Ž��)
* 
* �������� ������ �������� ���� : ������ ����� �� â�� �̸��� ������ ���� ��ġ�ϵ��� �����Ͽ�
* �ٸ� �͹̳� â�� ��ġ�� �ʵ��� �ϱ� �����Դϴ�.
* 
* GetWindowThreadProcessId : ������ ���μ����� ���̵� �����ͼ�
* OpenProcess : ������ �޸𸮿� ���� ���� ��û(PROCESS_VM_READ)
* 
* ���μ��� �̸��� �����ͼ� �͹̳��� ��쿡 hwnd �μ��� ���� �Ѱ��ְ� �Լ��� �����մϴ�
*/

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {  // EnumWindows �Լ��� ���޵� �ݹ� �Լ�
   EnumData* data = (EnumData*)lParam;  // lParam�� EnumData ����ü�� ĳ����
   DWORD processId;  // ���μ��� ID�� ������ ����
   TCHAR windowTitle[MAX_PATH] = { 0 };  // ������ ������ ������ �迭

   GetWindowText(hwnd, windowTitle, MAX_PATH);  // hwnd�� �ش��ϴ� �������� ������ ������

   if (data->windowName != NULL && _tcsstr(windowTitle, data->windowName) == NULL) {  // ������ �̸� ��
       return TRUE;  // ������ �̸��� �ٸ��� ��� Ž��
   }

   GetWindowThreadProcessId(hwnd, &processId);  // �������� ���μ��� ID�� ������
   HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);  // ���μ��� �ڵ��� ��

   if (processHandle) {  // ���μ��� �ڵ��� ��ȿ�� ���
       TCHAR processName[MAX_PATH] = { 0 };  // ���μ��� �̸��� ������ �迭
       if (GetModuleBaseName(processHandle, NULL, processName, MAX_PATH)) {  // ���μ��� �̸��� ������
           if (_tcsicmp(processName, _T("WindowsTerminal.exe")) == 0) {  // ���μ��� �̸��� "WindowsTerminal.exe"���� ��
               *(data->targetHwnd) = hwnd;  // �߰��� �ڵ��� ����
               CloseHandle(processHandle);  // ���μ��� �ڵ��� ����
               return FALSE;  // Ž�� ����
           }
       }
       CloseHandle(processHandle);  // ���μ��� �ڵ��� ����
   }
   return TRUE;  // ��� Ž��
}

HWND FindTerminalWindow(const TCHAR* windowName) {  // �͹̳� �����츦 ã�� �Լ�
   HWND hwnd = NULL;  // ������ �ڵ��� ������ ���� �ʱ�ȭ
   EnumData data = { &hwnd, windowName };  // EnumData ����ü �ʱ�ȭ
   EnumWindows(EnumWindowsProc, (LPARAM)&data);  // ��� �����츦 �����Ͽ� �ݹ� �Լ� ȣ��
   return hwnd;  // ã�� ������ �ڵ� ��ȯ
}

/*
* Windows, Ư�� ������ 11���� �ܼ��� �ΰ����� �ֽ��ϴ�
* 1. �⺻ �ܼ� �� ȣ��Ʈ (conhost.exe or cmd.exe)
* 2. �͹̳� (WindowsTerminal.exe)
* 
* �͹̳��� ���� �ܼ��� ����ȣȯ �������� �������� ����ϱ� ���ϵ��� ���� �ΰ� ����� ���� �ܼ��Դϴٸ�
* �ƽ��Ե� system�Լ��� �̿��ؼ� mode �ܼ� ��ɾ�� â ũ�� ������ �Ұ��ϴ�����
* 
* �׷��Ƿ� ������ â�� �ڵ��� �����ͼ� â ũ�� ������ ��Ű�� ����� ����߽��ϴ�
* �׷��� �⺻ �ܼ��� � ������ ����Ǿ��ִ� ���� ������Ʈ��(�������� ������) �� ������ �Ǿ��ִµ�
* �̸� �о �ܼ� �� ȣ��Ʈ�� �⺻���� �������� �ƴ� �͹̳��� �������� �����ؼ� �˸°� â ũ�⸦
* �����ϵ��� �Ϸ��� ���� �������� �Լ��Դϴ�
* 
* ���� �׽�Ʈ �ϽŴٸ� ���� �⺻ �ܼ� ���� ��õ�մϴ�. ������Ʈ�� ������ ������ ���� ���ϴ� ��Ŀ�
* ���� �����ϵ��� �۵��Ǳ� �����Դϴ�.
*/

int CheckConsoleRegistry() {  // �ܼ� ������Ʈ���� Ȯ���ϴ� �Լ�
   HKEY hkey;  // ������Ʈ�� Ű �ڵ�
   DWORD dwSize;  // ������ ũ��
   wchar_t value[256];  // ������Ʈ�� ���� ������ �迭

   if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("CONSOLE\\%%Startup"), 0, KEY_READ, &hkey) != ERROR_SUCCESS) {  // ������Ʈ�� Ű ����
       clearLine(6); uprintf(6, L"������Ʈ���� ���� �������� ������ �߻��߽��ϴ�");  // ���� �޽��� ���
       return -1;  // ���� ��ȯ
   }

   dwSize = sizeof(value);  // ������ ũ�� ����

   if (RegQueryValueExW(hkey, TEXT("DelegationConsole"), NULL, NULL, (LPBYTE)value, &dwSize) == ERROR_SUCCESS) {  // ������Ʈ�� �� ��ȸ
       if (wcscmp(value, L"{2EACA947-7F5F-4CFA-BA87-8F7FBEEFBE69}") == 0)  // �� ��
           return 0; // yes
       else
           return 1; // no
   }
   else
       return -1;  // ���� ��ȯ
}

void resizeWindow(int width, int height) {  // ������ ũ�⸦ �����ϴ� �Լ�
   
   if (isTerminal == -1) isTerminal = CheckConsoleRegistry();  // �͹̳� ���� Ȯ��
   HWND targetHwnd = FindTerminalWindow(player.pathArray.array != NULL ? wcsrchr(getStringArrayValue(&player.pathArray, player.playIndex), L'\\') + 1 : NULL);  // �͹̳� ������ ã��
   switch (isTerminal) {  // �͹̳� ���ο� ���� �б�
   case 0:
       
       if (targetHwnd == NULL) break;  // ������ �ڵ��� NULL�� ��� ����
       RECT rect;  // ������ ��ġ�� ũ�⸦ ������ ����ü
       GetWindowRect(targetHwnd, &rect);  // ������ ��ġ�� ũ�� ��������
       MoveWindow(targetHwnd, rect.left, rect.top, width*10, height*23, TRUE);  // ������ ũ�� ����
       break;

   case 1:

       char buffer[256];  // ��ɾ ������ �迭
       sprintf_s(buffer,"mode con cols=%d lines=%d",width,height);  // ��ɾ� ����
       system(buffer);  // ��ɾ� ����
       break;

   }
}
