// Order is significant:
#include "resourceppc.h"
#include "stdafx.h"
#include <commctrl.h>


HINSTANCE _application;
HWND _menuBar;


static INT_PTR CALLBACK aboutDialogBox(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG: {
            // Create a Done button and size it.  
            SHINITDLGINFO info;
            info.dwMask = SHIDIM_FLAGS;
            info.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN | SHIDIF_EMPTYMENU;
            info.hDlg = handle;
            SHInitDialog(&info);
        }
        return (INT_PTR) TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            EndDialog(handle, LOWORD(wParam));
            return TRUE;
        }
        break;
    case WM_CLOSE:
        EndDialog(handle, message);
        return TRUE;
    }
    
    return (INT_PTR) FALSE;
}


static LRESULT CALLBACK mainWindow(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
    static SHACTIVATEINFO shellActivate;
    
    switch (message) {
    // Process the application menu:
    case WM_COMMAND: {
        int menuId = LOWORD(wParam);
        
        // Parse the menu selections:
        switch (menuId) {
        case IDM_HELP_ABOUT:
            DialogBox(_application, (LPCTSTR) IDD_ABOUTBOX, handle, aboutDialogBox);
            break;
        case IDM_OK:
            SendMessage(handle, WM_CLOSE, 0, 0);				
            break;
        default:
            return DefWindowProc(handle, message, wParam, lParam);
        }
        break;
    }
    case WM_CREATE:
        SHMENUBARINFO info;
        
        memset(&info, 0, sizeof(SHMENUBARINFO));
        info.cbSize = sizeof(SHMENUBARINFO);
        info.hwndParent = handle;
        info.nToolBarId = IDR_MENU;
        info.hInstRes = _application;
        
        if (!SHCreateMenuBar(&info)) {
            _menuBar = NULL;
        }
        else {
            _menuBar = info.hwndMB;
        }
        
        // Initialize the shell activate info structure.
        memset(&shellActivate, 0, sizeof(shellActivate));
        shellActivate.cbSize = sizeof(shellActivate);
        
        CreateWindow( 
            L"BUTTON",   // Predefined class; Unicode assumed. 
            L"OK",       // Button text. 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles. 
            10,         // x position. 
            10,         // y position. 
            100,        // Button width.
            100,        // Button height.
            handle,       // Parent window.
            NULL,       // No menu.
            _application, 
            NULL);      // Pointer not needed.        
        
        break;
        // Paint the main window:
    case WM_PAINT: {
        PAINTSTRUCT paint;
        HDC context = BeginPaint(handle, &paint);
        
        // TODO: Add any drawing code here.
        
        EndPaint(handle, &paint);
        break;
    }
    // Post a quit message and return.
    case WM_DESTROY:
        CommandBar_Destroy(_menuBar);
        PostQuitMessage(0);
        break;
    case WM_ACTIVATE:
        // Notify shell of our activate message.
        SHHandleWMActivate(handle, wParam, lParam, &shellActivate, FALSE);
        break;
    case WM_SETTINGCHANGE:
        SHHandleWMSettingChange(handle, wParam, lParam, &shellActivate);
        break;
    default:
        return DefWindowProc(handle, message, wParam, lParam);
    }
    
    return 0;
}


static ATOM registerWindowClass(HINSTANCE application, LPTSTR windowClassName) {
	WNDCLASS windowClass;
    
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = mainWindow;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = application;
	windowClass.hIcon = LoadIcon(application, MAKEINTRESOURCE(IDI_VIEWER));
	windowClass.hCursor = 0;
	windowClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = windowClassName;
    
	return RegisterClass(&windowClass);
}


static BOOL initializeApplication(HINSTANCE application, int showMode) {
    const size_t MAX_LOADSTRING = 100;
    TCHAR titleBarText[MAX_LOADSTRING];
    TCHAR windowClassName[MAX_LOADSTRING];
    HWND window;
    
    // Save instance handle.
    _application = application;
    
    // This should be called once during the application's initialization to
    // initialize any of the device specific controls, e.g. CAPEDIT and SIPPREF.
    SHInitExtraControls();
    
    LoadString(application, IDS_APP_TITLE, titleBarText, MAX_LOADSTRING); 
    LoadString(application, IDS_APP_WND_CLASS, windowClassName, MAX_LOADSTRING);
    
    // If it is already running, then focus on the window, and exit.
    window = FindWindow(windowClassName, titleBarText);	
    
    if (window) {
        // Set focus to foremost child window. The "| 0x00000001" is used to
        // bring any owned windows to the foreground and activate them.
        SetForegroundWindow((HWND)((ULONG) window | 0x00000001));
        return FALSE;
    } 
    
    if (!registerWindowClass(application, windowClassName)) {
    	return FALSE;
    }
    
    window = CreateWindow(windowClassName, titleBarText, WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, application, NULL);
    
    if (!window) {
        return FALSE;
    }
    
    // When the main window is created using CW_USEDEFAULT the height of the
    // menubar (if one is created is not taken into account). So resize the
    // window after creating it if a menubar is present.
    if (_menuBar) {
        RECT windowArea;
        RECT menuBarArea;
        
        GetWindowRect(window, &windowArea);
        GetWindowRect(_menuBar, &menuBarArea);
        windowArea.bottom -= (menuBarArea.bottom - menuBarArea.top);
	    
        MoveWindow(window, windowArea.left, windowArea.top,
            windowArea.right - windowArea.left, windowArea.bottom - windowArea.top, FALSE);
    }
    
    ShowWindow(window, showMode);
    UpdateWindow(window);

    return TRUE;
}


int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPTSTR commandLine, int showMode) {
	// Perform application initialization:
	if (!initializeApplication(instance, showMode)) {
		return FALSE;
	}
    
	HACCEL accelerators = LoadAccelerators(instance, MAKEINTRESOURCE(IDS_APP_WND_CLASS));
	MSG message;
    
	// Main message loop:
	while (GetMessage(&message, NULL, 0, 0)) {
		if (!TranslateAccelerator(message.hwnd, accelerators, &message)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
    
	return (int) message.wParam;
}
