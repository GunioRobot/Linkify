#pragma once


// Order is significant:
#include "stdafx.h"
#include <commctrl.h>

// Order is not significant:
#include <exception>
#include <map>
#include <string>
#include "ref.h"


namespace Win32 {
    typedef std::basic_string<TCHAR> tstring;
    
    
    ref<tstring> LoadStringT(UINT id, HINSTANCE module = GetModuleHandle(NULL));


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


        static LRESULT CALLBACK genericHandler(
            HWND handle,
            UINT message,
            WPARAM wParam,
            LPARAM lParam)
        {
            return _windows[handle]->handler(message, wParam, lParam);
        }


    private:
        HWND _handle;


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
            
            _handle = CreateWindow(className->c_str(), title->c_str(),
                WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                CW_USEDEFAULT, NULL, NULL, module, NULL);

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


    protected:
        virtual void onPaint() {
        }


    private:
        LRESULT handler(UINT message, WPARAM wParam, LPARAM lParam) {
            static SHACTIVATEINFO sipInfo;
            
            switch (message) {
            case WM_CREATE:
                memset(&sipInfo, 0, sizeof(sipInfo));
                sipInfo.cbSize = sizeof(sipInfo);
                break;
            case WM_PAINT:
                PAINTSTRUCT paint;
                HDC context;
                
                context = BeginPaint(_handle, &paint);
                onPaint();
                EndPaint(_handle, &paint);
                break;
            case WM_DESTROY:
                PostQuitMessage(EXIT_SUCCESS);
                break;
            case WM_ACTIVATE:
                SHHandleWMActivate(_handle, wParam, lParam, &sipInfo, FALSE);
                break;
            case WM_SETTINGCHANGE:
                SHHandleWMSettingChange(_handle, wParam, lParam, &sipInfo);
                break;
            default:
                return DefWindowProc(_handle, message, wParam, lParam);
            }
            
            return 0;
        }
    };
}
