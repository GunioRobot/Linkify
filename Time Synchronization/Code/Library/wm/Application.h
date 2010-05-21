#pragma once


#include "Object.h"


namespace wm {
    class Application: public Object {
    private:
        HINSTANCE _handle;


    public:
        Application(HINSTANCE handle): _handle(handle) {
        }


        virtual HINSTANCE getApplicationHandle() {
            return _handle;
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
    };
}
