#include <cstdlib>
#include "WM.h"


class TimeTextBox: public WM::TextBox, public WM::Timer {
private:
    // Store the time in the following format: "YYYY-MM-DD HH:MM:SS".
    static const size_t _LENGTH = 4 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1;


private:
    void (*_getTime)(SYSTEMTIME*);
    TCHAR _buffer[_LENGTH];


public:
    TimeTextBox(void (*getTime)(SYSTEMTIME*)):
        WM::TextBox(), _getTime(getTime)
    {
        onTimeout();
        start(1);
    }


protected:
    virtual void onTimeout() {
        SYSTEMTIME time;
        _getTime(&time);
        
        _sntprintf(_buffer, _LENGTH, TEXT("%d-%02d-%02d %02d:%02d:%02d"),
            time.wYear, time.wMonth, time.wDay,
            time.wHour, time.wMinute, time.wSecond);

        setText(_buffer);
    }
};
