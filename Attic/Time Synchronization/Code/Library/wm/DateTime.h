#pragma once


#include "Exception.h"
#include "Object.h"
#include "String.h"


namespace wm {
    class DateTime: public Object, public SYSTEMTIME {
    public:
        static DateTime utcToLocal(DateTime time) {
            FILETIME utcTime, localTime;

            if (!SystemTimeToFileTime(&time, &utcTime)) {
                Exception::throwLastError();
            }

            if (!FileTimeToLocalFileTime(&utcTime, &localTime)) {
                Exception::throwLastError();
            }
            
            if (!FileTimeToSystemTime(&localTime, &time)) {
                Exception::throwLastError();
            }

            return time;
        }


    private:
        static void fromScalar(ULARGE_INTEGER& scalar, SYSTEMTIME& time) {
            FILETIME fileTime;
            
            fileTime.dwLowDateTime = scalar.LowPart;
            fileTime.dwHighDateTime = scalar.HighPart;

            if (!FileTimeToSystemTime(&fileTime, &time)) {
                Exception::throwLastError();
            }
        }
        
        
        static void toScalar(SYSTEMTIME& time, ULARGE_INTEGER& scalar) {
            FILETIME fileTime;

            if (!SystemTimeToFileTime(&time, &fileTime)) {
                Exception::throwLastError();
            }

            scalar.LowPart = fileTime.dwLowDateTime;
            scalar.HighPart = fileTime.dwHighDateTime;
        }
    
    
    public:
        DateTime() {
            this->wYear = 0;
            this->wMonth = 0;
            this->wDayOfWeek = 0;
            this->wDay = 0;
            this->wHour = 0;
            this->wMinute = 0;
            this->wSecond = 0;
            this->wMilliseconds = 0;
        }


        DateTime(SYSTEMTIME systemTime) {
            this->wYear = systemTime.wYear;
            this->wMonth = systemTime.wMonth;
            this->wDayOfWeek = systemTime.wDayOfWeek;
            this->wDay = systemTime.wDay;
            this->wHour = systemTime.wHour;
            this->wMinute = systemTime.wMinute;
            this->wSecond = systemTime.wSecond;
            this->wMilliseconds = systemTime.wMilliseconds;
        }


        DateTime(ULARGE_INTEGER value) {
            fromScalar(value, *this);
        }


        virtual operator ULARGE_INTEGER() {
            ULARGE_INTEGER time;

            toScalar(*this, time);
            return time;
        }


        virtual void addMicroSeconds(LONGLONG us) {
            ULARGE_INTEGER time;

            toScalar(*this, time);
            time.QuadPart += us * 10;
            fromScalar(time, *this);
        }


        virtual void addMilliSeconds(LONGLONG ms) {
            ULARGE_INTEGER time;

            toScalar(*this, time);
            time.QuadPart += ms * 1000 * 10;
            fromScalar(time, *this);
        }


        virtual void addMinutes(LONGLONG min) {
            ULARGE_INTEGER time;

            toScalar(*this, time);
            time.QuadPart += min * 60 * 1000 * 1000 * 10;
            fromScalar(time, *this);
        }


        virtual void addSeconds(LONGLONG s) {
            ULARGE_INTEGER time;

            toScalar(*this, time);
            time.QuadPart += s * 1000 * 1000 * 10;
            fromScalar(time, *this);
        }


        virtual String formatIso(String designator = S("T")) {
            return String::format(TEXT("%d-%02d-%02d%s%02d:%02d:%02d"),
                this->wYear, this->wMonth, this->wDay,
                designator.c_str(),
                this->wHour, this->wMinute, this->wSecond);
        }
    };
}
