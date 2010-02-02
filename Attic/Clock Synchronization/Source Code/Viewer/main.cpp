#include "resources.h"
#include "Win32.h"


#define TITLE Win32::LoadStringT(IDS_TITLE)
#define WINDOW_CLASS Win32::LoadStringT(IDS_WINDOW_CLASS)


class Viewer : public Win32::Application, Win32::Window {
public:
    Viewer() : Win32::Window(TITLE, WINDOW_CLASS) {
    }


protected:
    virtual void onStart(LPTSTR commandLine, int windowShowMode) {
        show(windowShowMode);
    }
};


int WINAPI WinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    LPTSTR commandLine,
    int windowShowMode)
{
    try {
        return Viewer().start(commandLine, windowShowMode);
    }
    catch (std::exception error) {
        return EXIT_FAILURE;
    }
}


/*
        case WM_COMMAND: {
            int menuId = LOWORD(wParam);
            
            switch (menuId) {
            case IDS_SK_EXIT:
                SendMessage(handle, WM_CLOSE, 0, 0);
                break;
            case IDS_SK_UPDATE:
                break;
            default:
                return DefWindowProc(handle, message, wParam, lParam);
            }
            break;
        }
*/

/*
        HINSTANCE application = GetModuleHandle(NULL);
        HMENU menuBar = CreateMenu();
        
        ref<Win32::tstring> update = Win32::LoadStringT(IDS_SK_UPDATE);
        ref<Win32::tstring> exit = Win32::LoadStringT(IDS_SK_EXIT);
        
        InsertMenu(menuBar, -1, MF_BYPOSITION, IDS_SK_UPDATE, update->c_str()); 
        InsertMenu(menuBar, -1, MF_BYPOSITION, IDS_SK_EXIT, exit->c_str());

        SHMENUBARINFO menuBarInfo;

        memset(&menuBarInfo, 0, sizeof(SHMENUBARINFO));
        menuBarInfo.cbSize = sizeof(SHMENUBARINFO);
        menuBarInfo.hwndParent = _handle;
        menuBarInfo.dwFlags = SHCMBF_HMENU | SHCMBF_HIDESIPBUTTON;
        menuBarInfo.nToolBarId = (UINT) menuBar;
        menuBarInfo.hInstRes = application;
        
        SHCreateMenuBar(&menuBarInfo);

        CreateWindow( 
            L"BUTTON",   // Predefined class; Unicode assumed.
            L"OK",       // Button text. 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles.
            10,         // x position. 
            10,         // y position. 
            100,        // Button width.
            100,        // Button height.
            _handle,       // Parent window.
            NULL,       // No menu.
            application, 
            NULL);      // Pointer not needed.
*/
