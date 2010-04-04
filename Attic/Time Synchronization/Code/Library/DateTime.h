#pragma once


#include "Exception.h"
#include "Object.h"


namespace Wm {
    class DateTime: public Object, public SYSTEMTIME {
    public:
        static DateTime utcToLocal(DateTime time) {
            TIME_ZONE_INFORMATION timeZone;
            DWORD zoneId = GetTimeZoneInformation(&timeZone);
            long minutes = -timeZone.Bias;

            if (zoneId == TIME_ZONE_ID_DAYLIGHT) {
                WORD month = timeZone.DaylightDate.wMonth;
                minutes -= (month == 0) ? 0 : timeZone.DaylightBias;
            }
            else if (zoneId == TIME_ZONE_ID_STANDARD) {
                WORD month = timeZone.StandardDate.wMonth;
                minutes -= (month == 0) ? 0 : timeZone.StandardBias;
            }
            else {
                Wm::Exception::throwLastError();
            }
            
            time.addMinutes(minutes);
            return time;
        }


    private:
        static void fromScalar(ULARGE_INTEGER& scalar, SYSTEMTIME& time) {
            FILETIME fileTime;
            
            fileTime.dwLowDateTime = scalar.LowPart;
            fileTime.dwHighDateTime = scalar.HighPart;

            if (!FileTimeToSystemTime(&fileTime, &time)) {
                Wm::Exception::throwLastError();
            }
        }
        
        
        static void toScalar(SYSTEMTIME& time, ULARGE_INTEGER& scalar) {
            FILETIME fileTime;

            if (!SystemTimeToFileTime(&time, &fileTime)) {
                Wm::Exception::throwLastError();
            }

            scalar.LowPart = fileTime.dwLowDateTime;
            scalar.HighPart = fileTime.dwHighDateTime;
        }
    
    
    public:
        DateTime() {
            wYear = 0;
            wMonth = 0;
            wDayOfWeek = 0;
            wDay = 0;
            wHour = 0;
            wMinute = 0;
            wSecond = 0;
            wMilliseconds = 0;
        }


        DateTime(SYSTEMTIME systemTime) {
            wYear = systemTime.wYear;
            wMonth = systemTime.wMonth;
            wDayOfWeek = systemTime.wDayOfWeek;
            wDay = systemTime.wDay;
            wHour = systemTime.wHour;
            wMinute = systemTime.wMinute;
            wSecond = systemTime.wSecond;
            wMilliseconds = systemTime.wMilliseconds;
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
    };
}
