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


#define S(string) WM::ToString(TEXT(string))


namespace WM {
    typedef std::basic_string<TCHAR> String;
    typedef std::basic_stringstream<TCHAR> StringStream;
    

    void ErrorMessageBox(ref<String> message);
    int GetDeviceCaps(int item, HDC deviceContext = NULL);
    ref<String> GetLastErrorMessage();
    size_t Scale(double logicalMm, int pxPerLogicalIn);
    size_t ScaleX(double logicalMm);
    size_t ScaleY(double logicalMm);
    ref<String> ToString(const TCHAR* string);


    class Object {
    public:
        virtual ~Object() {
        }
    };


    class Exception: public Object, public std::exception {
    private:
        ref<String> _message;


    public:
        Exception(ref<String> message): _message(message) {
        }


        virtual ref<String> getMessage() {
            return _message;
        }
    };


    class Application: public Object {
    private:
        HINSTANCE _handle;


    public:
        Application(HINSTANCE handle): _handle(handle) {
        }


        virtual int start(int windowShowMode) {
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
    };


    class MenuItem: public Object {
    private:
        ref<String> _caption;
        bool _hasId;
        UINT_PTR _id;


    public:
        MenuItem(ref<String> caption): _caption(caption), _hasId(false) {
        }


        virtual ref<String> getCaption() {
            return _caption;
        }


        virtual UINT_PTR getId() {
            // Zero is reserved for controls.
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
    };
    

    // TODO: Distinguish between MenuBar and PopupMenu?
    class Menu: public MenuItem {
    private:
        HMENU _handle;
        std::vector<ref<MenuItem>> _items;


    public:
        Menu(ref<String> caption):
            MenuItem(caption), _handle(CreatePopupMenu())
        {
            if (_handle == NULL) {
                throw Exception(GetLastErrorMessage());
            }
        }


        virtual void add(ref<MenuItem> item) {
            UINT flags = item->getType();
            UINT_PTR id = item->getId();
            LPCTSTR caption = item->getCaption()->c_str();

            if (!AppendMenu(getHandle(), flags, id, caption)) {
                throw Exception(GetLastErrorMessage());
            }

            _items.push_back(item);
        }


        virtual HMENU getHandle() {
            return _handle;
        }


        virtual UINT_PTR getId() {
            return (UINT_PTR) getHandle();
        }


        virtual ref<MenuItem> getItemById(UINT_PTR id) {
            for (size_t i = 0; i < getItemCount(); ++i) {
                ref<MenuItem> item = _items[i];

                if (item->getId() == id) {
                    return item;
                }
                else if (item->getType() == MF_POPUP) {
                    ref<MenuItem> child = item.cast<Menu>()->getItemById(id);

                    if (child != NULL) {
                        return child;
                    }
                }
            }

            return NULL;
        }


        virtual size_t getItemCount() {
            return _items.size();
        }


        virtual UINT getType() {
            return MF_POPUP;
        }
    };


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


    private:
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


        virtual void choose(ref<MenuItem> item) {
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
                throw Exception(GetLastErrorMessage());
            }

            _menuBar = menu;
        }


        virtual HWND getHandle() {
            return _handle;
        }


        virtual void open(int mode) {
            ShowWindow(getHandle(), mode);

            if (UpdateWindow(getHandle()) == 0) {
                throw Exception(GetLastErrorMessage());
            }
        }


    private:
        void handleCommand(WPARAM wParam, LPARAM lParam) {
            WORD notifyCode = HIWORD(wParam);
            WORD id = LOWORD(wParam);
            HWND handle = (HWND) lParam;

            if ((notifyCode == 0) && (id != 0) && (_menuBar != NULL)) {
                choose(_menuBar->getItemById(id));
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
            default:
                return DefWindowProc(getHandle(), message, wParam, lParam);
            }
            
            return 0;
        }
    };
    
    
    class Widget: public Object {
    };


    class Label: public Widget {
    private:
        HWND _handle;


    public:
        Label(ref<String> caption, ref<Window> owner) {
            _handle = CreateWindow(
                TEXT("STATIC"),
                caption->c_str(),
                WS_CHILD + WS_TABSTOP,
                0,
                0,
                0,
                0,
                owner->getHandle(),
                NULL,
                GetModuleHandle(NULL), 
                NULL);

            if (_handle == NULL) {
                throw Exception(GetLastErrorMessage());
            }
        }
    };
}
