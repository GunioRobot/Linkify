#pragma once


#include <exception>
#include "Object.h"
#include "String.h"


namespace WM {
    void ErrorMessageBox(ref<String> message);
    ref<String> GetLastErrorMessage();
    
    
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
}
