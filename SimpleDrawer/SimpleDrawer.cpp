#include "stdafx.h"
#define MAX_CHARS 200

// SimpleDrawer.cpp : ����Ӧ�ó������ڵ㡣
//

HDC hDC;							// ���ھ����ȫ�ֱ���
char SrcFilePath[MAX_CHARS];		// ���ڴ��Դ�����ļ�·��
static char Name[] = "Compiler";	// ������

									// ------------------------- ��ʼ�����ں�������
static bool PrepareWindow(HINSTANCE, HINSTANCE, int);

// ------------------------- ���Դ�����ļ��Ƿ�Ϸ���������
static bool CheckSrcFile(LPSTR);

// ------------------------- ������Ϣ����������
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ------------------------- window����������
int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
#ifdef DEBUG
	scanner(lpCmdLine);
#endif
	// ����Դ�ļ�·��
	strcpy_s(SrcFilePath, lpCmdLine);
	//  strcpy(SrcFilePath, "test5.txt");
	// ��ʼ������.
	if (PrepareWindow(hInstance, hPrevInstance, nCmdShow) != true)
	{
		MessageBox(NULL, "���ڳ�ʼ��ʧ�� !", "����", MB_OK);
		return 1;
	}
	// ���Ҫ������Դ�����ļ�
	//if ( !CheckSrcFile(lpCmdLine)) return 1;
	if (!CheckSrcFile(SrcFilePath)) return 1;

	// --------------------------------------------
	//		���û�ͼ���Խ�����
	parser(SrcFilePath);
	// --------------------------------------------

	// ����window��Ϣѭ��
	MSG Msg;
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

// ------------------------- ��ʼ�����ں���ʵ��
bool PrepareWindow(HINSTANCE hInst,
	HINSTANCE hPrevInstance,
	int		 nCmdShow)
{
	HWND		hWnd;
	WNDCLASS	W;

	memset(&W, 0, sizeof(WNDCLASS));
	W.style = CS_HREDRAW | CS_VREDRAW;
	W.lpfnWndProc = WndProc;
	W.hInstance = hInst;
	W.hCursor = LoadCursor(NULL, IDC_ARROW);
	W.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	W.lpszClassName = Name;
	RegisterClass(&W);

	hWnd = CreateWindow(Name, Name,
		WS_OVERLAPPEDWINDOW, 10, 10, 740, 490, NULL, NULL, hInst, NULL);
	if (hWnd == NULL)	return false;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetCursor(LoadCursor(hInst, IDC_ARROW));

	hDC = GetDC(hWnd);
	return true;
}

// ------------------------- ���Դ�����ļ��Ƿ�Ϸ�����ʵ��
bool CheckSrcFile(LPSTR lpszCmdParam)
{
	FILE * file = NULL;

	if (strlen(lpszCmdParam) == 0)
	{
		MessageBox(NULL, "δָ��Դ�����ļ� !", "����", MB_OK);
		return false;
	}
	if (fopen_s(&file, lpszCmdParam, "r") != 0)
	{
		MessageBox(NULL, "��Դ�����ļ����� !", "����", MB_OK);
		MessageBox(NULL, lpszCmdParam, "�ļ���", MB_OK);
		return false;
	}
	else fclose(file);
	return true;
}

// ------------------------- ���ڴ�����ʵ��
LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_DESTROY:
		ReleaseDC(hWnd, hDC);
		PostQuitMessage(0);
		return 0;
		//			break;
	case WM_PAINT:
		PAINTSTRUCT pt;
		BeginPaint(hWnd, &pt);
		parser(SrcFilePath);
		EndPaint(hWnd, &pt);
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
	}
}
