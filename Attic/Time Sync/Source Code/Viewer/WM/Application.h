#pragma once


#include "Object.h"


namespace WM {
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
}
