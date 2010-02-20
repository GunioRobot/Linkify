#include <cstdlib>
#include "WM.h"


class TimeTextBox: public WM::TextBox, public WM::Timer {
private:
    // Store the time in the following format: "YYYY-MM-DD HH:MM:SS".
    static const size_t _LENGTH = 4 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1;


private:
    void (*_getTime)(SYSTEMTIME*);
    TCHAR _buffer[_LENGTH];
    WM::String _time;


public:
    TimeTextBox(void (*getTime)(SYSTEMTIME*)):
        WM::TextBox(S("-")), _getTime(getTime), _time(TEXT(""))
    {
    }


protected:
    virtual void onAddTo(ref<WM::Window> owner) {
        WM::TextBox::onAddTo(owner);
        onTimeout();
        start(1);
    }


    virtual void onTimeout() {
        SYSTEMTIME time;
        _getTime(&time);
        
        _sntprintf(_buffer, _LENGTH, TEXT("%d-%02d-%02d %02d:%02d:%02d"),
            time.wYear, time.wMonth, time.wDay,
            time.wHour, time.wMinute, time.wSecond);

        _time = _buffer;
        setText(noref &_time);
    }
};


class Executable: public WM::Application, WM::Window {
private:
    ref<WM::MenuItem> _exitOption;
    ref<WM::MenuItem> _updateOption;


public:
    Executable(HINSTANCE handle):
        WM::Application(handle), WM::Window(S("Time Sync"), S("TIMESYNC"))
    {
        _exitOption = new WM::MenuItem(S("Exit"));
        _updateOption = new WM::MenuItem(S("Update"));
        
        ref<WM::Menu> mainMenu = new WM::Menu(S("Menu"));
        ref<WM::Label> deviceLabel = new WM::Label(S("Device:"));
        ref<WM::TextBox> deviceTime = new TimeTextBox(GetLocalTime);
        ref<WM::Label> gpsLabel = new WM::Label(S("GPS:"));
        ref<WM::TextBox> gpsTime = new WM::TextBox(S("-"));

        const long MARGIN = 8;
        const long PADDING = 2;
        const long HEIGHT = 20;
        
        deviceLabel->setSize(50, HEIGHT - PADDING);
        deviceTime->setSize(WM::Widget::EXPANDABLE, HEIGHT);
        gpsLabel->setSize(deviceLabel);
        gpsTime->setSize(deviceTime);

        deviceLabel->setPosition(MARGIN, MARGIN + PADDING);
        deviceTime->setPosition(MARGIN + deviceLabel->getWidth(), MARGIN);
        
        gpsLabel->setPosition(
            deviceLabel->getLeft(),
            2 * deviceLabel->getTop() + deviceLabel->getHeight());
        
        gpsTime->setPosition(
            deviceTime->getLeft(),
            2 * deviceTime->getTop() + deviceTime->getHeight());

        deviceTime->setMarginRight(MARGIN);
        deviceTime->setReadOnly(true);
        gpsTime->setMarginRight(MARGIN);
        gpsTime->setReadOnly(true);

        mainMenu->add(_updateOption);
        mainMenu->add(_exitOption);
        enableMenuBar(mainMenu);
        
        add(deviceLabel);
        add(deviceTime);
        add(gpsLabel);
        add(gpsTime);
    }


    virtual int start(int windowShowMode) {
        open(windowShowMode);
        return WM::Application::start(windowShowMode);
    }


protected:
    virtual void onChoose(ref<WM::MenuItem> item) {
        if (item == _exitOption) {
            close();
        }
    }
};


int WINAPI WinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    LPTSTR commandLine,
    int windowShowMode)
{
    try {
        return Executable(instance).start(windowShowMode);
    }
    catch (WM::Exception exception) {
        WM::ErrorMessageBox(exception.getMessage());
        return EXIT_FAILURE;
    }
}
