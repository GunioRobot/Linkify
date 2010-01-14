// Order is significant:
#include "stdafx.h"
#include <commctrl.h>

// Order is not significant:
#include "resources.h"
#include <string>


namespace Win32 {
    typedef std::basic_string<TCHAR> tstring;
    
    
    tstring LoadStringT(HINSTANCE module, UINT id) {
        // TODO: Add caching?
        // TODO: Add error checking.
        const size_t BUFFER_SIZE = 128;
        TCHAR buffer[BUFFER_SIZE];
        int length = LoadString(module, id, buffer, BUFFER_SIZE);

        return tstring(buffer);
    }
}


HINSTANCE _application;
HWND _menuBar;


static LRESULT CALLBACK mainWindow(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
    static SHACTIVATEINFO shellActivate;
    
    switch (message) {
    // Process the application menu:
    case WM_COMMAND: {
        int menuId = LOWORD(wParam);
        
        // Parse the menu selections:
        switch (menuId) {
        case IDS_SK_EXIT:
            SendMessage(handle, WM_CLOSE, 0, 0);
            break;
        case IDS_SK_UPDATE:
            break;
        default:
            return DefWindowProc(handle, message, wParam, lParam);
        }
        break;
    }
    case WM_CREATE:
        // Initialize the shell activate info structure.
        memset(&shellActivate, 0, sizeof(shellActivate));
        shellActivate.cbSize = sizeof(shellActivate);
        
        HMENU menuBar;
        menuBar = CreateMenu();
        
        {
        Win32::tstring update = Win32::LoadStringT(_application, IDS_SK_UPDATE);
        Win32::tstring exit = Win32::LoadStringT(_application, IDS_SK_EXIT);
        InsertMenu(menuBar, -1, MF_BYPOSITION, IDS_SK_UPDATE, update.c_str()); 
        InsertMenu(menuBar, -1, MF_BYPOSITION, IDS_SK_EXIT, exit.c_str());
        }

        SHMENUBARINFO menuBarInfo;
        memset(&menuBarInfo, 0, sizeof(SHMENUBARINFO));
        menuBarInfo.cbSize = sizeof(SHMENUBARINFO);
        menuBarInfo.hwndParent = handle;
        menuBarInfo.dwFlags = SHCMBF_HMENU | SHCMBF_HIDESIPBUTTON;
        menuBarInfo.nToolBarId = (UINT) menuBar;
        menuBarInfo.hInstRes = _application;
        
        if (!SHCreateMenuBar(&menuBarInfo)) {
            _menuBar = NULL;
        }
        else {
            _menuBar = menuBarInfo.hwndMB;
        }

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


static ATOM registerWindowClass(HINSTANCE application, Win32::tstring name) {
	WNDCLASS windowClass;
    
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = mainWindow;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = application;
	windowClass.hIcon = NULL;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	windowClass.lpszMenuName = 0;
    windowClass.lpszClassName = name.c_str();
    
	return RegisterClass(&windowClass);
}


static BOOL initializeApplication(HINSTANCE application, int showMode) {
    Win32::tstring title = Win32::LoadStringT(application, IDS_TITLE);
    Win32::tstring windowClass = Win32::LoadStringT(application, IDS_WINDOW_CLASS);
    HWND window;
    
    // Save instance handle.
    _application = application;
    
    // This should be called once during the application's initialization to
    // initialize any of the device specific controls, e.g. CAPEDIT and SIPPREF.
    SHInitExtraControls();
    
    // If it is already running, then focus on the window, and exit.
    window = FindWindow(windowClass.c_str(), title.c_str());
    
    if (window) {
        // Set focus to foremost child window. The "| 0x00000001" is used to
        // bring any owned windows to the foreground and activate them.
        SetForegroundWindow((HWND)((ULONG) window | 0x00000001));
        return FALSE;
    } 
    
    if (!registerWindowClass(application, windowClass)) {
    	return FALSE;
    }
    
    window = CreateWindow(windowClass.c_str(), title.c_str(), WS_VISIBLE,
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
    
	MSG message;
    
	// Main message loop:
	while (GetMessage(&message, NULL, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
    
	return (int) message.wParam;
}
