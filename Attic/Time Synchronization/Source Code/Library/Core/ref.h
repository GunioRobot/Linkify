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
    ref(): _obj(NULL), _count(NULL), _type(&typeid(NULL)) {
    }
    
    
    /**
     * Creates a copy of a reference.
     *
     * @param [in] r reference to copy
     */
    ref(const ref<T>& r): _obj(r._obj), _count(r._count), _type(r._type) {
        _increment();
    }
    
    
    /**
     * Creates a copy of a reference with a different but compatible type.
     *
     * @param [in] r reference to copy
     */
    template<typename U>
    ref(const ref<U>& r): _obj(r._obj), _count(r._count), _type(r._type) {
        _increment();
    }
    
    
    /**
     * Creates a reference to an object.
     *
     * @param [in] object object pointer
     */
    ref(T* object): _obj(object), _count(new size_t(0)), _type(&typeid(T)) {
        _increment();
    }
    
    
    /**
     * Creates a reference to an object with a different but compatible type.
     *
     * @param [in] object object pointer
     */
    template<typename U>
    ref(U* object): _obj(object), _count(new size_t(0)), _type(&typeid(U)) {
        _increment();
    }
    
    
    ~ref() {
        _decrement();
    }
    
    
    ref<T>& operator =(const ref<T>& copy) {
        if (this != &copy) {
            _decrement();
            
            _obj = copy._obj;
            _count = copy._count;
            _type = copy._type;
            
            _increment();
        }
        
        return *this;
    }
    
    
    bool operator ==(const ref<T>& other) {
        return _obj == other._obj;
    }
    
    
    bool operator !=(const ref<T>& other) {
        return _obj != other._obj;
    }
    
    
    T* operator ->() {
        return _obj;
    }
    
    
    const T& operator *() {
        return *_obj;
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
        U* object = dynamic_cast<U*>(_obj);
        
        if (object != NULL) {
            r._obj = object;
            r._count = _count;
            r._type = &typeid(U);
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
        return _obj == NULL;
    }
    
    
    /**
     * Gets type information.
     *
     * @return value of @c typeid applied to the managed object
     */
    const std::type_info& type() {
        return *_type;
    }
    
    
//private:
    void _decrement() {
        if ((_obj != NULL) && (_count != NULL) && (--*_count == 0)) {
            delete _obj;
            delete _count;
        }
    }
    
    
    void _increment() {
        if ((_obj != NULL) && (_count != NULL)) {
            ++*_count;
        }
    }
    
    
    /** Pointer to the object being managed. */
    T* _obj;
    
    /** Number of references to the object, or @c NULL if it's not managed. */
    size_t* _count;
    
    /** Type information about the object. */
    const std::type_info* _type;
};


template<>
class ref<void> {
public:
    template<typename T>
    ref<T> operator <<(T* object) {
        ref<T> r = NULL;
        
        r._obj = object;
        r._count = NULL;
        r._type = &typeid(T);
        
        return r;
    }
};


#endif
