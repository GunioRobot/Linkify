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
    private:
        HINSTANCE _instance;


    public:
        Application(HINSTANCE instance) : _instance(instance) {
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
        virtual HINSTANCE getInstance() {
            return _instance;
        }


        virtual void onStart(int windowShowMode) {
        }
    };


    class Window {
    private:
        static std::map<HWND, Window*> _windows;
        static std::map<HWND, SHACTIVATEINFO> _sipInfos;


        static LRESULT CALLBACK genericHandler(
            HWND handle,
            UINT message,
            WPARAM wParam,
            LPARAM lParam)
        {
            switch (message) {
            case WM_ACTIVATE:
                SHHandleWMActivate(handle, wParam, lParam, &_sipInfos[handle], FALSE);
                break;
            case WM_CREATE:
                SHACTIVATEINFO sipInfo;
                memset(&sipInfo, 0, sizeof(sipInfo));
                sipInfo.cbSize = sizeof(sipInfo);
                _sipInfos[handle] = sipInfo;
                break;
            case WM_SETTINGCHANGE:
                SHHandleWMSettingChange(handle, wParam, lParam, &_sipInfos[handle]);
                break;
            default:
                if (_windows.find(handle) != _windows.end()) {
                    return _windows[handle]->handler(message, wParam, lParam);
                }
                else {
                    return DefWindowProc(handle, message, wParam, lParam);
                }
            }
            
            return 0;
        }


    private:
        HWND _handle;


    public:
        Window(ref<tstring> title, ref<tstring> className) : _handle(NULL) {
            HWND window = FindWindow(className->c_str(), title->c_str());

            if (window != NULL) {
                SetForegroundWindow(window);
                PostQuitMessage(EXIT_FAILURE);
            }
            else {
                WNDCLASS windowClass;

                windowClass.style = CS_HREDRAW | CS_VREDRAW;
                windowClass.lpfnWndProc = Window::genericHandler;
                windowClass.cbClsExtra = 0;
                windowClass.cbWndExtra = 0;
                windowClass.hInstance = GetModuleHandle(NULL);
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
                    CW_USEDEFAULT, NULL, NULL, GetModuleHandle(NULL), NULL);

                if ((_handle == NULL) || !SHInitExtraControls()) {
                    throw Exception(GetLastErrorMessage());
                }

                _windows[_handle] = this;
            }
        }


        ~Window() {
            _windows.erase(_handle);
            _sipInfos.erase(_handle);
        }


        virtual void show(int mode) {
            ShowWindow(getHandle(), mode);

            if (UpdateWindow(getHandle()) == 0) {
                throw Exception(GetLastErrorMessage());
            }
        }


    protected:
        virtual HWND getHandle() {
            return _handle;
        }


        virtual void onPaint() {
        }


    private:
        LRESULT handler(UINT message, WPARAM wParam, LPARAM lParam) {
            switch (message) {
            case WM_DESTROY:
                PostQuitMessage(EXIT_SUCCESS);
                break;
            case WM_PAINT:
                PAINTSTRUCT paint;
                HDC context;
                context = BeginPaint(getHandle(), &paint);
                onPaint();
                EndPaint(getHandle(), &paint);
                break;
            default:
                return DefWindowProc(getHandle(), message, wParam, lParam);
            }
            
            return 0;
        }
    };
}
