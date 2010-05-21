#ifndef __REF__
#define __REF__


/**
 * @file
 * @brief Smart pointer
 * @see http://www.boost.org/doc/libs/release/libs/smart_ptr/shared_ptr.htm
 *
 * Simple garbage collector implemented with reference counting.
 */


#include <cstddef>
#include <typeinfo>


/**
 * Indicates that an object must not be managed.
 *
 * @hideinitializer
 * @code
 * std::string buffer;
 * ref<std::string> r = noref &buffer;
 * @endcode
 */
#define noref \
    ref<void>() <<


/**
 * Wrapper class for managed objects.
 *
 * Any object pointer stored will be automatically memory managed.
 *
 * @code
 * ref<std::string> name = new std::string("John");
 * @endcode
 * @todo Change visibility of identifiers with a leading underscore to private.
 */
template<typename T>
class ref {
public:
    /**
     * Creates a null reference.
     */
    ref(): _object(NULL), _count(NULL) {
    }
    
    
    /**
     * Creates a copy of a reference.
     *
     * @param [in] r reference to copy
     */
    ref(const ref<T>& r): _object(r._object), _count(r._count) {
        _increment();
    }
    
    
    /**
     * Creates a copy of a reference with a different but compatible type.
     *
     * @param [in] r reference to copy
     */
    template<typename U>
    ref(const ref<U>& r): _object(r._object), _count(r._count) {
        _increment();
    }
    
    
    /**
     * Creates a reference to an object.
     *
     * @param [in] object object pointer
     */
    ref(T* object): _object(object), _count(new size_t(0)) {
        _increment();
    }
    
    
    /**
     * Creates a reference to an object with a different but compatible type.
     *
     * @param [in] object object pointer
     */
    template<typename U>
    ref(U* object): _object(object), _count(new size_t(0)) {
        _increment();
    }
    
    
    ~ref() {
        _decrement();
    }
    
    
    ref<T>& operator =(const ref<T>& r) {
        if (this != &r) {
            _decrement();
            
            _object = r._object;
            _count = r._count;
            
            _increment();
        }
        
        return *this;
    }
    
    
    bool operator ==(const ref<T>& r) {
        return _object == r._object;
    }
    
    
    bool operator !=(const ref<T>& r) {
        return _object != r._object;
    }
    
    
    T* operator ->() {
        return _object;
    }
    
    
    const T& operator *() {
        return *_object;
    }
    
    
    /**
     * Performs a dynamic cast.
     *
     * @return copy to the same object using a different type if successful,
     *         otherwise is a null reference
     */
    template<typename U>
    ref<U> cast() {
        ref<U> r = NULL;
        U* object = dynamic_cast<U*>(_object);
        
        if (object != NULL) {
            r._object = object;
            r._count = _count;
            r._increment();
        }
        
        return r;
    }
    
    
    /**
     * Checks if this is a null reference.
     *
     * @return @c true if the object points to @c NULL, or @c false otherwise
     */
    bool null() {
        return _object == NULL;
    }
    
    
    /**
     * Gets type information.
     *
     * @return value of @c typeid applied to the managed object
     */
    const std::type_info& type() {
        return typeid(*_object);
    }
    
    
//private:
    void _decrement() {
        if ((_object != NULL) && (_count != NULL) && (--*_count == 0)) {
            delete _object;
            delete _count;
        }
    }
    
    
    void _increment() {
        if ((_object != NULL) && (_count != NULL)) {
            ++*_count;
        }
    }
    
    
    /** Pointer to the object being managed. */
    T* _object;
    
    /** Number of references to the object, or @c NULL if it's not managed. */
    size_t* _count;
};


template<>
class ref<void> {
public:
    template<typename T>
    ref<T> operator <<(T* object) {
        ref<T> r = NULL;
        
        r._object = object;
        r._count = NULL;
        
        return r;
    }
};


#endif
