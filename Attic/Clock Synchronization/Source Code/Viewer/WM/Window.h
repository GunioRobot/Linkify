#pragma once


#include <map>
#include "Exception.h"
#include "Menu.h"
#include "MenuItem.h"
#include "Object.h"
#include "String.h"
#include "Widget.h"


namespace WM {
    class Window: public Object {
    private:
        static struct State {
            Window* instance;
            SHACTIVATEINFO sip;
            bool exceptionCaught;

            State() : instance(NULL), exceptionCaught(false) {
                ZeroMemory(&sip, sizeof(sip));
                sip.cbSize = sizeof(sip);
            }
        };


        static std::map<HWND, ref<State>> _windows;


        static LRESULT CALLBACK genericHandler(
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
        ref<Menu> _menuBar;


    public:
        Window(ref<String> title, ref<String> className):
            _handle(NULL), _menuBar(NULL)
        {
            HWND window = FindWindow(className->c_str(), title->c_str());

            if (window != NULL) {
                SetForegroundWindow(window);
                throw Exception(S("Duplicate window instance."));
            }
            else {
                WNDCLASS windowClass;

                ZeroMemory(&windowClass, sizeof(WNDCLASS));
                windowClass.style = CS_HREDRAW | CS_VREDRAW;
                windowClass.lpfnWndProc = Window::genericHandler;
                windowClass.hInstance = GetModuleHandle(NULL);
                windowClass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
                windowClass.lpszClassName = className->c_str();
                
                if (RegisterClass(&windowClass) == 0) {
                    Exception::throwLastError();
                }

                _handle = CreateWindow(className->c_str(), title->c_str(),
                    WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                    CW_USEDEFAULT, NULL, NULL, GetModuleHandle(NULL), NULL);

                if ((_handle == NULL) || !SHInitExtraControls()) {
                    Exception::throwLastError();
                }

                _windows[_handle]->instance = this;
            }
        }


        ~Window() {
            _windows.erase(_handle);
        }


        virtual void add(ref<Widget> widget, size_t left, size_t top) {
            widget->onAddTo(noref this, left, top);
        }


        virtual void close() {
            SendMessage(getHandle(), WM_CLOSE, 0, 0);
        }


        virtual void enableMenuBar(ref<Menu> menu) {
            if (menu->getItemCount() > 2) {
                throw Exception(S("Too many items in the menu bar."));
            }
            
            SHMENUBARINFO info;

            ZeroMemory(&info, sizeof(SHMENUBARINFO));
            info.cbSize = sizeof(SHMENUBARINFO);
            info.hwndParent = getHandle();
            info.dwFlags = SHCMBF_HMENU | SHCMBF_HIDESIPBUTTON;
            info.nToolBarId = (UINT) menu->getHandle();
            info.hInstRes = GetModuleHandle(NULL);
            
            if (!SHCreateMenuBar(&info)) {
                Exception::throwLastError();
            }

            _menuBar = menu;
        }


        virtual HWND getHandle() {
            return _handle;
        }


        virtual void open(int mode) {
            ShowWindow(getHandle(), mode);

            if (UpdateWindow(getHandle()) == 0) {
                Exception::throwLastError();
            }
        }


    protected:
        virtual void onChoose(ref<MenuItem> item) {
        }


        virtual void onResizeLandscape(size_t width, size_t height) {
        }

        
        virtual void onResizePortait(size_t width, size_t height) {
        }


        virtual void onResizeSquare(size_t width, size_t height) {
        }


    private:
        void handleCommand(WPARAM wParam, LPARAM lParam) {
            WORD notifyCode = HIWORD(wParam);
            WORD id = LOWORD(wParam);
            HWND handle = (HWND) lParam;

            if ((notifyCode == 0) && (id != 0) && (_menuBar != NULL)) {
                onChoose(_menuBar->getItemById(id));
            }
        }


        void handleResize(WPARAM wParam, LPARAM lParam) {
            size_t width = LOWORD(lParam);
            size_t height = HIWORD(lParam);

            switch (DRA::GetDisplayMode()) {
            case DRA::Landscape:
                onResizeLandscape(width, height);
                break;
            case DRA::Portrait:
                onResizePortait(width, height);
                break;
            case DRA::Square:
                onResizeSquare(width, height);
                break;
            }
        }


        LRESULT handler(UINT message, WPARAM wParam, LPARAM lParam) {
            switch (message) {
            case WM_COMMAND:
                handleCommand(wParam, lParam);
                break;
            case WM_DESTROY:
                PostQuitMessage(EXIT_SUCCESS);
                break;
            case WM_SIZE:
                handleResize(wParam, lParam);
                break;
            default:
                return DefWindowProc(getHandle(), message, wParam, lParam);
            }
            
            return 0;
        }
    };
}
