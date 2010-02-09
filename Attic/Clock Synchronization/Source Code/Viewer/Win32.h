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
    
    
    ref<tstring> GetLastErrorMessage();
    ref<tstring> LoadStringT(UINT id, HINSTANCE module = GetModuleHandle(NULL));


    class Exception : public std::exception {
    private:
        ref<tstring> _message;


    public:
        Exception(TCHAR* message) : _message(new tstring(message)) {
        }


        Exception(ref<tstring> message) : _message(message) {
        }


        virtual ref<tstring> getMessage() {
            return _message;
        }
    };


    class Application {
    public:
        Application() {
        }


        virtual int start(int windowShowMode) {
            onStart(windowShowMode);
            
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
        virtual void onStart(int windowShowMode) {
        }
    };


    class Window {
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
        Window(ref<tstring> title, ref<tstring> className) {
            _handle = FindWindow(className->c_str(), title->c_str());

            if (_handle != NULL) {
                SetForegroundWindow(_handle);
                PostQuitMessage(EXIT_FAILURE);
            }
            else {
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
                    throw Exception(GetLastErrorMessage());
                }
                
                _handle = CreateWindow(className->c_str(), title->c_str(),
                    WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                    CW_USEDEFAULT, NULL, NULL, module, NULL);

                if ((_handle == NULL) || !SHInitExtraControls()) {
                    throw Exception(GetLastErrorMessage());
                }
            }

            _windows[_handle] = this;
        }


        ~Window() {
            _windows.erase(_handle);
        }


        virtual void show(int mode) {
            ShowWindow(getHandle(), mode);
            UpdateWindow(getHandle());
        }


    protected:
        virtual HWND getHandle() {
            return _handle;
        }


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
                
                context = BeginPaint(getHandle(), &paint);
                onPaint();
                EndPaint(getHandle(), &paint);
                break;
            case WM_DESTROY:
                PostQuitMessage(EXIT_SUCCESS);
                break;
            case WM_ACTIVATE:
                SHHandleWMActivate(getHandle(), wParam, lParam, &sipInfo, FALSE);
                break;
            case WM_SETTINGCHANGE:
                SHHandleWMSettingChange(getHandle(), wParam, lParam, &sipInfo);
                break;
            default:
                return DefWindowProc(getHandle(), message, wParam, lParam);
            }
            
            return 0;
        }
    };
}
