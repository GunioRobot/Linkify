#ifndef __GC__
#define __GC__


/**
 * Smart pointer implementation (similar to Boost's shared_ptr) that can be used
 * as a simple Garbage Collector based on reference counting.
 */


#include <cstddef>
#include <exception>
#include <map>
#include <typeinfo>


namespace GC {
    class DuplicateReferenceError : public std::exception {
    public:
        virtual const char* what() const throw() {
            return "Duplicate reference";
        }
    };
    
    
    class NullReferenceError : public std::exception {
    public:
        virtual const char* what() const throw() {
            return "Null reference";
        }
    };
    
    
    class Counter {
    protected:
        template<typename T>
        static void decrement(T* object) {
            if ((object != NULL) && (--_count[object] == 0)) {
                delete object;
                _count.erase(_count.find(object));
            }
        }
        
        
        static size_t increment(void* object) {
            return object != NULL ? ++_count[object] : 0;
        }
        
        
    private:
        static std::map<void*, size_t> _count;
    };
    
    
    template<typename T>
    class ref : protected Counter {
    public:
        ref() : _object(NULL), _type(&typeid(NULL)) {
        }
        
        
        ref(const ref<T>& copy) : _object(copy._object), _type(copy._type) {
            increment(_object);
        }
        
        
        template<typename U>
        ref(const ref<U>& copy) : _object(copy._object), _type(copy._type) {
            increment(_object);
        }
        
        
        ref(T* object) : _object(object), _type(&typeid(T)) {
            if (increment(_object) > 1) {
                throw DuplicateReferenceError();
            }
        }
        
        
        template<typename U>
        ref(U* object) : _object(object), _type(&typeid(U)) {
            if (increment(_object) > 1) {
                throw DuplicateReferenceError();
            }
        }
        
        
        ~ref() {
            decrement(_object);
        }
        
        
        ref<T>& operator =(const ref<T>& copy) {
            if (this != &copy) {
                decrement(_object);
                
                _object = copy._object;
                _type = copy._type;
                
                increment(_object);
            }
            
            return *this;
        }
        
        
        bool operator ==(const ref<T>& other) {
            return _object == other._object;
        }
        
        
        bool operator !=(const ref<T>& other) {
            return _object != other._object;
        }
        
        
        T* operator ->() {
            if (_object == NULL) {
                throw NullReferenceError();
            }
            
            return _object;
        }
        
        
        const T& operator *() {
            if (_object == NULL) {
                throw NullReferenceError();
            }
            
            return *_object;
        }
        
        
        const std::type_info& type() {
            return *_type;
        }
        
        
    //private:
        T* _object;
        const std::type_info* _type;
    };
}


#endif
