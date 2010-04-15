/**
 * @fileoverview Ensures E262 conformance.
 * @author Márcio Faustino
 * @version 2010-04-15
 * @see http://www.mozilla.org/js/language/E262-3.pdf
 */


var E262 = new function() {
    var self = this;
    
    
    /**
     * Specification edition.
     */
    this.edition = 3;
    
    /**
     * White space characters.
     */
    this.whiteSpace = [
        '\u0009', '\u000A', '\u000B', '\u000C', '\u000D', '\u0020', '\u00A0',
        '\u1680', '\u180E', '\u2000', '\u2001', '\u2002', '\u2003', '\u2004',
        '\u2005', '\u2006', '\u2007', '\u2008', '\u2009', '\u200A', '\u2028',
        '\u2029', '\u202F', '\u205F', '\u3000'
    ];
    
    this._leftWhiteSpace = new RegExp('^[' + self.whiteSpace.join('') + ']+');
    
    
    /**
     * Gets the default (primitive) value of an object.
     *
     * @param object object for which to get the default value
     * @param hint optional hint for the default value, String or Number
     * @return default value for the given object
     * @type null, undefined, boolean, number, string
     * @throws SyntaxError The hint is an object other than String or Number.
     * @throws SyntaxError The object has no properties.
     * @throws TypeError The object has no default value.
     */
    this.getDefaultValue = function(object, hint) {
        if (object == undefined) {
            throw new SyntaxError('Object has no properties');
        }
        if (arguments.length == 1) {
            hint = object instanceof Date ? String : Number;
        }
        
        var methods;
        
        if (hint == String) {
            methods = ['toString', 'valueOf'];
        }
        else if (hint == Number) {
            methods = ['valueOf', 'toString'];
        }
        else {
            throw new SyntaxError('Hint must be String or Number');
        }
        
        for (var i = 0; i < methods.length; ++i) {
            if (typeof object[methods[i]] == 'function') {
                var value = object[methods[i]]();
                
                if (self.isPrimitive(value)) {
                    return value;
                }
            }
        }
        
        throw new TypeError("Object's default value isn't a primitive value");
    };
    
    
    /**
     * Gets the global object.
     *
     * @return default global object
     * @type Object
     */
    this.getGlobalObject = function() {
        // Nested functions have the global object as the receiver.
        return (function() {return this})();
    };
    
    
    /**
     * Checks if a value is a primitive value.
     *
     * A primitive value is an entity that belongs to one of the following
     * types: null, undefined, boolean, number or string.
     *
     * @param value value to check
     * @return true if the given value is a primitive value or false otherwise
     * @type boolean
     */
    this.isPrimitive = function(value) {
        return (value === null)
            || (value === undefined)
            || (typeof value == 'boolean')
            || (typeof value == 'number')
            || (typeof value == 'string');
    };
    
    
    /**
     * Converts a value to a boolean.
     *
     * @param value value to convert
     * @return corresponding boolean value
     * @type boolean
     */
    this.toBoolean = function(value) {
        return !!value;
    };
    
    
    /**
     * Converts a value to a signed 32 bit integer.
     *
     * @param value value to convert
     * @return corresponding signed 32 bit integer value
     * @type number
     */
    this.toInt32 = function(value) {
        return self.toInteger(value) | 0;
    };
    
    
    /**
     * Converts a value to an integer.
     *
     * @param value value to convert
     * @return corresponding integer value
     * @type number
     */
    this.toInteger = function(value) {
        var number = self.toNumber(value);
        
        if (isNaN(number)) {
            return 0;
        }
        else if ((number === 0) || (Math.abs(number) == Infinity)) {
            return number;
        }
        else {
            var sign = (number < 0 ? -1 : 1);
            return sign * Math.floor(Math.abs(number));
        }
    };
    
    
    /**
     * Converts a value to a number.
     *
     * @param value value to convert
     * @return corresponding number value
     * @type number
     */
    this.toNumber = function(value) {
        if (value === null) {
            return 0;
        }
        
        switch (typeof value) {
        case 'boolean':
            return value ? 1 : 0;
        case 'number':
            return value;
        case 'string':
            return +value.replace(self._leftWhiteSpace, '');
        case 'undefined':
            return NaN;
        default:
            return self.toNumber(self.toPrimitive(value, Number));
        }
    };
    
    
    /**
     * Converts a value to an object.
     *
     * @param value value to convert
     * @return corresponding Object value
     * @type Object
     * @throws TypeError The value to convert is null or undefined.
     */
    this.toObject = function(value) {
        if ((value === null) || (value === undefined)) {
            throw new TypeError(value + "can't be converted to an Object");
        }
        
        switch (typeof value) {
        case 'boolean':
            return new Boolean(value);
        case 'number':
            return new Number(value);
        case 'string':
            return new String(value);
        default:
            return value;
        }
    };
    
    
    /**
     * Converts a value to a primitive value.
     *
     * @param value value to convert
     * @param preferredType optional type to favour the conversion of the value
     *        to, if capable of converting to more than one primitive type
     * @return primitive value of the given value
     * @type Undefined, Null, Boolean, String, Number
     */
    this.toPrimitive = function(value, preferredType) {
        if (self.isPrimitive(value)) {
            return value;
        }
        else if (arguments.length == 2) {
            return self.getDefaultValue(value, preferredType);
        }
        else {
            return self.getDefaultValue(value);
        }
    };
    
    
    /**
     * Converts a value to a string.
     *
     * @param value value to convert
     * @return corresponding string value
     * @type string
     */
    this.toString = function(value) {
        return value + '';
    };
    
    
    /**
     * Converts a value to an unsigned 32 bit integer.
     *
     * @param value value to convert
     * @return corresponding unsigned 32 bit integer value
     * @type number
     */
    this.toUInt32 = function(value) {
        return self.toInt32(value) >>> 0;
    };
    
    
    this._apply = function(thisArg, argArray) {
        var name = 'Function.prototype.apply';
        
        if (typeof this != 'function') {
            throw new TypeError(name + ' called on incompatible object.');
        }
        
        if ((argArray != undefined)
                && !(argArray instanceof Array)
                && (typeof argArray.callee != 'function')) {
            throw new TypeError('Second parameter to ' + name
                + ' must be an array or arguments object.');
        }
        
        thisArg = (thisArg != undefined) ?
            self.toObject(thisArg) :
            self.getGlobalObject();
        
        return this._apply(thisArg, argArray || []);
    };
    
    
    this._indexOf = function(search, position) {
        search = self.toString(search);
        position = self.toInteger(position);
        
        var string = self.toString(this);
        var start = Math.min(Math.max(position, 0), string.length);
        
        SEARCH:
        for (var i = start; (i + search.length) <= string.length; ++i) {
            for (var j = 0; j < search.length; ++j) {
                if (string.charCodeAt(i + j) != search.charCodeAt(j)) {
                    continue SEARCH;
                }
            }
            
            return i;
        }
        
        return -1;
    };
    
    
    this._lastIndexOf = function(search, position) {
        search = self.toString(search);
        position = self.toNumber(position);
        
        var string = self.toString(this);
        var begin = isNaN(position) ? +Infinity : self.toInteger(position);
        var start = Math.min(Math.max(begin, 0), string.length) - search.length;
        
        SEARCH:
        for (var i = start; (i >= 0) && (i <= string.length); --i) {
            for (var j = 0; j < search.length; ++j) {
                if (string.charCodeAt(i + j) != search.charCodeAt(j)) {
                    continue SEARCH;
                }
            }
            
            return i;
        }
        
        return -1;
    };
    
    
    this._parseFloat = function(string) {
        var s = self.toString(string).replace(self._leftWhiteSpace, '');
        return (s === '') ? NaN : _parseFloat(s);
    };
    
    
    this._parseInt = function(string, radix) {
        var s = self.toString(string).replace(self._leftWhiteSpace, '');
        return (s === '') ? NaN : _parseInt(s, self.toInt32(radix));
    };
    
    
    this._unshift = function(/* ... */) {
        var length = this.length;
        
        this._unshift.apply(this, arguments);
        return self.toUInt32(length) + arguments.length;
    };
    
    
    // Detect implementation errors and fix them.
    
    var fixApply = false;
    
    try {
        (function() {}).apply(null, {});
        fixApply = true;
    }
    catch (exception) {
        if (!(exception instanceof TypeError)) {
            fixApply = true;
        }
    }
    
    if (fixApply) {
        Function.prototype._apply = Function.prototype.apply;
        Function.prototype.apply = self._apply;
    }
    
    if ((''.indexOf('', 1) != 0) || ('x'.indexOf('', 2) != 1)) {
        String.prototype._indexOf = String.prototype.indexOf;
        String.prototype.indexOf = self._indexOf;
    }
    
    if (('x'.lastIndexOf('x', -1) != -1) || (''.lastIndexOf('', -1) != 0)) {
        String.prototype._lastIndexOf = String.prototype.lastIndexOf;
        String.prototype.lastIndexOf = self._lastIndexOf;
    }
    
    for (var i = 0; i < self.whiteSpace.length; ++i) {
        if (parseFloat(self.whiteSpace[i] + '0') !== 0) {
            _parseFloat = parseFloat;
            parseFloat = self._parseFloat;
            break;
        }
    }
    
    for (var i = 0; i < self.whiteSpace.length; ++i) {
        if (parseInt(self.whiteSpace[i] + '0') !== 0) {
            _parseInt = parseInt;
            parseInt = self._parseInt;
            break;
        }
    }
    
    if ([].unshift() !== 0) {
        Array.prototype._unshift = Array.prototype.unshift;
        Array.prototype.unshift = self._unshift;
    }
    
    var globalObject = self.getGlobalObject();
    
    if ((globalObject + '') !== globalObject.toString()) {
        globalObject.toString = function() {
            return globalObject + '';
        };
    }
};
