// Order is significant:
#include "stdafx.h"
#include <commctrl.h>

// Order is not significant:
#include "ref.h"
#include "resources.h"
#include <exception>
#include <map>
#include <string>


namespace Win32 {
    typedef std::basic_string<TCHAR> tstring;
    
    
    // TODO: Check when the buffer is too small for the whole string.
    ref<tstring> LoadStringT(UINT id, HINSTANCE module = GetModuleHandle(NULL)) {
        const size_t BUFFER_SIZE = 128;
        TCHAR buffer[BUFFER_SIZE];
        int length = LoadString(module, id, buffer, BUFFER_SIZE);
        ref<tstring> string = (length == 0) ? NULL : new tstring(buffer);

        return string;
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
        ref<Win32::tstring> update = Win32::LoadStringT(IDS_SK_UPDATE);
        ref<Win32::tstring> exit = Win32::LoadStringT(IDS_SK_EXIT);
        InsertMenu(menuBar, -1, MF_BYPOSITION, IDS_SK_UPDATE, update->c_str()); 
        InsertMenu(menuBar, -1, MF_BYPOSITION, IDS_SK_EXIT, exit->c_str());
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
    ref<Win32::tstring> title = Win32::LoadStringT(IDS_TITLE);
    ref<Win32::tstring> className = Win32::LoadStringT(IDS_WINDOW_CLASS);
    HWND window = FindWindow(className->c_str(), title->c_str());
    
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
    windowClass.hCursor = NULL;
    windowClass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1); //(HBRUSH) GetStockObject(WHITE_BRUSH);
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = className->c_str();
    
    if (RegisterClass(&windowClass) == 0) {
        return FALSE;
    }

    window = CreateWindow(className->c_str(), title->c_str(), WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL,
        application, NULL);
    
    if (!window) {
        return FALSE;
    }
    
    SHInitExtraControls();
    UpdateWindow(window);

    return TRUE;
}


class Application {
public:
    Application() {
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
};


class Window {
public:
    static class AlreadyExistsError : public std::exception {
    };
    

    static class ClassRegistrationError : public std::exception {
    };
    

    static class CreationError : public std::exception {
    };
    

private:
    static std::map<HWND, Window*> _windows;


    static LRESULT CALLBACK handler(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
        return _windows[handle]->handler(message, wParam, lParam);
    }


public:
    Window(ref<Win32::tstring> title, ref<Win32::tstring> className) {
        _handle = FindWindow(className->c_str(), title->c_str());

        if (_handle != NULL) {
            SetForegroundWindow(_handle);
            throw AlreadyExistsError();
        }
        
        WNDCLASS windowClass;
        HINSTANCE module = GetModuleHandle(NULL);

        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = &Window::handler;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;
        windowClass.hInstance = module;
        windowClass.hIcon = NULL;
        windowClass.hCursor = NULL;
        windowClass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
        windowClass.lpszMenuName = NULL;
        windowClass.lpszClassName = className->c_str();
        
        if (RegisterClass(&windowClass) == 0) {
            throw ClassRegistrationError();
        }
        
        _handle = CreateWindow(className->c_str(), title->c_str(), WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            NULL, NULL, module, NULL);

        if (_handle == NULL) {
            throw CreationError();
        }
        
        _windows[_handle] = this;
        SHInitExtraControls();
    }


    ~Window() {
        _windows.erase(_handle);
    }


    virtual void show(int mode) {
        ShowWindow(_handle, mode);
        UpdateWindow(_handle);
    }


private:
    LRESULT handler(UINT message, WPARAM wParam, LPARAM lParam) {
        return DefWindowProc(_handle, message, wParam, lParam);
    }


    HWND _handle;
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
