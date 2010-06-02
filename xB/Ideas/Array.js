// Create a local scope.
(function() {


// Array.shuffle
// - Don't use: array.sort(function() {return 0.5 - Math.random();});
// - See http://www.robweir.com/blog/2010/02/microsoft-random-browser-ballot.html


/**
 * Compares this array with another.
 *
 * @param {Array} array array to be compared for equality
 * @param {Function} comparator optional comparator function that receives two
 *        arguments and returns true or 0 if they're equal (defaults to ===)
 * @return true if the arrays are equal or false otherwise
 * @type Boolean
 */
Array.prototype.equals = function(array, comparator) {
    if (this.length != array.length) {
        return false;
    }
    if (!defined(comparator)) {
        comparator = function(x, y) { return x === y; };
    }
    
    for (var i = 0; i < this.length; ++i) {
        var result = comparator(this[i], array[i]);
        
        if ((result !== 0) && (result !== true)) {
            return false;
        }
    }
    
    return true;
};


/**
 * Gets the index of the first occurrence of an element.
 *
 * @param searchElement element to search for
 * @param {Number} fromIndex optional index at which to begin the search
 * @param {Function} comparator optional comparator function that receives two
 *        arguments and returns true or 0 if they're equal (defaults to ===)
 * @return the index of the first occurrence or -1 if it wasn't found
 * @type Number
 */
Array.prototype.indexOf = function(searchElement, fromIndex, comparator) {
    if ((arguments.length == 2) && (typeof fromIndex == 'function')) {
        comparator = fromIndex;
        fromIndex = void 0;
    }
    if (!defined(comparator)) {
        comparator = function(a, b) { return a === b; };
    }
    
    var length = Object.toUInt32(this.length);
    
    fromIndex = Object.toNumber(fromIndex) || 0;
    fromIndex = (fromIndex < 0) ? Math.ceil(fromIndex) : Math.floor(fromIndex);
    
    if (fromIndex < 0) {
        fromIndex += length;
    }
    
    for (; fromIndex < length; ++fromIndex) {
        if (!(fromIndex in this)) {
            continue;
        }
        
        var result = comparator(this[fromIndex], searchElement);
        if ((result === 0) || (result === true)) {
            return fromIndex;
        }
    }
    
    return NaN;
};


/**
 * Gets the index of the last occurrence of an element.
 *
 * @param searchElement element to search for
 * @param {Number} fromIndex optional index at which to begin the search
 * @param {Function} comparator optional comparator function that receives two
 *        arguments and returns true or 0 if they're equal (defaults to ===)
 * @return the index of the last occurrence or -1 if it wasn't found
 * @type Number
 */
Array.prototype.lastIndexOf = function(searchElement, fromIndex, comparator) {
    if ((arguments.length == 2) && (typeof fromIndex == 'function')) {
        comparator = fromIndex;
        fromIndex = void 0;
    }
    if (!defined(comparator)) {
        comparator = function(a, b) { return a === b; };
    }
    
    var length = Object.toUInt32(this.length);
    fromIndex = Object.toNumber(fromIndex);
    
    if (isNaN(fromIndex)) {
        fromIndex = length - 1;
    }
    else {
        fromIndex = (fromIndex < 0) ? Math.ceil(fromIndex)
                                    : Math.floor(fromIndex);
        if (fromIndex < 0) {
            fromIndex += length;
        }
        else if (fromIndex >= length) {
            fromIndex = length - 1;
        }
    }
    
    for (; fromIndex > -1; --fromIndex) {
        if (!(fromIndex in this)) {
            continue;
        }
        
        var result = comparator(this[fromIndex], searchElement);
        if ((result === 0) || (result === true)) {
            return fromIndex;
        }
    }
    
    return NaN;
};


var ArrayPrototypePushOriginal = Array.prototype.push;

Array.prototype.push = function(/* ... */) {
    ArrayPrototypePushOriginal.apply(this, arguments);
    return this;
};


Array.prototype.toString = function() {
    return this.join(', ');
};


var ArrayPrototypeUnshiftOriginal = Array.prototype.unshift;

Array.prototype.unshift = function(/* ... */) {
    ArrayPrototypeUnshiftOriginal.apply(this, arguments);
    return this;
};


Array.prototype.pop = function() {
    var length = Object.toUInt32(this.length);
    var element;
    
    if (length > 0) {
        length = (length - 1) >>> 0;    // 32-bit arithmetic.
        element = this[length];
        delete this[length];
    }
    
    this.length = length;
    return element;
};


Array.prototype.push = function() {
    var length = Object.toUInt32(this.length);
    
    for (var i = 0; i < arguments.length; ++i) {
        this[length] = arguments[i];
        length = (length + 1) >>> 0;    // 32-bit arithmetic.
    }
    
    this.length = length;
    return length;
};


Array.prototype.shift = function() {
    var length = Object.toUInt32(this.length);
    if (length == 0) {
        this.length = length;
        return;
    }
    
    var element = this[0];
    
    for (var i = 1; i < length; ++i) {
        if (i in this) {
            this[i - 1] = this[i];
        }
        else {
            delete this[i - 1];
        }
    }
    
    length = (length - 1) >>> 0;    // 32-bit arithmetic.
    delete this.length;
    this.length = length;
    return element;
};


Array.prototype.splice = function(start, deleteCount) {
    start = Object.toInteger(start);
    
    // Since length is a 32-bit unsigned integer, all operations with it
    // must use 32-bit arithmetic.
    var length = Object.toUInt32(this.length);
    var startIndex = (start < 0) ? Math.max((length + start) >>> 0, 0 >>> 0)
                                 : Math.min(start >>> 0, length);
    var limit = Math.min(Math.max(Object.toInteger(deleteCount)>>>0, 0>>>0),
                         (length - startIndex) >>> 0);
    var removedElements = [];
    
    for (var i = 0; i < limit; ++i) {
        var index = (startIndex + i) >>> 0;
        if (index in this) {
            removedElements[i] = this[index];
        }
    }
    
    removedElements.length = limit;
    var insertCount = (arguments.length - 2) >>> 0;
    
    if (insertCount > limit) {
        for (var i = (length - limit) >>> 0; i > startIndex; --i) {
            var sourceIndex = (i + limit - 1) >>> 0;
            var destIndex = (i + insertCount - 1) >>> 0;
            
            if (sourceIndex in this) {
                this[destIndex] = this[sourceIndex];
            }
            else {
                delete this[destIndex];
            }
        }
    }
    else if (insertCount < limit) {
        for (var i = startIndex; i < (length - limit) >>> 0; ++i) {
            var sourceIndex = (i + limit) >>> 0;
            var destIndex = (i + insertCount) >>> 0;
            
            if (sourceIndex in this) {
                this[destIndex] = this[sourceIndex];
            }
            else {
                delete this[destIndex];
            }
        }
        
        for (var i = length; i > (length - limit + insertCount)>>>0; --i) {
            delete this[i - 1];
        }
    }
    
    for (var i = startIndex, j = 2; j < (arguments.length>>>0); ++i, ++j) {
        this[i] = arguments[j];
    }
    
    this.length = (length - limit + insertCount) >>> 0;
    return removedElements;
};


Array.prototype.unshift = function() {
    // Since length is a 32-bit unsigned integer, all operations with it
    // must use 32-bit arithmetic.
    var length = Object.toUInt32(this.length);
    
    for (var k = length; k > 0; --k) {
        if (((k - 1) >>> 0) in this) {
            this[(k + arguments.length - 1) >>> 0] = this[(k - 1) >>> 0];
        }
        else {
            delete this[(k + arguments.length - 1) >>> 0];
        }
    }
    
    for (var k = 0; k < arguments.length; ++k) {
        this[k] = arguments[k];
    }
    
    this.length = (length + arguments.length) >>> 0;
    return this.length;
};


Array.prototype.concat = function(/* ... */) {
    var newArray = [];
    var element = this;
    var length = 0, i = 0;
    
    do {
        if (element instanceof Array) {
            for (var j = 0; j < element.length; ++j, ++length) {
                if (j in element) {
                    newArray[length] = element[j];
                }
            }
        }
        else {
            newArray[length] = element;
            ++length;
        }
        
        element = arguments[i];
    }
    while (i++ < arguments.length);
    
    newArray.length = length;
    return newArray;
};


Array.prototype.reverse = function() {
    var length = Object.toUInt32(this.length);
    var middle = Math.floor(length / 2);
    
    for (var i = 0; i < middle; ++i) {
        var rightPosition = length - i - 1;
        var leftElement = this[i];
        var rightElement = this[rightPosition];
        
        if (!(rightPosition in this)) {
            if (!(i in this)) {
                delete this[i];
                delete this[rightPosition];
            }
            else {
                delete this[i];
                this[rightPosition] = leftElement;
            }
        }
        else if (!(i in this)) {
            this[i] = rightElement;
            delete this[rightPosition];
        }
        else {
            this[i] = rightElement;
            this[rightPosition] = leftElement;
        }
    }
    
    return this;
};


Array.from = function(object) {
    var array = [];
    
    if (Array.like(object)) {
        for (var i = 0; i < object.length; ++i) {
            array.push(object[i]);
        }
    }
    
    return array;
};


/**
 * Creates an array from the given iterable object.
 *
 * @param {Object} iterable iterable object from which to create an array
 * @return an array from the given iterable object or an empty array if the
 *          iterable object is null or not defined
 * @type Array
 */
Array.from = function(iterable) {
    if ( !defined(iterable) || (iterable === null)) {
        return [];
    }
    if (typeof(iterable.toArray) == 'function') {
        return iterable.toArray();
    }
    
    var array = [];
    for (var i = 0; i < iterable.length; ++i) {
        array.push(iterable[i]);
    }
    return array;
};


Array.like = function(obj) {
    return obj != null && Number.like(obj.length) && (obj.length > 0);
};


Array.original = {
    join: Array.prototype.join,
};


Array.prototype.filter = function(include) {
    var result = [];
    
    for (var i = 0; i < this.length; ++i) {
        if (include(this[i])) {
            result.push(this[i]);
        }
    }
    
    return result;
};


Array.prototype.flatten = function() {
    var result = [];
    
    for (var i = 0; i < this.length; ++i) {
        if (Array.like(this[i])) {
            var values = Array.prototype.flatten.call(this[i]);
            Array.prototype.push.apply(result, values);
        }
        else {
            result.push(this[i]);
        }
    }
    
    return result;
};


/**
 * Concatenates the elements of this array.
 *
 * @param {String} separator optional separator (defaults to empty string)
 * @return string formed by concatenating all elements of this array
 * @type String
 */Array.prototype.join = function(separator) {
    return Array.original.join.call(this, separator || '');
};


Array.prototype.map = function(f) {
    var result = [];
    
    for (var i = 0; i < this.length; ++i) {
        result.push(f(this[i]));
    }
    
    return result;
};


/**
 * Gets the length of this array.
 *
 * @return number of elements in this array
 * @type Number
 */
Array.prototype.valueOf = function() {
    //return Object.toUInt32(this.length);
    return this.length;
};


Array.prototype.toString = function() {
    return this.join(', ');
};


Array.prototype.clone = function() {
    return this.slice(0);
};


Array.prototype.all = function(predicate) {
    for (var i = 0; i < this.length; ++i) {
        if (!predicate) {
            return false;
        }
    }
    return true;
};


/**
 * Returns an array equal to this array but without all the elements specified
 * in the arguments list.
 *
 * @return an array equal to this array but without all the elements specified
 *          in the arguments list
 * @type Array
 */
Array.prototype.without = function(/* ... */) {
    var args = arguments;
    
    return this.each(function(element) {
        return (Array.prototype.indexOf.call(args, element) < 0) ?
               element :
               void(0);
    });
};


/**
 * Inserts an element into the given index.
 *
 * @param element element to be inserted
 * @param index position at which to insert the element (if omitted, the element
 *              is added to the end of the array)
 * @return the array
 * @type Array
 */
Array.prototype.add = function(element, index) {
    if ( !defined(index)) {
        index = this.length;
    }
    
    if (index <= 0) {
        this.unshift(element);
    }
    else if (index < this.length) {
        this.splice(index, 0, element);
    }
    else {
        this.push(element);
    }
    
    return this;
};


/**
 * Calls the given function for each element in the array. The function receives
 * three arguments: the element, its index and the array being iterated.
 *
 * @param {Function} iterator function to be called for each element in the
 *                            array
 * @param {Object} thisObject optional object to use as the "this" for each
 *                            invocation of the function
 * @return an array with all values returned by the function in order, if any
 * @type Array
 */
Array.prototype.each = function(iterator, thisObject) {
    var values = [];
    
    for (var i = 0; i < this.length; ++i) {
        var value = iterator.call(thisObject, this[i], i, this);
        if (defined(value)) {
            values.push(value);
        }
    }
    
    return values;
};


// End of local scope.
})();
