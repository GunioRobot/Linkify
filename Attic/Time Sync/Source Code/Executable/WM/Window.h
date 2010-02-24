#pragma once


#include <map>
#include <vector>
#include "Application.h"
#include "Exception.h"
#include "Menu.h"
#include "MenuItem.h"
#include "Object.h"
#include "String.h"
#include "Widget.h"


namespace WM {
    // TODO: Implement removal of child widgets.
    // TODO: Implement removal of the menu bar, and check if one has been added
    // already.
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
                        Application::exit(exception);
                    }
                }
                
                return DefWindowProc(handle, message, wParam, lParam);
            }
            
            return 0;
        }


    private:
        HWND _handle;
        ref<Menu> _menuBar;
        HWND _menuBarWindowHandle;
        std::vector<ref<Widget>> _widgets;


    public:
        Window(ref<String> title, ref<String> className):
            _handle(NULL), _menuBar(NULL), _menuBarWindowHandle(NULL)
        {
            HWND window = FindWindow(className->c_str(), title->c_str());

            if (window != NULL) {
                SetForegroundWindow(window);
                throw Exception(S("Duplicate window instance."));
            }
            
            WNDCLASS windowClass;

            ZeroMemory(&windowClass, sizeof(WNDCLASS));
            windowClass.style = CS_HREDRAW | CS_VREDRAW;
            windowClass.lpfnWndProc = handler;
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


        ~Window() {
            _windows.erase(_handle);
        }


        virtual void add(ref<Widget> widget) {
            if (SetParent(widget->getHandle(), getHandle()) == NULL) {
                Exception::throwLastError();
            }
            
            ShowWindow(widget->getHandle(), SW_SHOWNORMAL);
            _widgets.push_back(widget);
        }


        virtual void close() {
            SendMessage(getHandle(), WM_CLOSE, 0, 0);
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


        virtual void setMenuBar(ref<Menu> menu) {
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
            _menuBarWindowHandle = info.hwndMB;
        }


    protected:
        virtual void onChoose(ref<MenuItem> item) {
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
            size_t width = DRA::UNSCALEX(LOWORD(lParam));
            size_t height = DRA::UNSCALEY(HIWORD(lParam));

            if (_menuBarWindowHandle != NULL) {
                RECT size;
                
                if (!GetWindowRect(_menuBarWindowHandle, &size)) {
                    Exception::throwLastError();
                }

                height -= DRA::UNSCALEY(size.bottom - size.top);
            }

            Size area(width, height);

            for (size_t i = 0; i < _widgets.size(); ++i) {
                _widgets[i]->onOwnerResize(area);
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
