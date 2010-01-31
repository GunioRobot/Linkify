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


class Application {
public:
    Application() {
    }


    virtual int start(LPTSTR commandLine, int windowShowMode) {
        onStart(commandLine, windowShowMode);
        
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
    virtual void onStart(LPTSTR commandLine, int windowShowMode) {
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


    static LRESULT CALLBACK genericHandler(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
        return _windows[handle]->handler(handle, message, wParam, lParam);
    }


    static HWND setup(Window* window, ref<Win32::tstring> title, ref<Win32::tstring> className) {
        HWND handle = FindWindow(className->c_str(), title->c_str());

        if (handle != NULL) {
            SetForegroundWindow(handle);
            throw AlreadyExistsError();
        }
        
        WNDCLASS windowClass;
        HINSTANCE module = GetModuleHandle(NULL);

        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = Window::genericHandler;
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
        
        handle = CreateWindow(className->c_str(), title->c_str(), WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            NULL, NULL, module, NULL);

        if (handle == NULL) {
            throw CreationError();
        }
        
        _windows[handle] = window;
        SHInitExtraControls();

        return handle;
    }


public:
    Window(UINT titleId, UINT classNameId) {
        ref<Win32::tstring> title = Win32::LoadStringT(titleId);
        ref<Win32::tstring> className = Win32::LoadStringT(classNameId);

        _handle = setup(this, title, className);
    }


    Window(ref<Win32::tstring> title, ref<Win32::tstring> className) {
        _handle = setup(this, title, className);
    }


    ~Window() {
        _windows.erase(_handle);
    }


    virtual void show(int mode) {
        ShowWindow(_handle, mode);
        UpdateWindow(_handle);
    }


private:
    LRESULT handler(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
        static SHACTIVATEINFO sipInfo;
        
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
            memset(&sipInfo, 0, sizeof(sipInfo));
            sipInfo.cbSize = sizeof(sipInfo);
            
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
            SHHandleWMActivate(handle, wParam, lParam, &sipInfo, FALSE);
            break;
        case WM_SETTINGCHANGE:
            SHHandleWMSettingChange(handle, wParam, lParam, &sipInfo);
            break;
        default:
            return DefWindowProc(handle, message, wParam, lParam);
        }
        
        return 0;
    }


    HWND _handle;
};


std::map<HWND, Window*> Window::_windows;


class Viewer : public Application, Window {
public:
    Viewer() : Window(IDS_TITLE, IDS_WINDOW_CLASS) {
    }


protected:
    virtual void onStart(LPTSTR commandLine, int windowShowMode) {
        show(windowShowMode);
    }
};


int WINAPI WinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    LPTSTR commandLine,
    int windowShowMode)
{
    try {
        return Viewer().start(commandLine, windowShowMode);
    }
    catch (std::exception error) {
        return EXIT_FAILURE;
    }
}
