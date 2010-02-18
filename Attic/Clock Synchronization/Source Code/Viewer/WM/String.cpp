#include "String.h"


namespace WM {
    ref<String> ToString(const TCHAR* string) {
        return new String(string);
    }
}
