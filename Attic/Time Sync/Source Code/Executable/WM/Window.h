#pragma once


#include <map>
#include <vector>
#include "Application.h"
#include "Exception.h"
#include "Menu.h"
#include "MenuItem.h"
#include "Widget.h"


namespace WM {
    class Window: public Widget {
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


    public:
        static void createClass(String className) {
            WNDCLASS windowClass;

            ZeroMemory(&windowClass, sizeof(WNDCLASS));
            windowClass.style = CS_HREDRAW | CS_VREDRAW;
            windowClass.lpfnWndProc = handler;
            windowClass.hInstance = NULL;
            windowClass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
            windowClass.lpszClassName = className.c_str();
            
            if (RegisterClass(&windowClass) == 0) {
                Exception::throwLastError();
            }
        }


        static bool exists(String className, String title) {
            HWND window = FindWindow(className.c_str(), title.c_str());

            if (window != NULL) {
                SetForegroundWindow(window);
                return true;
            }

            return false;
        }


    private:
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
        ref<Menu> _menuBar;
        HWND _menuBarWindowHandle;
        std::vector<ref<Widget>> _widgets;


    public:
        Window(String className, String title):
            Widget(className, title, WS_SYSMENU),
            _menuBar(NULL), _menuBarWindowHandle(NULL)
        {
            if (!SHInitExtraControls()) {
                Exception::throwLastError();
            }

            _windows[getHandle()]->instance = this;
        }


        ~Window() {
            _windows.erase(getHandle());
        }


        virtual void add(ref<Widget> widget) {
            widget->setParent(noref this);
            _widgets.push_back(widget);
        }


        virtual void close() {
            SendMessage(getHandle(), WM_CLOSE, 0, 0);
        }


        virtual Size getSize() {
            RECT area;
            size_t menuBarHeight = 0;
            
            if (!GetWindowRect(getHandle(), &area)) {
                Exception::throwLastError();
            }

            if (_menuBarWindowHandle != NULL) {
                RECT size;
                
                if (!GetWindowRect(_menuBarWindowHandle, &size)) {
                    Exception::throwLastError();
                }

                menuBarHeight = size.bottom - size.top;
            }
            
            Length width(DRA::UNSCALEX(area.right - area.left));
            Length height(DRA::UNSCALEY(area.bottom - area.top - menuBarHeight));

            return Size(width, height);
        }
        
        
        virtual void onChoose(ref<MenuItem> item) {
        }


        virtual void onResize() {
        }


        virtual void open(int mode) {
            ShowWindow(getHandle(), mode);

            if (UpdateWindow(getHandle()) == 0) {
                Exception::throwLastError();
            }
        }


        virtual void setMenuBar(ref<Menu> menu) {
            if (menu->getItemCount() > 2) {
                throw Exception(S("Too many items for a menu bar."));
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
            for (size_t i = 0; i < _widgets.size(); ++i) {
                _widgets[i]->onParentResize();
            }
            
            onResize();
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
