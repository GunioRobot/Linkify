#pragma once


// Order is significant:
#include "stdafx.h"
#include <commctrl.h>

// Order is not significant:
#include <exception>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "ref.h"


namespace Win32 {
    typedef std::basic_string<TCHAR> String;
    typedef std::basic_stringstream<TCHAR> StringStream;
    

    void ErrorMessageBox(ref<String> message);
    ref<String> GetLastErrorMessage();
    ref<String> LoadStringT(UINT id, HINSTANCE module = GetModuleHandle(NULL));


    class Object {
    };


    class Exception : public Object, public std::exception {
    private:
        ref<String> _message;


    public:
        Exception(TCHAR* message) : _message(new String(message)) {
        }


        Exception(ref<String> message) : _message(message) {
        }


        virtual ref<String> getMessage() {
            return _message;
        }
    };


    class Application : public Object {
    private:
        HINSTANCE _handle;


    public:
        Application(HINSTANCE handle) : _handle(handle) {
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


        virtual HINSTANCE getHandle() {
            return _handle;
        }


    protected:
        virtual void onStart(int windowShowMode) {
        }
    };


    class MenuItem : public Object {
        friend class Menu;

    private:
        ref<String> _caption;
        bool _hasId;
        UINT_PTR _id;


    public:
        MenuItem(ref<String> caption) : _caption(caption), _hasId(false) {
        }


        virtual ref<String> getCaption() {
            return _caption;
        }


    protected:
        virtual UINT_PTR getId() {
            // Must not be zero for popup menus to work.
            static UINT_PTR counter = 1;

            if (!_hasId) {
                _id = counter++;
                _hasId = true;
            }

            return _id;
        }


        virtual UINT getType() {
            return MF_STRING;
        }


        virtual void onChoose() {
        }
    };
    

    class Menu : public MenuItem {
    private:
        HMENU _handle;
        std::vector<ref<MenuItem>> _items;


    public:
        Menu(ref<String> caption = NULL) : MenuItem(caption) {
            _handle = CreateMenu();

            if (_handle == NULL) {
                throw Exception(GetLastErrorMessage());
            }
        }


        virtual void addItem(ref<MenuItem> item) {
            UINT flags = item->getType();
            UINT_PTR id = item->getId();
            LPCTSTR caption = item->getCaption()->c_str();

            if (!AppendMenu(getHandle(), flags, id, caption)) {
                throw Exception(GetLastErrorMessage());
            }

            _items.push_back(item);
        }


        virtual bool chooseItem(UINT_PTR id) {
            for (size_t i = 0; i < _items.size(); ++i) {
                ref<MenuItem> item = _items[i];

                if (item->getId() == id) {
                    item->onChoose();
                    return true;
                }
                else if (item->getType() == MF_POPUP) {
                    ref<Menu> menu = item.cast<Menu>();

                    if (menu->chooseItem(id)) {
                        return true;
                    }
                }
            }

            return false;
        }


        virtual HMENU getHandle() {
            return _handle;
        }


        virtual size_t getItemCount() {
            return _items.size();
        }


    protected:
        virtual UINT_PTR getId() {
            return (UINT_PTR) getHandle();
        }


        virtual UINT getType() {
            return MF_POPUP;
        }
    };


    class Window : public Object {
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
        ref<Menu> _menuBar;


    public:
        Window(ref<String> title, ref<String> className) {
            HWND window = FindWindow(className->c_str(), title->c_str());

            if (window != NULL) {
                SetForegroundWindow(window);
                PostQuitMessage(EXIT_FAILURE);
            }
            else {
                WNDCLASS windowClass;

                ZeroMemory(&windowClass, sizeof(WNDCLASS));
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
            if (menu->getItemCount() > 2) {
                throw Exception(TEXT("Too many items in the menu bar."));
            }
            
            SHMENUBARINFO info;

            ZeroMemory(&info, sizeof(SHMENUBARINFO));
            info.cbSize = sizeof(SHMENUBARINFO);
            info.hwndParent = getHandle();
            info.dwFlags = SHCMBF_HMENU | SHCMBF_HIDESIPBUTTON;
            info.nToolBarId = (UINT) menu->getHandle();
            info.hInstRes = GetModuleHandle(NULL);
            
            if (!SHCreateMenuBar(&info)) {
                throw Exception(GetLastErrorMessage());
            }

            _menuBar = menu;
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
            case WM_COMMAND:
                if (_menuBar != NULL) {
                    _menuBar->chooseItem(LOWORD(wParam));
                }
                break;
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
