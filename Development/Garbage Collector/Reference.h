#ifndef __REFERENCE__
#define __REFERENCE__


/**
 * @file
 * @brief Garbage Collector
 * @see http://www.boost.org/doc/libs/release/libs/smart_ptr/shared_ptr.htm
 *
 * Simple garbage collector implemented with reference counting.
 */


#include <cstddef>
#include <typeinfo>


template<typename T>
class Reference {
public:
    Reference(): _obj(NULL), _count(NULL), _type(&typeid(NULL)) {
    }
    
    
    Reference(const Reference<T>& r): _obj(r._obj), _count(r._count), _type(r._type) {
        increment();
    }
    
    
    template<typename U>
    Reference(const Reference<U>& r): _obj(r._obj), _count(r._count), _type(r._type) {
        increment();
    }
    
    
    Reference(T* object): _obj(object), _count(NULL), _type(&typeid(T)) {
        increment();
    }
    
    
    template<typename U>
    Reference(U* object): _obj(object), _count(NULL), _type(&typeid(U)) {
        increment();
    }
    
    
    ~Reference() {
        decrement();
    }
    
    
    Reference<T>& operator =(const Reference<T>& copy) {
        if (this != &copy) {
            decrement();
            
            _obj = copy._obj;
            _type = copy._type;
            
            increment();
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
        return _obj;
    }
    
    
    const T& operator *() {
        return *_obj;
    }
    
    
    bool null() {
        return _obj == NULL;
    }
    
    
    const std::type_info& type() {
        return *_type;
    }
    
    
private:
    void decrement() {
        if ((_obj != NULL) && (--*_count == 0)) {
            delete _obj;
            delete _count;
        }
    }
    
    
    void increment() {
        if (_obj != NULL) {
            if (_count == NULL) {
                _count = new size_t(1);
            }
            else {
                ++*_count;
            }
        }
    }
    
    
public:
    T* _obj;
    size_t* _count;
    const std::type_info* _type;
};


#endif
