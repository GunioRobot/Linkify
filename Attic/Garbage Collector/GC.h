#ifndef __GC__
#define __GC__


/**
 * @file
 * @brief Garbage Collector
 * @see http://www.boost.org/doc/libs/release/libs/smart_ptr/shared_ptr.htm
 *
 * Simple garbage collector implemented with reference counting.
 */


#include <cstddef>
#include <exception>
#include <map>
#include <typeinfo>


namespace GC {
    class DuplicateReferenceError: public std::exception {
    public:
        virtual const char* what() const throw() {
            return "Duplicate reference";
        }
    };
    
    
    class NullReferenceError: public std::exception {
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
    class Reference: protected Counter {
    public:
        Reference(): _obj(NULL), _type(&typeid(NULL)) {
        }
        
        
        Reference(const Reference<T>& r): _obj(r._obj), _type(r._type) {
            increment(_obj);
        }
        
        
        template<typename U>
        Reference(const Reference<U>& r): _obj(r._obj), _type(r._type) {
            increment(_obj);
        }
        
        
        Reference(T* object): _obj(object), _type(&typeid(T)) {
            if (increment(_obj) > 1) {
                throw DuplicateReferenceError();
            }
        }
        
        
        template<typename U>
        Reference(U* object): _obj(object), _type(&typeid(U)) {
            if (increment(_obj) > 1) {
                throw DuplicateReferenceError();
            }
        }
        
        
        ~Reference() {
            decrement(_obj);
        }
        
        
        Reference<T>& operator =(const Reference<T>& copy) {
            if (this != &copy) {
                decrement(_obj);
                
                _obj = copy._obj;
                _type = copy._type;
                
                increment(_obj);
            }
            
            return *this;
        }
        
        
        bool operator ==(const Reference<T>& other) {
            return _obj == other._obj;
        }
        
        
        bool operator !=(const Reference<T>& other) {
            return _obj != other._obj;
        }
        
        
        T* operator ->() {
            if (_obj == NULL) {
                throw NullReferenceError();
            }
            
            return _obj;
        }
        
        
        const T& operator *() {
            if (_obj == NULL) {
                throw NullReferenceError();
            }
            
            return *_obj;
        }
        
        
        const std::type_info& type() {
            return *_type;
        }
        
        
    //private:
        T* _obj;
        const std::type_info* _type;
    };
}


#endif
