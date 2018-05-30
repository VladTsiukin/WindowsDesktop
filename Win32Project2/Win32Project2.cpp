// Win32Project2.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Win32Project2.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


///////////////////////////////////////////////////////////////////
#define IDC_LB 123456777
#define ID_MESSAGE_DESTROYWINDOW WM_APP + 1
const int nTimerUnitsPerSecond = 10000000; // 1 second
int check = 0;

////////////////////////////////////////////////////////////////////

typedef struct {
	TCHAR endTime[256];
	TCHAR end2Time[256];
} TIMERINFO;

////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

unsigned __stdcall CheckCloseThread(void* pArguments)
{
	HWND hWnd = (HWND)pArguments;
	HWND hwndList = GetDlgItem(hWnd, IDC_LB);

	SYSTEMTIME lt;
	TIMERINFO timerInfo;

	DWORD dwWaitResult;

	int time = 10; // second

	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;

	HANDLE hEvent = OpenEvent(SYNCHRONIZE, FALSE, TEXT("MyTask2Event"));

	if (hEvent == NULL)
	{
		MessageBox(hWnd, L"Could not Open Event. Run Win32Project1!", TEXT("App2"), MB_OK);
		return 1;
	}

	dwWaitResult = WaitForSingleObject(
		hEvent,       // event handle
		INFINITE);    // indefinite wait

	switch (dwWaitResult)
	{
		// Event object was signaled
	case WAIT_OBJECT_0:

		GetLocalTime(&lt);
		swprintf(timerInfo.endTime, 256, TEXT("Close App1 = %02d min-%02d sec-%d milliseconds"),
			lt.wMinute, lt.wSecond, lt.wMilliseconds);
		SendMessage(hwndList, LB_ADDSTRING, 255, (LPARAM)timerInfo.endTime);
		break;

		// An error occurred
	default:
		return 1;
	}

	///////////////////// for close App 2:

	liDueTime.QuadPart = -(time * nTimerUnitsPerSecond); // ... seconds (by minus 100ns)

														 // Create an unnamed waitable timer.
	hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	if (NULL == hTimer) return 1;

	// Set a timer to wait for 10 seconds.
	if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0)) return 2;

	// Wait for the timer.
	if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0) return 3;

	//////////////////////for close app 2:

	PostMessage(hWnd, ID_MESSAGE_DESTROYWINDOW, 0, 0);

	//////////////////////

	CloseHandle(hEvent);
	CloseHandle(hTimer);

	_endthreadex(0);
	return 0;
}

/////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////

// Thread 2 worker:
unsigned __stdcall While5Second(void* pArguments)
{
	HWND hWnd = (HWND)pArguments;
	HWND hwndList = GetDlgItem(hWnd, IDC_LB);

	SYSTEMTIME lt;
	TIMERINFO timerInfo;
	int time = 5; // second

	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;

	liDueTime.QuadPart = -(time * nTimerUnitsPerSecond); // ... seconds (by minus 100ns)

														 // Create an unnamed waitable timer.
	while (check == 0)
	{
		hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
		if (NULL == hTimer) return 1;

		// Set a timer to wait for 5 seconds.
		if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0)) return 2;

		// Wait for the timer.
		if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0) return 3;

		GetLocalTime(&lt);
		swprintf(timerInfo.endTime, 256, TEXT("While 5 secnd work... Time = %02d min-%02d sec-%d milliseconds"),
			lt.wMinute, lt.wSecond, lt.wMilliseconds);
		SendMessage(hwndList, LB_ADDSTRING, 255, (LPARAM)timerInfo.endTime);

		CloseHandle(hTimer);
	}
	
	CloseHandle(hTimer);
	_endthreadex(0);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32PROJECT2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT2));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32PROJECT2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	static HWND hListBox;
	HANDLE hThread, hThread2;
	unsigned threadID, threadID2;
	TIMERINFO timerInfo;
	RECT rc;
	SYSTEMTIME lt;

    switch (message)
    {
    case WM_COMMAND:
        {
			int wmId = LOWORD(wParam);
			int lmId = LOWORD(lParam);
            // Parse the menu selections:
            switch (wmId)
            {			
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_CREATE:
	{
		GetWindowRect(hWnd, &rc);

		if (!(hListBox = CreateWindowEx(0, L"ListBox", L"Список",
			WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL,
			0, 0, rc.right, rc.bottom, hWnd, (HMENU)(IDC_LB),
			GetModuleHandle(NULL), NULL)))
		{
			MessageBox(NULL, L"Could not create listbox", TEXT("App2"), MB_OK);
		}

		////////// Create the thread for check close App1:  
		hThread = (HANDLE)_beginthreadex(NULL, 0, &CheckCloseThread, hWnd, 0, &threadID);

		////////// Create the thread for check close App1:  
		hThread2 = (HANDLE)_beginthreadex(NULL, 0, &While5Second, hWnd, 0, &threadID2);
	}
	break;
	case ID_MESSAGE_DESTROYWINDOW:

		GetLocalTime(&lt);
		swprintf(timerInfo.endTime, 256, TEXT("Close App2 = %02d min-%02d sec-%d milliseconds"),
			lt.wMinute, lt.wSecond, lt.wMilliseconds);

		/////////// For stop 2 thread(While5Second func):
		check = 1;
		///////////

		MessageBox(NULL, timerInfo.endTime, TEXT("Close App2:"), MB_OK);

		DestroyWindow(hWnd);

		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}