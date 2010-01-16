// Order is significant:
#include "stdafx.h"
#include <commctrl.h>

// Order is not significant:
#include "resources.h"
#include <string>


namespace Win32 {
    typedef std::basic_string<TCHAR> tstring;
    
    
    // TODO: Add caching?
    // TODO: Add error checking.
    // TODO: Check when the buffer is too small for the whole string?
    tstring LoadStringT(UINT id, HINSTANCE module = GetModuleHandle(NULL)) {
        const size_t BUFFER_SIZE = 128;
        TCHAR buffer[BUFFER_SIZE];
        
        LoadString(module, id, buffer, BUFFER_SIZE);
        return tstring(buffer);
    }
}


static LRESULT CALLBACK mainWindow(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
    static SHACTIVATEINFO shellActivate;
    
    switch (message) {
    case WM_COMMAND: {
        int menuId = LOWORD(wParam);
        
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
        memset(&shellActivate, 0, sizeof(shellActivate));
        shellActivate.cbSize = sizeof(shellActivate);
        
        HINSTANCE application;
        application = GetModuleHandle(NULL);
        
        HMENU menuBar;
        menuBar = CreateMenu();
        
        {
        Win32::tstring update = Win32::LoadStringT(IDS_SK_UPDATE);
        Win32::tstring exit = Win32::LoadStringT(IDS_SK_EXIT);
        InsertMenu(menuBar, -1, MF_BYPOSITION, IDS_SK_UPDATE, update.c_str()); 
        InsertMenu(menuBar, -1, MF_BYPOSITION, IDS_SK_EXIT, exit.c_str());
        }

        SHMENUBARINFO menuBarInfo;
        memset(&menuBarInfo, 0, sizeof(SHMENUBARINFO));
        menuBarInfo.cbSize = sizeof(SHMENUBARINFO);
        menuBarInfo.hwndParent = handle;
        menuBarInfo.dwFlags = SHCMBF_HMENU | SHCMBF_HIDESIPBUTTON;
        menuBarInfo.nToolBarId = (UINT) menuBar;
        menuBarInfo.hInstRes = application;
        
        SHCreateMenuBar(&menuBarInfo);

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
            application, 
            NULL);      // Pointer not needed.
        
        break;
    case WM_PAINT: {
        // Paint the main window:
        PAINTSTRUCT paint;
        HDC context = BeginPaint(handle, &paint);
        
        // TODO: Add any drawing code here.
        
        EndPaint(handle, &paint);
        break;
    }
    case WM_DESTROY:
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


static BOOL initializeApplication(HINSTANCE application, int showMode) {
    Win32::tstring title = Win32::LoadStringT(IDS_TITLE);
    Win32::tstring windowClassName = Win32::LoadStringT(IDS_WINDOW_CLASS);
    HWND window = FindWindow(windowClassName.c_str(), title.c_str());
    
    if (window != NULL) {
        SetForegroundWindow(window);
        return FALSE;
    }
    
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
    windowClass.lpszClassName = windowClassName.c_str();
    
    if (!RegisterClass(&windowClass)) {
        return FALSE;
    }

    window = CreateWindow(windowClassName.c_str(), title.c_str(), WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, application, NULL);
    
    if (!window) {
        return FALSE;
    }
    
    SHInitExtraControls();
    ShowWindow(window, showMode);
    UpdateWindow(window);

    return TRUE;
}


class Application {
public:
    Application(HINSTANCE instance): _instance(instance) {
    }


    virtual int execute(LPTSTR commandLine, int windowShowMode) {
        if (!onStart(commandLine, windowShowMode)) {
            return EXIT_FAILURE;
        }

        MSG message;
        
        while (BOOL result = GetMessage(&message, NULL, 0, 0)) {
            if (result == -1) {
                return EXIT_FAILURE;
            }
            else {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }
        
        return (int) message.wParam;
    }


protected:
    virtual BOOL onStart(LPTSTR commandLine, int windowShowMode) {
        return TRUE;
    }


private:
    HINSTANCE _instance;
};


int WINAPI WinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    LPTSTR commandLine,
    int windowShowMode)
{
    if (!initializeApplication(instance, windowShowMode)) {
        return FALSE;
    }
    
    MSG message;
    
    while (GetMessage(&message, NULL, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    
    return (int) message.wParam;
}
