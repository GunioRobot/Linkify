#pragma once


#include "Exception.h"
#include "Object.h"


namespace Wm {
    class Application: public Object {
    public:
        static void exit(Exception exception) {
            UINT type = MB_OK + MB_ICONERROR;
            const TCHAR* message = exception.getMessage().c_str();

            MessageBox(GetForegroundWindow(), message, NULL, type);
            ExitProcess(EXIT_FAILURE);
        }


    private:
        HINSTANCE _handle;


    public:
        Application(HINSTANCE handle): _handle(handle) {
        }


        virtual HINSTANCE getHandle() {
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
