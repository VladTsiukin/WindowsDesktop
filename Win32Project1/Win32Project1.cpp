// Win32Project1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Win32Project1.h"

#define MAX_LOADSTRING 100
#define IDC_LB 1234567
const int nTimerUnitsPerSecond = 10000000; // 1 second

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HANDLE hEvent;                                  // the event by object kernel


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


////////////////////////////////////////////////////////////////////////////////

typedef struct {
	TCHAR startTime[256];
	TCHAR start2Time[256];
	TCHAR endTime[256];
} TIMERINFO;

//////////////////////////////////////////////////////////////////////////////////////////

// Thread 2 worker:
unsigned __stdcall SecondThreadFunc(void* pArguments)
{
	
	int time = 5; // second

	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;

	liDueTime.QuadPart = -(time * nTimerUnitsPerSecond); // ... seconds (by minus 100ns)
	
	// Create an unnamed waitable timer.
	hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	if (NULL == hTimer) return 1;

	// Set a timer to wait for 5 seconds.
	if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0)) return 2;

	// Wait for the timer.
	if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0) return 3;

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
    LoadStringW(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32PROJECT1);
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
	HANDLE hThread;
	unsigned threadID;
	TIMERINFO timerInfo;
	RECT rc;
	SYSTEMTIME lt;

	///////////for procees 2:

	STARTUPINFO         si;
	PROCESS_INFORMATION  pi;
	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.cb = sizeof(si);

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case IDM_FILE_CREATETHREAD:

				////////////// start App1:
				GetLocalTime(&lt);
				swprintf(timerInfo.startTime, 256, TEXT("Start App1 = %02d min-%02d sec-%d milliseconds"),
					lt.wMinute, lt.wSecond, lt.wMilliseconds);
				SendMessage(hListBox, LB_ADDSTRING, 255, (LPARAM)timerInfo.startTime);

				//////////////

				// Create the second thread.  
				hThread = (HANDLE)_beginthreadex(NULL, 0, &SecondThreadFunc, NULL, 0, &threadID);

				// Wait until second thread terminates.
				WaitForSingleObject(hThread, INFINITE);

				///////////// Start App2:

				GetLocalTime(&lt);
				swprintf(timerInfo.start2Time, 256, TEXT("Start App2 = %02d min-%02d sec-%d milliseconds"),
					lt.wMinute, lt.wSecond, lt.wMilliseconds);
				SendMessage(hListBox, LB_ADDSTRING, 255, (LPARAM)timerInfo.start2Time);

				//////////// Create process 2:

				if (!CreateProcess(
					TEXT("../Debug/Win32Project2.exe"),
					NULL, DEFAULT_SECURITY, DEFAULT_SECURITY,
					FALSE, 0, NULL, NULL, &si, &pi))
					MessageBoxEx(NULL, TEXT("Can not create process!"), TEXT("ERORR"), MB_OK, 0);

				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
				CloseHandle(hThread);
				break;
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

		if (!(hListBox = CreateWindowEx(0, L"ListBox", L"Listbox",
			WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL,
			0, 0, rc.right, rc.bottom, hWnd, (HMENU)(IDC_LB),
			GetModuleHandle(NULL), NULL)))
		{
			MessageBox(NULL, L"Could not create listbox", TEXT("App1"), MB_OK);
		}

		// create event for 'app2' in busy state:
		hEvent = CreateEvent(
			NULL,               // default security attributes
			TRUE,               // manual-reset event
			FALSE,              // initial state is nonsignaled
			TEXT("MyTask2Event")  // object name
		);

		if (hEvent == NULL)
		{
			MessageBox(NULL, L"Could not Create Event", TEXT("App1"), MB_OK);
		}

	}
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...

			//
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_CLOSE:
		// print close time:
		GetLocalTime(&lt);
		swprintf(timerInfo.endTime, 256, TEXT("Close App1 = %02d min-%02d sec-%d milliseconds"),
			lt.wMinute, lt.wSecond, lt.wMilliseconds);

		MessageBox(NULL, timerInfo.endTime, TEXT("Close App1:"), MB_OK);


		////////////// puts the event in the free state:

		SetEvent(hEvent);

		//////////////

		// Close window:
		DestroyWindow(hWnd);

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