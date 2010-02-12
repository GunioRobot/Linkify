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
    
    
    void ErrorMessageBox(ref<tstring> message);
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
        HINSTANCE _handle;


    public:
        Application(HINSTANCE handle) : _handle(handle) {
        }


        virtual int start(int windowShowMode) {
            MSG message;
            onStart(windowShowMode);
            
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


        virtual HINSTANCE getHandle() {
            return _handle;
        }


    protected:
        virtual void onStart(int windowShowMode) {
        }
    };


    class MenuItem {
    private:
        ref<tstring> _caption;


    public:
        MenuItem(ref<tstring> caption) : _caption(caption) {
        }


        virtual ref<tstring> getCaption() {
            return _caption;
        }
    };
    
    
    class Menu {
    private:
        HMENU _handle;


    public:
        Menu() : _handle(CreateMenu()) {
            if (_handle == NULL) {
                throw Exception(GetLastErrorMessage());
            }
        }


        virtual void addItem(ref<MenuItem> item) {
            const TCHAR* caption = item->getCaption()->c_str();

            if (InsertMenu(_handle, -1, MF_BYPOSITION, 0, caption) == 0) {
                throw Exception(GetLastErrorMessage());
            }
        }
        
        
        virtual HMENU getHandle() {
            return _handle;
        }
    };


    class Window {
    private:
        static struct State {
            Window* instance;
            SHACTIVATEINFO sip;
            bool exceptionCaught;

            State() : instance(NULL), exceptionCaught(false) {
                memset(&sip, 0, sizeof(sip));
                sip.cbSize = sizeof(sip);
            }
        };

    private:
        static std::map<HWND, ref<State>> _windows;


        static LRESULT CALLBACK handler(
            HWND handle,
            UINT message,
            WPARAM wParam,
            LPARAM lParam)
        {
            if (_windows.find(handle) == _windows.end()) {
                _windows[handle] = new State();
            }
            
            ref<State> state = _windows[handle];

            switch (message) {
            case WM_ACTIVATE:
                SHHandleWMActivate(handle, wParam, lParam, &state->sip, FALSE);
                break;
            case WM_CREATE:
                break;
            case WM_SETTINGCHANGE:
                SHHandleWMSettingChange(handle, wParam, lParam, &state->sip);
                break;
            default:
                if (!state->exceptionCaught && (state->instance != NULL)) {
                    try {
                        return state->instance->handler(message, wParam, lParam);
                    }
                    catch (Exception exception) {
                        state->exceptionCaught = true;

                        ErrorMessageBox(exception.getMessage());
                        PostQuitMessage(EXIT_FAILURE);
                    }
                }
                
                return DefWindowProc(handle, message, wParam, lParam);
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

                memset(&windowClass, 0, sizeof(WNDCLASS));
                windowClass.style = CS_HREDRAW | CS_VREDRAW;
                windowClass.lpfnWndProc = Window::handler;
                windowClass.hInstance = GetModuleHandle(NULL);
                windowClass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
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

                _windows[_handle]->instance = this;
            }
        }


        ~Window() {
            _windows.erase(_handle);
        }


        virtual void addMenuBar(ref<Menu> menu) {
            SHMENUBARINFO info;

            memset(&info, 0, sizeof(SHMENUBARINFO));
            info.cbSize = sizeof(SHMENUBARINFO);
            info.hwndParent = getHandle();
            info.dwFlags = SHCMBF_HMENU | SHCMBF_HIDESIPBUTTON;
            info.nToolBarId = (UINT) menu->getHandle();
            info.hInstRes = GetModuleHandle(NULL);
            
            if (!SHCreateMenuBar(&info)) {
                throw Exception(GetLastErrorMessage());
            }
        }


        virtual HWND getHandle() {
            return _handle;
        }


        virtual void show(int mode) {
            if (getHandle() != NULL) {
                ShowWindow(getHandle(), mode);

                if (UpdateWindow(getHandle()) == 0) {
                    throw Exception(GetLastErrorMessage());
                }
            }
        }


    private:
        LRESULT handler(UINT message, WPARAM wParam, LPARAM lParam) {
            switch (message) {
            case WM_DESTROY:
                PostQuitMessage(EXIT_SUCCESS);
                break;
            default:
                return DefWindowProc(getHandle(), message, wParam, lParam);
            }
            
            return 0;
        }
    };
}
