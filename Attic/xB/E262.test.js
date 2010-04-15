/**
 * @fileoverview Tests E262 conformance.
 * @author Márcio Faustino
 * @version 2010-04-15
 * @requires Assert
 */


test('Private', {
    testDefaultValueOfUndefinedValueWithNumberHint: function() {
        assertException(SyntaxError, function() {
            E262.getDefaultValue(undefined, Number);
        });
    },
    
    testDefaultValueOfUndefinedValueWithStringHint: function() {
        assertException(SyntaxError, function() {
            E262.getDefaultValue(undefined, String);
        });
    },
    
    testDefaultValueOfNullValueWithNumberHint: function() {
        assertException(SyntaxError, function() {
            E262.getDefaultValue(null, Number);
        });
    },
    
    testDefaultValueOfNullValueWithStringHint: function() {
        assertException(SyntaxError, function() {
            E262.getDefaultValue(null, String);
        });
    },
    
    testDefaultValueOfBooleanValuesWithNumberHint: function() {
        assertEquals(E262.getDefaultValue(true, Number), true);
        assertEquals(E262.getDefaultValue(false, Number), false);
    },
    
    testDefaultValueOfBooleanValuesNumberWithStringHint: function() {
        assertEquals(E262.getDefaultValue(true, String), 'true');
        assertEquals(E262.getDefaultValue(false, String), 'false');
    },
    
    testDefaultValueOfNumberValuesWithNumberHint: function() {
        assertEquals(E262.getDefaultValue(-1, Number), -1);
        assertEquals(E262.getDefaultValue(0, Number), 0);
        assertEquals(E262.getDefaultValue(1, Number), 1);
        assertEquals(E262.getDefaultValue(1.23, Number), 1.23);
        assertNaN(E262.getDefaultValue(NaN, Number));
        assertEquals(E262.getDefaultValue(+Infinity, Number), +Infinity);
        assertEquals(E262.getDefaultValue(-Infinity, Number), -Infinity);
    },
    
    testDefaultValueOfNumberValuesWithStringHint: function() {
        assertEquals(E262.getDefaultValue(-1, String), '-1');
        assertEquals(E262.getDefaultValue(0, String), '0');
        assertEquals(E262.getDefaultValue(1, String), '1');
        assertEquals(E262.getDefaultValue(1.23, String), '1.23');
        assertEquals(E262.getDefaultValue(NaN, String), 'NaN');
        assertEquals(E262.getDefaultValue(+Infinity, String), 'Infinity');
        assertEquals(E262.getDefaultValue(-Infinity, String), '-Infinity');
    },
    
    testDefaultValueOfStringsWithNumberHint: function() {
        assertEquals(E262.getDefaultValue('', Number), '');
        assertEquals(E262.getDefaultValue('123', Number), '123');
    },
    
    testDefaultValueOfStringsWithStringHint: function() {
        assertEquals(E262.getDefaultValue('', String), '');
        assertEquals(E262.getDefaultValue('123', String), '123');
    },
    
    testDefaultValueOfObjectWithNumberHint: function() {
        assertEquals(E262.getDefaultValue({}, Number), '[object Object]');
    },
    
    testDefaultValueOfObjectWithStringHint: function() {
        assertEquals(E262.getDefaultValue({}, String), '[object Object]');
    },
    
    testDefaultValueOfObjectWithNumberHintAfterDeletingToString: function() {
        var object = {};
        assertTrue(delete object.toString);
        assertEquals(E262.getDefaultValue(object, Number), '[object Object]');
    },
    
    testDefaultValueOfObjectWithStringHintAfterDeletingToString: function() {
        var object = {};
        assertTrue(delete object.toString);
        assertEquals(E262.getDefaultValue(object, String), '[object Object]');
    },
    
    testDefaultValueOfObjectWithNumberHintAfterDeletingValueOf: function() {
        var object = {};
        assertTrue(delete object.valueOf);
        assertEquals(E262.getDefaultValue(object, Number), '[object Object]');
    },
    
    testDefaultValueOfObjectWithStringHintAfterDeletingValueOf: function() {
        var object = {};
        assertTrue(delete object.valueOf);
        assertEquals(E262.getDefaultValue(object, String), '[object Object]');
    },
    
    testDefaultValueWithNoHint: function() {
        assertEquals(E262.getDefaultValue('123'), '123');
        assertEquals(E262.getDefaultValue(123), 123);
    },
    
    testDefaultValueWithInvalidHint: function() {
        function Type() {}
        assertException(SyntaxError, function() {
            E262.getDefaultValue(123, Type);
        });
    },
    
    testConversionOfUndefinedValueToPrimitiveValue: function() {
        assertUndefined(E262.toPrimitive(undefined));
    },
    
    testConversionOfNullValueToPrimitiveValue: function() {
        assertNull(E262.toPrimitive(null));
    },
    
    testConversionOfBooleanValuesToPrimitiveValue: function() {
        assertEquals(E262.toPrimitive(true), true);
        assertEquals(E262.toPrimitive(false), false);
    },
    
    testConversionOfNumberValuesToPrimitiveValue: function() {
        assertEquals(E262.toPrimitive(-1), -1);
        assertEquals(E262.toPrimitive(0), 0);
        assertEquals(E262.toPrimitive(1), 1);
        assertEquals(E262.toPrimitive(1.23), 1.23);
        assertNaN(E262.toPrimitive(NaN));
        assertEquals(E262.toPrimitive(+Infinity), +Infinity);
        assertEquals(E262.toPrimitive(-Infinity), -Infinity);
    },
    
    testConversionOfStringsToPrimitiveValue: function() {
        assertEquals(E262.toPrimitive(''), '');
        assertEquals(E262.toPrimitive('123'), '123');
    },
    
    testConversionOfObjectToPrimitiveValue: function() {
        assertEquals(E262.toPrimitive({}), '[object Object]');
    },
    
    testNullValueIsPrimitiveValue: function() {
        assertTrue(E262.isPrimitive(null));
    },
    
    testUndefinedValueIsPrimitiveValue: function() {
        assertTrue(E262.isPrimitive(undefined));
    },
    
    testBooleanValuesArePrimitiveValues: function() {
        assertTrue(E262.isPrimitive(true));
        assertTrue(E262.isPrimitive(false));
    },
    
    testNumberValuesArePrimitiveValues: function() {
        assertTrue(E262.isPrimitive(0));
        assertTrue(E262.isPrimitive(1));
        assertTrue(E262.isPrimitive(-1));
        assertTrue(E262.isPrimitive(1.23));
        assertTrue(E262.isPrimitive(-1.23));
    },
    
    testInfinityIsPrimitiveValue: function() {
        assertTrue(E262.isPrimitive(+Infinity));
        assertTrue(E262.isPrimitive(-Infinity));
    },
    
    testNaNIsPrimitiveValue: function() {
        assertTrue(E262.isPrimitive(NaN));
    },
    
    testStringsArePrimitiveValues: function() {
        assertTrue(E262.isPrimitive(''));
        assertTrue(E262.isPrimitive('       '));
        assertTrue(E262.isPrimitive('123'));
        assertTrue(E262.isPrimitive('\n'));
    },
    
    testObjectsAreNotPrimitiveValues: function() {
        assertFalse(E262.isPrimitive({}));
        assertFalse(E262.isPrimitive(Object));
        assertFalse(E262.isPrimitive(Boolean));
        assertFalse(E262.isPrimitive(Number));
        assertFalse(E262.isPrimitive(String));
        assertFalse(E262.isPrimitive(new Boolean(true)));
        assertFalse(E262.isPrimitive(new Number(123)));
        assertFalse(E262.isPrimitive(new String('123')));
    },
    
    testConversionOfUndefinedToNumber: function() {
        assertNaN(E262.toNumber(undefined));
    },
    
    testConversionOfNullToNumber: function() {
        assertEquals(E262.toNumber(null), 0);
    },
    
    testConversionOfBooleanToNumber: function() {
        assertEquals(E262.toNumber(true), 1);
        assertEquals(E262.toNumber(false), 0);
    },
    
    testConversionOfNumbersToNumber: function() {
        assertEquals(E262.toNumber(-1), -1);
        assertEquals(E262.toNumber(0), 0);
        assertEquals(E262.toNumber(1), 1);
        assertEquals(E262.toNumber(1.23), 1.23);
        assertNaN(E262.toNumber(NaN));
        assertEquals(E262.toNumber(+Infinity), Infinity);
        assertEquals(E262.toNumber(-Infinity), -Infinity);
    },
    
    testConversionOfEmptyStringToNumber: function() {
        assertEquals(E262.toNumber(''), 0);
    },
    
    testConversionOfStringsWithWhiteSpaceToNumber: function() {
        for (var i = 0; i < E262.whiteSpace.length; ++i) {
            assertEquals(E262.toNumber(E262.whiteSpace[i]), 0);
        }
    },
    
    testConversionOfStringsWithInfinityToNumber: function() {
        var infinity = 1 / 0;
        assertEquals(E262.toNumber('Infinity'), infinity);
        assertEquals(E262.toNumber('+Infinity'), +infinity);
        assertEquals(E262.toNumber('-Infinity'), -infinity);
    },
    
    testConversionOfStringsWithIntegerToNumber: function() {
        assertEquals(E262.toNumber('1'), 1);
        assertEquals(E262.toNumber('+1'), 1);
        assertEquals(E262.toNumber('-1'), -1);
        assertEquals(E262.toNumber('0'), 0);
        assertEquals(E262.toNumber('+0'), 0);
        assertEquals(E262.toNumber('-0'), 0);
        
        assertEquals(E262.toNumber('1.'), 1);
        assertEquals(E262.toNumber('+1.'), 1);
        assertEquals(E262.toNumber('-1.'), -1);
        assertEquals(E262.toNumber('0.'), 0);
        assertEquals(E262.toNumber('+0.'), 0);
        assertEquals(E262.toNumber('-0.'), 0);
    },
    
    testConversionOfStringsWithDecimalToNumber: function() {
        assertEquals(E262.toNumber('1.23'), 1.23);
        assertEquals(E262.toNumber('+1.23'), 1.23);
        assertEquals(E262.toNumber('-1.23'), -1.23);
        
        assertEquals(E262.toNumber('.23'), 0.23);
        assertEquals(E262.toNumber('+.23'), 0.23);
        assertEquals(E262.toNumber('-.23'), -0.23);
    },
    
    testConversionOfStringsWithIntegerAndExponentToNumber: function() {
        var strings = {
            '1e2': 100,   '1.e2': 100,
            '1E2': 100,   '1.E2': 100,
            '1e+2': 100,  '1.e+2': 100,
            '1E+2': 100,  '1.E+2': 100,
            '1e-2': 0.01, '1.e-2': 0.01,
            '1E-2': 0.01, '1.E-2': 0.01
        };
        
        for (var string in strings) {
            assertEquals(E262.toNumber(string), strings[string]);
            assertEquals(E262.toNumber('+' + string), strings[string]);
            assertEquals(E262.toNumber('-' + string), -strings[string]);
        }
    },
    
    testConversionOfStringsWithDecimalAndExponentToNumber: function() {
        var strings = {
            '1.23e2': 123,     '.23e2': 23,
            '1.23E2': 123,     '.23E2': 23,
            '1.23e+2': 123,    '.23e+2': 23,
            '1.23E+2': 123,    '.23E+2': 23,
            '1.23e-2': 0.0123, '.23e-2': 0.0023,
            '1.23E-2': 0.0123, '.23E-2': 0.0023
        };
        
        for (var string in strings) {
            assertEquals(E262.toNumber(string), strings[string]);
            assertEquals(E262.toNumber('+' + string), strings[string]);
            assertEquals(E262.toNumber('-' + string), -strings[string]);
        }
    },
    
    testConversionOfStringsWithHexadecimalToNumber: function() {
        var strings = ['0xa', '0xA', '0Xa', '0XA'];
        for (var i = 0; i < strings.length; ++i) {
            assertEquals(E262.toNumber(strings[i]), 10);
        }
    },
    
    testConversionOfStringWithInvalidNumberToNumber: function() {
        assertNaN(E262.toNumber('Test.'));
    },
    
    testConversionOfUndefinedToInteger: function() {
        assertEquals(E262.toInteger(undefined), 0);
    },
    
    testConversionOfNullToInteger: function() {
        assertEquals(E262.toInteger(null), 0);
    },
    
    testConversionOfNaNToInteger: function() {
        assertEquals(E262.toInteger(NaN), 0);
    },
    
    testConversionOfInfinityToInteger: function() {
        assertEquals(E262.toInteger(+Infinity), Infinity);
        assertEquals(E262.toInteger(-Infinity), -Infinity);
    },
    
    testConversionOfIntegersToInteger: function() {
        assertEquals(E262.toInteger(-1), -1);
        assertEquals(E262.toInteger(0), 0);
        assertEquals(E262.toInteger(1), 1);
    },
    
    testConversionOfDecimalsToInteger: function() {
        assertEquals(E262.toInteger(1.23), 1);
        assertEquals(E262.toInteger(7.89), 7);
        assertEquals(E262.toInteger(-1.23), -1);
        assertEquals(E262.toInteger(-7.89), -7);
        assertEquals(E262.toInteger(0.0001), 0);
        assertEquals(E262.toInteger(-0.0001), 0);
    },
    
    testConversionOfUndefinedToInt32: function() {
        assertEquals(E262.toInt32(undefined), 0);
    },
    
    testConversionOfNullToInt32: function() {
        assertEquals(E262.toInt32(null), 0);
    },
    
    testConversionOfNaNToInt32: function() {
        assertEquals(E262.toInt32(NaN), 0);
    },
    
    testConversionOfInfinityToInt32: function() {
        assertEquals(E262.toInt32(+Infinity), 0);
        assertEquals(E262.toInt32(-Infinity), 0);
    },
    
    testConversionOfIntegersToInt32: function() {
        assertEquals(E262.toInt32(-1), -1);
        assertEquals(E262.toInt32(0), 0);
        assertEquals(E262.toInt32(1), 1);
        assertEquals(E262.toInt32(~0), -1);
        assertEquals(E262.toInt32(2147483647), 2147483647);
        assertEquals(E262.toInt32(2147483648), -2147483648);
    },
    
    testConversionOfDecimalsToInt32: function() {
        assertEquals(E262.toInt32(1.23), 1);
        assertEquals(E262.toInt32(7.89), 7);
        assertEquals(E262.toInt32(-1.23), -1);
        assertEquals(E262.toInt32(-7.89), -7);
        assertEquals(E262.toInt32(0.0001), 0);
        assertEquals(E262.toInt32(-0.0001), 0);
        assertEquals(E262.toInt32(2147483647.5), 2147483647);
        assertEquals(E262.toInt32(2147483648.5), -2147483648);
    },
    
    testConversionOfUndefinedToUInt32: function() {
        assertEquals(E262.toUInt32(undefined), 0);
    },
    
    testConversionOfNullToUInt32: function() {
        assertEquals(E262.toUInt32(null), 0);
    },
    
    testConversionOfNaNToUInt32: function() {
        assertEquals(E262.toUInt32(NaN), 0);
    },
    
    testConversionOfInfinityToUInt32: function() {
        assertEquals(E262.toUInt32(+Infinity), 0);
        assertEquals(E262.toUInt32(-Infinity), 0);
    },
    
    testConversionOfIntegersToUInt32: function() {
        assertEquals(E262.toUInt32(-1), 4294967295);
        assertEquals(E262.toUInt32(0), 0);
        assertEquals(E262.toUInt32(1), 1);
        assertEquals(E262.toUInt32(~0), 4294967295);
        assertEquals(E262.toUInt32(-~0), 1);
        assertEquals(E262.toUInt32(2147483647), 2147483647);
        assertEquals(E262.toUInt32(2147483648), 2147483648);
        assertEquals(E262.toUInt32(4294967295), 4294967295);
        assertEquals(E262.toUInt32(4294967296), 0);
        assertEquals(E262.toUInt32(4294967297), 1);
        assertEquals(E262.toUInt32(-4294967295), 1);
        assertEquals(E262.toUInt32(-4294967296), 0);
        assertEquals(E262.toUInt32(-4294967297), 4294967295);
    },
    
    testConversionOfDecimalsToUInt32: function() {
        assertEquals(E262.toUInt32(1.23), 1);
        assertEquals(E262.toUInt32(7.89), 7);
        assertEquals(E262.toUInt32(-1.23), 4294967295);
        assertEquals(E262.toUInt32(-7.89), 4294967289);
        assertEquals(E262.toUInt32(0.0001), 0);
        assertEquals(E262.toUInt32(-0.0001), 0);
        assertEquals(E262.toUInt32(2147483647.5), 2147483647);
        assertEquals(E262.toUInt32(2147483648.5), 2147483648);
    },
    
    testConversionOfUndefinedToBoolean: function() {
        assertFalse(E262.toBoolean(undefined));
    },
    
    testConversionOfNullToBoolean: function() {
        assertFalse(E262.toBoolean(null));
    },
    
    testConversionOfBooleanToBoolean: function() {
        assertTrue(E262.toBoolean(true));
        assertFalse(E262.toBoolean(false));
    },
    
    testConversionOfNumberToBoolean: function() {
        assertFalse(E262.toBoolean(0));
        assertFalse(E262.toBoolean(NaN));
        
        assertTrue(E262.toBoolean(-1));
        assertTrue(E262.toBoolean(1));
        assertTrue(E262.toBoolean(-1.23));
        assertTrue(E262.toBoolean(1.23));
        assertTrue(E262.toBoolean(+Infinity));
        assertTrue(E262.toBoolean(-Infinity));
    },
    
    testConversionOfStringToBoolean: function() {
        assertFalse(E262.toBoolean(''));
        
        assertTrue(E262.toBoolean(' '));
        assertTrue(E262.toBoolean('0'));
        assertTrue(E262.toBoolean('test'));
    },
    
    testConversionOfObjectToBoolean: function() {
        assertTrue(E262.toBoolean({}));
        assertTrue(E262.toBoolean(new Boolean(true)));
        assertTrue(E262.toBoolean(new Boolean(false)));
    },
    
    testConversionOfUndefinedValueToObject: function() {
        assertException(TypeError, function() {
            E262.toObject(undefined);
        });
    },
    
    testConversionOfNullToObject: function() {
        assertException(TypeError, function() {
            E262.toObject(null);
        });
    },
    
    testConversionOfObjectValueToObject: function() {
        assertTrue(E262.toObject({}) instanceof Object);
        assertTrue(E262.toObject(function() {}) instanceof Object);
    },
    
    testConversionOfStringValueToObject: function() {
        assertTrue(E262.toObject('') instanceof String);
        assertTrue(E262.toObject('123') instanceof String);
    },
    
    testConversionOfBooleanValueToObject: function() {
        assertTrue(E262.toObject(false) instanceof Boolean);
        assertTrue(E262.toObject(true) instanceof Boolean);
    },
    
    testConversionOfNumberValueToObject: function() {
        assertTrue(E262.toObject(-1) instanceof Number);
        assertTrue(E262.toObject(0) instanceof Number);
        assertTrue(E262.toObject(1) instanceof Number);
        assertTrue(E262.toObject(1.23) instanceof Number);
    },
    
    testConversionOfUndefinedValueToStringValue: function() {
        assertEquals(E262.toString(undefined), 'undefined');
    },
    
    testConversionOfNullValueToStringValue: function() {
        assertEquals(E262.toString(null), 'null');
    },
    
    testConversionOfBooleanValuesToStringValue: function() {
        assertEquals(E262.toString(true), 'true');
        assertEquals(E262.toString(false), 'false');
    },
    
    testConversionOfNumberValuesToStringValue: function() {
        assertEquals(E262.toString(-1), '-1');
        assertEquals(E262.toString(0), '0');
        assertEquals(E262.toString(1), '1');
        assertEquals(E262.toString(1.23), '1.23');
        assertEquals(E262.toString(NaN), 'NaN');
        assertEquals(E262.toString(+Infinity), 'Infinity');
        assertEquals(E262.toString(-Infinity), '-Infinity');
    },
    
    testConversionOfStringsToStringValue: function() {
        assertEquals(E262.toString(''), '');
        assertEquals(E262.toString('123'), '123');
    },
    
    testParsingIntegerOnStringsWithLeadingWhiteSpace: function() {
        for (var i = 0; i < E262.whiteSpace.length; ++i) {
            assertEquals(parseInt(E262.whiteSpace[i] + '0'), 0);
        }
    },
    
    testParsingFloatOnStringsWithLeadingWhiteSpace: function() {
        for (var i = 0; i < E262.whiteSpace.length; ++i) {
            assertEquals(parseFloat(E262.whiteSpace[i] + '0'), 0);
        }
    },
    
    testCallFunctionPrototypeApplyWithoutFunctionArguments: function() {
        var func = function() {return this};
        
        assertEquals(func.apply(null), E262.getGlobalObject());
        assertEquals(func.apply(undefined), E262.getGlobalObject());
        
        assertTrue(func.apply(false) instanceof Boolean);
        assertTrue(func.apply(0) instanceof Number);
        assertTrue(func.apply('') instanceof String);
        assertTrue(func.apply({}) instanceof Object);
    },
    
    testCallFunctionPrototypeApply: function() {
        var end = '.';
        
        var func = function(end) {
            return this.toString() + end;
        };
        
        assertEquals(func.apply(null, [end]), E262.getGlobalObject() + end);
        assertEquals(func.apply(undefined, [end]), E262.getGlobalObject() + end);
        
        assertEquals(func.apply(false, [end]), new Boolean(false) + end);
        assertEquals(func.apply(0, [end]), new Number(0) + end);
        assertEquals(func.apply('', [end]), new String('') + end);
        assertEquals(func.apply({}, [end]), new Object + end);
    },
    
    testCallFunctionPrototypeCallWithoutFunctionArguments: function() {
        var func = function() { return this; };
        
        assertEquals(func.call(null), E262.getGlobalObject());
        assertEquals(func.call(undefined), E262.getGlobalObject());
        
        assertTrue(func.call(false) instanceof Boolean);
        assertTrue(func.call(0) instanceof Number);
        assertTrue(func.call('') instanceof String);
        assertTrue(func.call({}) instanceof Object);
    },
    
    testCallFunctionPrototypeCall: function() {
        var end = '.';
        var func = function(end) {
            return this.toString() + end;
        };
        
        assertEquals(func.call(null, end), E262.getGlobalObject() + end);
        assertEquals(func.call(undefined, end), E262.getGlobalObject() + end);
        
        assertEquals(func.call(false, end), new Boolean(false) + end);
        assertEquals(func.call(0, end), new Number(0) + end);
        assertEquals(func.call('', end), new String('') + end);
        assertEquals(func.call({}, end), new Object + end);
    }
});


test('Public', {
    testUndefined: function() {
        assertEquals(undefined, void 0);
    },
    
    testNaN: function() {
        assertNotEquals(NaN, 0 / 0);
    },
    
    testInfinity: function() {
        assertEquals(+Infinity, +1 / 0);
        assertEquals(-Infinity, -1 / 0);
    },
    
    testParsingIntegerOnUndefinedValue: function() {
        assertNaN(parseInt(undefined));
    },
    
    testParsingIntegerOnNullValue: function() {
        assertNaN(parseInt(null));
    },
    
    testParsingIntegerOnEmptyString: function() {
        assertNaN(parseInt(''));
    },
    
    testParsingIntegerOnStringsWithIntegers: function() {
        assertEquals(parseInt('1'), 1);
        assertEquals(parseInt('+1'), 1);
        assertEquals(parseInt('-1'), -1);
        assertEquals(parseInt('0'), 0);
        assertEquals(parseInt('+0'), 0);
        assertEquals(parseInt('-0'), 0);
        
        assertEquals(parseInt('1.'), 1);
        assertEquals(parseInt('+1.'), 1);
        assertEquals(parseInt('-1.'), -1);
        assertEquals(parseInt('0.'), 0);
        assertEquals(parseInt('+0.'), 0);
        assertEquals(parseInt('-0.'), 0);
    },
    
    testParsingIntegerOnStringsWithDecimals: function() {
        assertEquals(parseInt('1.23'), 1);
        assertEquals(parseInt('+1.23'), 1);
        assertEquals(parseInt('-1.23'), -1);
        
        assertNaN(parseInt('.23'));
        assertNaN(parseInt('+.23'));
        assertNaN(parseInt('-.23'));
    },
    
    testParsingIntegerOnStringsWithIntegerAndExponent: function() {
        var strings = [
            '1e2', '1.e2', '1E2', '1.E2', '1e+2', '1.e+2', '1E+2', '1.E+2',
            '1e-2', '1.e-2', '1E-2', '1.E-2'
        ];
        
        for (var i = 0; i < strings.length; ++i) {
            assertEquals(parseInt(strings[i]), 1);
            assertEquals(parseInt('+' + strings[i]), 1);
            assertEquals(parseInt('-' + strings[i]), -1);
        }
    },
    
    testParsingIntegerOnStringsWithDecimalAndExponent: function() {
        var stringsGivingOne = [
            '1.23e2', '1.23E2', '1.23e+2', '1.23E+2', '1.23e-2', '1.23E-2'
        ];
        var stringsGivingNaN = [
            '.23e2', '.23E2', '.23e+2', '.23E+2', '.23e-2', '.23E-2'
        ];
        
        for (var i = 0; i < stringsGivingOne.length; ++i) {
            assertEquals(parseInt(stringsGivingOne[i]), 1);
            assertEquals(parseInt('+' + stringsGivingOne[i]), 1);
            assertEquals(parseInt('-' + stringsGivingOne[i]), -1);
        }
        
        for (var i = 0; i < stringsGivingNaN.length; ++i) {
            assertNaN(parseInt(stringsGivingNaN[i]));
            assertNaN(parseInt('+' + stringsGivingNaN[i]));
            assertNaN(parseInt('-' + stringsGivingNaN[i]));
        }
    },
    
    testParsingIntegerOnStringWithInvalidNumber: function() {
        assertNaN(parseInt('Test.'));
    },
    
    testParsingFloatOnUndefinedValue: function() {
        assertNaN(parseFloat(undefined));
    },
    
    testParsingFloatOnNullValue: function() {
        assertNaN(parseFloat(null));
    },
    
    testParsingFloatOnEmptyString: function() {
        assertNaN(parseFloat(''));
    },
    
    testParsingFloatOnStringsWithInfinity: function() {
        var expectedValue = 10e10000;
        assertEquals(parseFloat('Infinity'), expectedValue);
        assertEquals(parseFloat('+Infinity'), +expectedValue);
        assertEquals(parseFloat('-Infinity'), -expectedValue);
    },
    
    testParsingFloatOnStringsWithInteger: function() {
        assertEquals(parseFloat('1'), 1);
        assertEquals(parseFloat('+1'), 1);
        assertEquals(parseFloat('-1'), -1);
        assertEquals(parseFloat('0'), 0);
        assertEquals(parseFloat('+0'), 0);
        assertEquals(parseFloat('-0'), 0);
        
        assertEquals(parseFloat('1.'), 1);
        assertEquals(parseFloat('+1.'), 1);
        assertEquals(parseFloat('-1.'), -1);
        assertEquals(parseFloat('0.'), 0);
        assertEquals(parseFloat('+0.'), 0);
        assertEquals(parseFloat('-0.'), 0);
    },
    
    testParsingFloatOnStringsWithDecimal: function() {
        assertEquals(parseFloat('1.23'), 1.23);
        assertEquals(parseFloat('+1.23'), 1.23);
        assertEquals(parseFloat('-1.23'), -1.23);
        
        assertEquals(parseFloat('.23'), 0.23);
        assertEquals(parseFloat('+.23'), 0.23);
        assertEquals(parseFloat('-.23'), -0.23);
    },
    
    testParsingFloatOnStringsWithIntegerAndExponent: function() {
        var strings = {
            '1e2': 100,   '1.e2': 100,
            '1E2': 100,   '1.E2': 100,
            '1e+2': 100,  '1.e+2': 100,
            '1E+2': 100,  '1.E+2': 100,
            '1e-2': 0.01, '1.e-2': 0.01,
            '1E-2': 0.01, '1.E-2': 0.01
        };
        
        for (var string in strings) {
            assertEquals(parseFloat(string), strings[string]);
            assertEquals(parseFloat('+' + string), strings[string]);
            assertEquals(parseFloat('-' + string), -strings[string]);
        }
    },
    
    testParsingFloatOnStringsWithDecimalAndExponent: function() {
        var strings = {
            '1.23e2': 123,     '.23e2': 23,
            '1.23E2': 123,     '.23E2': 23,
            '1.23e+2': 123,    '.23e+2': 23,
            '1.23E+2': 123,    '.23E+2': 23,
            '1.23e-2': 0.0123, '.23e-2': 0.0023,
            '1.23E-2': 0.0123, '.23E-2': 0.0023
        };
        
        for (var string in strings) {
            assertEquals(parseFloat(string), strings[string]);
            assertEquals(parseFloat('+' + string), strings[string]);
            assertEquals(parseFloat('-' + string), -strings[string]);
        }
    },
    
    testParsingFloatOnStringWithInvalidNumber: function() {
        assertNaN(parseFloat('Test.'));
    },
    
    testParsingFloatOnStringWithInvalidSuffix: function() {
        assertEquals(parseFloat('1.23abc'), 1.23);
    },
    
    testNumbersAreNotNaN: function() {
        assertFalse(isNaN(-1));
        assertFalse(isNaN(0));
        assertFalse(isNaN(1));
        assertFalse(isNaN(1.23));
    },
    
    testInfinityIsNotNaN: function() {
        assertFalse(isNaN(+Infinity));
        assertFalse(isNaN(-Infinity));
    },
    
    testNaNIsNaN: function() {
        assertTrue(isNaN(NaN));
        assertTrue(isNaN(0 / 0));
    },
    
    testNumbersAreFinite: function() {
        assertTrue(isFinite(-1));
        assertTrue(isFinite(0));
        assertTrue(isFinite(1));
        assertTrue(isFinite(1.23));
    },
    
    testInfinityIsNotFinite: function() {
        assertFalse(isFinite(+Infinity));
        assertFalse(isFinite(-Infinity));
    },
    
    testNaNIsNotFinite: function() {
        assertFalse(isFinite(NaN));
        assertFalse(isFinite(0 / 0));
    },
    
    testURIDecoding: function() {
        var uri = '%7C%22%25%5C%C2%AB%C2%BB%C2%A3%C2%A7%7B%5B%5D%7D%3C%3E';
        assertEquals(decodeURI(uri), '|"%\\«»£§{[]}<>');
    },
    
    testURIComponentDecoding: function() {
        var uriComp = '%C3%A1%C3%A0%C2%BA%C2%AA%3B%2F%3F%3A%40%26%3D%2B%24%2C';
        assertEquals(decodeURIComponent(uriComp), 'áàºª;/?:@&=+$,');
    },
    
    testURIEncoding: function() {
        var uri = "%7C%22%25%5C%C2%AB%C2%BB%C2%A3%C2%A7%7B%5B%5D%7D%3C%3E#'";
        assertEquals(encodeURI('|"%\\«»£§{[]}<>#\''), uri);
    },
    
    testURIComponentEncoding: function() {
        var uriC = "%C3%A1%C3%A0%C2%BA%C2%AA%3B%2F%3F%3A%40%26%3D%2B%24%2C%23'";
        assertEquals(encodeURIComponent('áàºª;/?:@&=+$,#\''), uriC);
    },
    
    testObjectCreationFromAnUndefinedValue: function() {
        assertTrue(new Object(undefined) instanceof Object);
    },
    
    testObjectCreationFromNull: function() {
        assertTrue(new Object(null) instanceof Object);
    },
    
    testObjectCreationFromObject: function() {
        assertTrue(new Object({}) instanceof Object);
        assertTrue(new Object(function() {}) instanceof Object);
    },
    
    testObjectCreationFromString: function() {
        assertTrue(new Object('') instanceof String);
        assertTrue(new Object('123') instanceof String);
    },
    
    testObjectCreationFromBoolean: function() {
        assertTrue(new Object(true) instanceof Boolean);
        assertTrue(new Object(false) instanceof Boolean);
    },
    
    testObjectCreationFromNumber: function() {
        assertTrue(new Object(-1) instanceof Number);
        assertTrue(new Object(0) instanceof Number);
        assertTrue(new Object(1) instanceof Number);
        assertTrue(new Object(1.23) instanceof Number);
    },
    
    testCallObjectConstructorWithAnUndefinedValue: function() {
        assertTrue(Object(undefined) instanceof Object);
    },
    
    testCallObjectConstructorWithNull: function() {
        assertTrue(Object(null) instanceof Object);
    },
    
    testCallObjectConstructorWithObject: function() {
        assertTrue(Object({}) instanceof Object);
        assertTrue(Object(function() {}) instanceof Object);
    },
    
    testCallObjectConstructorWithString: function() {
        assertTrue(Object('') instanceof String);
        assertTrue(Object('123') instanceof String);
    },
    
    testCallObjectConstructorWithBoolean: function() {
        assertTrue(Object(true) instanceof Boolean);
        assertTrue(Object(false) instanceof Boolean);
    },
    
    testCallObjectConstructorWithNumber: function() {
        assertTrue(Object(-1) instanceof Number);
        assertTrue(Object(0) instanceof Number);
        assertTrue(Object(1) instanceof Number);
        assertTrue(Object(1.23) instanceof Number);
    },
    
    testObjectPrototypeConstructorPropertyIsObjectConstructor: function() {
        assertEquals(Object.prototype.constructor, Object);
    },
    
    testCallObjectPrototypeToString: function() {
        assertEquals(Object.prototype.toString(), '[object Object]');
        assertEquals(({}).toString(), '[object Object]');
    },
    
    testCallObjectPrototypeToLocaleString: function() {
        assertEquals(Object.prototype.toLocaleString(),
                     Object.prototype.toString());
        assertEquals(({}).toLocaleString(), ({}).toString());
    },
    
    testCallObjectPrototypeValueOf: function() {
        assertEquals(Object.prototype.valueOf(), Object.prototype);
        
        var emptyObject = {};
        assertEquals(emptyObject.valueOf(), emptyObject);
    },
    
    testCallObjectPrototypeHasOwnProperty: function() {
        assertTrue(Object.prototype.hasOwnProperty('hasOwnProperty'));
        assertTrue(({property: undefined}).hasOwnProperty('property'));
    },
    
    testCallObjectPrototypeHasOwnPropertyOnNotOwnedProperty: function() {
        assertFalse(({}).hasOwnProperty('hasOwnProperty'));
        assertFalse(({}).hasOwnProperty('toString'));
    },
    
    testCallObjectPrototypeIsPrototypeOfOnNonObject: function() {
        assertFalse(Object.prototype.isPrototypeOf(''));
    },
    
    testCallObjectPrototypeIsPrototypeOf: function() {
        assertTrue(Object.prototype.isPrototypeOf(Object));
        assertTrue(Object.prototype.isPrototypeOf({}));
        
        var Type = function() {};
        assertTrue(Type.prototype.isPrototypeOf(new Type));
        assertTrue(Object.prototype.isPrototypeOf(new Type));
        assertFalse(Type.prototype.isPrototypeOf({}));
    },
    
    testCallObjectPrototypePropertyIsEnumerable: function() {
        assertTrue(({property: undefined}).propertyIsEnumerable('property'));
    },
    
    testCallObjectPrototypePropertyIsEnumerableOnNonEnumerableProperty: function() {
        assertFalse(Object.prototype.propertyIsEnumerable('toString'));
        assertFalse(({}).propertyIsEnumerable('toString'));
    },
    
    testFunctionCreationWithoutArgumentsAndBody: function() {
        assertEquals(typeof new Function, 'function');
    },
    
    testFunctionCreationWithoutArgumentsButEmptyBody: function() {
        assertEquals(typeof new Function(''), 'function');
    },
    
    testFunctionCreationWithInvalidArgumentList: function() {
        assertException(SyntaxError, function() {
            new Function('**invalid argument list**', '');
        });
    },
    
    testFunctionCreationWithInvalidBody: function() {
        assertException(SyntaxError, function() {
            new Function('**invalid body**');
        });
    },
    
    testFunctionCreationUsingOneArgumentPerFormalParameter: function() {
        assertEquals(typeof new Function('a', 'b', 'c', ''), 'function');
    },
    
    testFunctionCreationUsingOneArgumentForAllFormalParameters: function() {
        assertEquals(typeof new Function('a, b, c', ''), 'function');
    },
    
    testFunctionCreationUsingMixedModeForSpecifyingFormalParameters: function() {
        assertEquals(typeof new Function('a, b', 'c', ''), 'function');
    },
    
    testFunctionCreationAndCallsTheCreatedFunction: function() {
        assertEquals(new Function('x', 'return x;')(1), 1);
    },
    
    testCallFunctionConstructorWithoutArgumentsAndBody: function() {
        assertEquals(typeof Function(), 'function');
    },
    
    testCallFunctionConstructorWithoutArgumentsButEmptyBody: function() {
        assertEquals(typeof Function(''), 'function');
    },
    
    testCallFunctionConstructorWithInvalidArgumentList: function() {
        assertException(SyntaxError, function() {
            Function('**invalid argument list**', '');
        });
    },
    
    testCallFunctionConstructorWithInvalidBody: function() {
        assertException(SyntaxError, function() {
            Function('**invalid body**');
        });
    },
    
    testCallFunctionConstructorUsingOneArgumentPerFormalParameter: function() {
        assertEquals(typeof Function('a', 'b', 'c', ''), 'function');
    },
    
    testCallFunctionConstructorUsingOneArgumentForAllFormalParameters: function() {
        assertEquals(typeof Function('a, b, c', ''), 'function');
    },
    
    testCallFunctionConstructorUsingMixedModeForSpecifyingFormalParameters: function() {
        assertEquals(typeof Function('a, b', 'c', ''), 'function');
    },
    
    testCallFunctionConstructorAndCallsTheCreatedFunction: function() {
        assertEquals(Function('x', 'return x;')(1), 1);
    },
    
    testFunctionPrototypePropertyIsEmptyFunction: function() {
        assertEquals(typeof Function.prototype, 'function');
        
        assertUndefined(Function.prototype());
        assertUndefined(Function.prototype(0));
        assertUndefined(Function.prototype(''));
        assertUndefined(Function.prototype(true));
        assertUndefined(Function.prototype({}));
        assertUndefined(Function.prototype(1, 'abc', false, Object));
    },
    
    testFunctionPrototypeConstructorPropertyIsFunctionConstructor: function() {
        assertEquals(Function.prototype.constructor, Function);
    },
    
    testCallFunctionPrototypeToString: function() {
        // Nothing to assert, because no exception must be thrown:
        Function.prototype.toString();
        (function() {}).toString();
    },
    
    testCallFunctionPrototypeApplyOnIncompatibleObject: function() {
        var object = {apply: Function.prototype.apply};
        
        assertException(TypeError, function() {
            object.apply(null, []);
        });
    },
    
    testCallFunctionPrototypeApplyWithInvalidFunctionArguments: function() {
        var func = function() {};
        
        assertException(TypeError, function() {
            func.apply(null, false);
        });
        assertException(TypeError, function() {
            func.apply(null, 0);
        });
        assertException(TypeError, function() {
            func.apply(null, '');
        });
        assertException(TypeError, function() {
            func.apply(null, {});
        });
    },
    
    testCallFunctionPrototypeCallOnIncompatibleObject: function() {
        var object = {call: Function.prototype.call};
        
        assertException(TypeError, function() {
            object.call(null);
        });
    },
    
    testArrayCreationByLengthWithInvalidLength: function() {
        var array = new Array(null);
        assertEquals(array.length, 1);
        assertEquals(array[0], null);
    },
    
    testArrayCreationByLengthWithOverflowingLength: function() {
        assertException(RangeError, function() {
            new Array(9999999999);
        });
    },
    
    testArrayCreation: function() {
        assertEquals(new Array().length, 0);
        
        var array1 = new Array('x', 'y');
        assertEquals(array1.length, 2);
        assertEquals(array1[0], 'x');
        assertEquals(array1[1], 'y');
        
        var array2 = new Array(null, true, undefined, false);
        assertEquals(array2.length, 4);
        assertEquals(array2[0], null);
        assertEquals(array2[1], true);
        assertEquals(array2[2], undefined);
        assertEquals(array2[3], false);
    },
    
    testCallArrayConstructorWithInvalidLength: function() {
        var array = Array(null);
        assertEquals(array.length, 1);
        assertEquals(array[0], null);
    },
    
    testCallArrayConstructorWithOverflowingLength: function() {
        assertException(RangeError, function() {
            Array(9999999999);
        });
    },
    
    testCallArrayConstructor: function() {
        assertEquals(Array().length, 0);
        
        var array1 = Array('x', 'y');
        assertEquals(array1.length, 2);
        assertEquals(array1[0], 'x');
        assertEquals(array1[1], 'y');
        
        var array2 = Array(null, true, undefined, false);
        assertEquals(array2.length, 4);
        assertEquals(array2[0], null);
        assertEquals(array2[1], true);
        assertEquals(array2[2], undefined);
        assertEquals(array2[3], false);
    },
    
    testArrayPrototypeConstructorPropertyIsArrayConstructor: function() {
        assertEquals(Array.prototype.constructor, Array);
    },
    
    testCallArrayPrototypeConcatOnEmptyArrayWithNoArguments: function() {
        var array = [];
        var newArray = array.concat();
        
        assertEquals(newArray.length, 0);
        assertTrue(newArray !== array);
    },
    
    testCallArrayPrototypeConcatOnEmptyArray: function() {
        var array = [];
        var newArray = array.concat(undefined, null, true, 1.23, 'a', {});
        
        assertEquals(newArray.length, 6);
        assertTrue(newArray !== array);
    },
    
    testCallArrayPrototypeConcatWithNoArguments: function() {
        var array = [1];
        var newArray = array.concat();
        
        assertEquals(newArray.length, 1);
        assertTrue(newArray !== array);
    },
    
    testCallArrayPrototypeConcat: function() {
        var array = [1];
        var newArray = array.concat(undefined, null, true, 1.23, 'a', {});
        
        assertEquals(newArray.length, 7);
        assertTrue(newArray !== array);
    },
    
    testCallArrayPrototypeJoinOnEmptyArrayWithNoSeparator: function() {
        assertEquals([].join(), '');
    },
    
    testCallArrayPrototypeJoinOnEmptyArrayWithNullSeparator: function() {
        assertEquals([].join(null), '');
    },
    
    testCallArrayPrototypeJoinOnEmptyArray: function() {
        assertEquals([].join(', '), '');
    },
    
    testCallArrayPrototypeJoinWithNoSeparator: function() {
        assertEquals([1, 2, 3].join(), '1,2,3');
    },
    
    testCallArrayPrototypeJoinWithNullSeparator: function() {
        assertEquals([1, 2, 3].join(null), '1null2null3');
    },
    
    testCallArrayPrototypeJoin: function() {
        assertEquals([1, 2, 3].join(', '), '1, 2, 3');
    },
    
    testCallArrayPrototypeToStringOnEmptyArray: function() {
        assertEquals([].toString(), '');
    },
    
    testCallArrayPrototypeToString: function() {
        assertEquals([1, 2, 3].toString(), '1,2,3');
    },
    
    testCallArrayPrototypePopOnEmptyArray: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertUndefined(array.pop());
        assertEquals(array.length, 0);
    },
    
    testCallArrayPrototypePop: function() {
        var array = ['a', 'b', 'c'];
        
        assertEquals(array.length, 3);
        assertEquals(array.pop(), 'c');
        assertEquals(array.length, 2);
    },
    
    testCallArrayPrototypePushWithNoElementsOnEmptyArray: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.push(), 0);
        assertEquals(array.length, 0);
    },
    
    testCallArrayPrototypePushWithNoElements: function() {
        var array = ['a', 'b', 'c'];
        
        assertEquals(array.length, 3);
        assertEquals(array.push(), 3);
        assertEquals(array.length, 3);
    },
    
    testCallArrayPrototypePushWithSingleElementOnEmptyArray: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.push('abc'), 1);
        assertEquals(array.length, 1);
        assertEquals(array[0], 'abc');
    },
    
    testCallArrayPrototypePushWithSingleElement: function() {
        var array = ['a', 'b', 'c'];
        
        assertEquals(array.length, 3);
        assertEquals(array.push('abc'), 4);
        assertEquals(array.length, 4);
        assertEquals(array[3], 'abc');
    },
    
    testCallArrayPrototypePushOnEmptyArray: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.push(1, 2, 3), 3);
        assertEquals(array.length, 3);
        assertEquals(array[0], 1);
        assertEquals(array[1], 2);
        assertEquals(array[2], 3);
    },
    
    testCallArrayPrototypePush: function() {
        var array = ['a', 'b', 'c'];
        
        assertEquals(array.length, 3);
        assertEquals(array.push(1, 2, 3), 6);
        assertEquals(array.length, 6);
        assertEquals(array[3], 1);
        assertEquals(array[4], 2);
        assertEquals(array[5], 3);
    },
    
    testCallArrayPrototypeReverseOnEmptyArray: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.reverse(), array);
        assertEquals(array.length, 0);
    },
    
    testCallArrayPrototypeReverseOnArrayWithOddLength: function() {
        var array = [1, 2, 3];
        
        assertEquals(array.length, 3);
        assertEquals(array.reverse(), array);
        assertEquals(array.length, 3);
        assertEquals(array[0], 3);
        assertEquals(array[1], 2);
        assertEquals(array[2], 1);
    },
    
    testCallArrayPrototypeReverseOnArrayWithEvenLength: function() {
        var array = [1, 2];
        
        assertEquals(array.length, 2);
        assertEquals(array.reverse(), array);
        assertEquals(array.length, 2);
        assertEquals(array[0], 2);
        assertEquals(array[1], 1);
    },
    
    testCallArrayPrototypeReverseChained: function() {
        var array = [1, 2, 3];
        
        assertEquals(array.length, 3);
        assertEquals(array.reverse().reverse(), array);
        assertEquals(array.length, 3);
        assertEquals(array[0], 1);
        assertEquals(array[1], 2);
        assertEquals(array[2], 3);
    },
    
    testCallArrayPrototypeShiftOnEmptyArray: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertUndefined(array.shift());
        assertEquals(array.length, 0);
    },
    
    testCallArrayPrototypeShift: function() {
        var array = ['a', 'b', 'c'];
        
        assertEquals(array.length, 3);
        assertEquals(array.shift(), 'a');
        assertEquals(array.length, 2);
    },
    
    testCallArrayPrototypeSpliceOnEmptyArrayAtZeroIndex: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.splice(0, 0).length, 0);
        assertEquals(array.length, 0);
    },
    
    testCallArrayPrototypeSpliceOnEmptyArrayAtPositiveIndex: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.splice(3, 0).length, 0);
        assertEquals(array.length, 0);
    },
    
    testCallArrayPrototypeSpliceOnEmptyArrayAtNegativeIndex: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.splice(-3, 0).length, 0);
        assertEquals(array.length, 0);
    },
    
    testCallArrayPrototypeSpliceOnEmptyArrayAtZeroIndexToRemoveElements: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.splice(0, 3).length, 0);
        assertEquals(array.length, 0);
    },
    
    testCallArrayPrototypeSpliceOnEmptyArrayAtPositiveIndexToRemoveElements: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.splice(3, 3).length, 0);
        assertEquals(array.length, 0);
    },
    
    testCallArrayPrototypeSpliceOnEmptyArrayAtNegativeIndexToRemoveElements: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.splice(-3, 3).length, 0);
        assertEquals(array.length, 0);
    },
    
    testCallArrayPrototypeSpliceOnEmptyArrayAtZeroIndexToInsertElements: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.splice(0, 0, 1, 2, 3).length, 0);
        assertEquals(array.length, 3);
        
        assertEquals(array[0], 1);
        assertEquals(array[1], 2);
        assertEquals(array[2], 3);
    },
    
    testCallArrayPrototypeSpliceOnEmptyArrayAtPositiveIndexToInsertElements: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.splice(3, 0, 1, 2, 3).length, 0);
        assertEquals(array.length, 3);
        
        assertEquals(array[0], 1);
        assertEquals(array[1], 2);
        assertEquals(array[2], 3);
    },
    
    testCallArrayPrototypeSpliceOnEmptyArrayAtNegativeIndexToInsertElements: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.splice(-3, 0, 1, 2, 3).length, 0);
        assertEquals(array.length, 3);
        
        assertEquals(array[0], 1);
        assertEquals(array[1], 2);
        assertEquals(array[2], 3);
    },
    
    testCallArrayPrototypeSpliceOnEmptyArrayAtZeroIndexWithElements: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.splice(0, 5, 1, 2, 3).length, 0);
        assertEquals(array.length, 3);
        
        assertEquals(array[0], 1);
        assertEquals(array[1], 2);
        assertEquals(array[2], 3);
    },
    
    testCallArrayPrototypeSpliceOnEmptyArrayAtPositiveIndexWithElements: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.splice(3, 5, 1, 2, 3).length, 0);
        assertEquals(array.length, 3);
        
        assertEquals(array[0], 1);
        assertEquals(array[1], 2);
        assertEquals(array[2], 3);
    },
    
    testCallArrayPrototypeSpliceOnEmptyArrayAtNegativeIndexWithElements: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.splice(-3, 5, 1, 2, 3).length, 0);
        assertEquals(array.length, 3);
        
        assertEquals(array[0], 1);
        assertEquals(array[1], 2);
        assertEquals(array[2], 3);
    },
    
    testCallArrayPrototypeSpliceAtZeroIndex: function() {
        var array = [1, 2, 3];
        
        assertEquals(array.length, 3);
        assertEquals(array.splice(0, 0).length, 0);
        assertEquals(array.length, 3);
    },
    
    testCallArrayPrototypeSpliceAtPositiveIndex: function() {
        var array = [1, 2, 3];
        
        assertEquals(array.length, 3);
        assertEquals(array.splice(2, 0).length, 0);
        assertEquals(array.length, 3);
    },
    
    testCallArrayPrototypeSpliceWithOverflowStartIndex: function() {
        var array = [1, 2, 3];
        
        assertEquals(array.length, 3);
        assertEquals(array.splice(5, 0).length, 0);
        assertEquals(array.length, 3);
    },
    
    testCallArrayPrototypeSpliceAtNegativeIndex: function() {
        var array = [1, 2, 3];
        
        assertEquals(array.length, 3);
        assertEquals(array.splice(-2, 0).length, 0);
        assertEquals(array.length, 3);
    },
    
    testCallArrayPrototypeSpliceToRemoveElementsAtZeroIndex: function() {
        var array = [1, 2, 3];
        var removed = array.splice(0, 2);
        
        assertEquals(array.length, 1);
        assertEquals(removed.length, 2);
        
        assertEquals(array[0], 3);
        assertEquals(removed[0], 1);
        assertEquals(removed[1], 2);
    },
    
    testCallArrayPrototypeSpliceToRemoveElementsAtPositiveIndex: function() {
        var array = [1, 2, 3];
        var removed = array.splice(1, 2);
        
        assertEquals(array.length, 1);
        assertEquals(removed.length, 2);
        
        assertEquals(array[0], 1);
        assertEquals(removed[0], 2);
        assertEquals(removed[1], 3);
    },
    
    testCallArrayPrototypeSpliceToRemoveElementsWithOverflowStartIndex: function() {
        var array = [1, 2, 3];
        
        assertEquals(array.length, 3);
        assertEquals(array.splice(3, 3).length, 0);
        assertEquals(array.length, 3);
    },
    
    testCallArrayPrototypeSpliceToRemoveElementsAtNegativeIndex: function() {
        var array = [1, 2, 3];
        var removed = array.splice(-2, 2);
        
        assertEquals(array.length, 1);
        assertEquals(removed.length, 2);
        
        assertEquals(array[0], 1);
        assertEquals(removed[0], 2);
        assertEquals(removed[1], 3);
    },
    
    testCallArrayPrototypeSpliceToInsertElementsAtZeroIndex: function() {
        var array = [1, 2, 3];
        
        assertEquals(array.length, 3);
        assertEquals(array.splice(0, 0, -1, 0).length, 0);
        assertEquals(array.length, 5);
        
        assertEquals(array[0], -1);
        assertEquals(array[1], 0);
        assertEquals(array[2], 1);
        assertEquals(array[3], 2);
        assertEquals(array[4], 3);
    },
    
    testCallArrayPrototypeSpliceToInsertElementsAtPositiveIndex: function() {
        var array = [1, 2, 3];
        
        assertEquals(array.length, 3);
        assertEquals(array.splice(1, 0, 1.5).length, 0);
        assertEquals(array.length, 4);
        
        assertEquals(array[0], 1);
        assertEquals(array[1], 1.5);
        assertEquals(array[2], 2);
        assertEquals(array[3], 3);
    },
    
    testCallArrayPrototypeSpliceToInsertElementsWithOverflowStartIndex: function() {
        var array = [1, 2, 3];
        
        assertEquals(array.length, 3);
        assertEquals(array.splice(3, 0, 4, 5).length, 0);
        assertEquals(array.length, 5);
        
        assertEquals(array[0], 1);
        assertEquals(array[1], 2);
        assertEquals(array[2], 3);
        assertEquals(array[3], 4);
        assertEquals(array[4], 5);
    },
    
    testCallArrayPrototypeSpliceToInsertElementsAtNegativeIndex: function() {
        var array = [1, 2, 3];
        
        assertEquals(array.length, 3);
        assertEquals(array.splice(-2, 0, 1.5).length, 0);
        assertEquals(array.length, 4);
        
        assertEquals(array[0], 1);
        assertEquals(array[1], 1.5);
        assertEquals(array[2], 2);
        assertEquals(array[3], 3);
    },
    
    testCallArrayPrototypeSpliceAtZeroIndexWithElements: function() {
        var array = [1, 2, 3];
        var removed = array.splice(0, 2, 5, 4);
        
        assertEquals(array.length, 3);
        assertEquals(removed.length, 2);
        
        assertEquals(array[0], 5);
        assertEquals(array[1], 4);
        assertEquals(array[2], 3);
        assertEquals(removed[0], 1);
        assertEquals(removed[1], 2);
    },
    
    testCallArrayPrototypeSpliceAtPositiveIndexWithElements: function() {
        var array = [1, 2, 3];
        var removed = array.splice(1, 5, 2.2, 3.3);
        
        assertEquals(array.length, 3);
        assertEquals(removed.length, 2);
        
        assertEquals(array[0], 1);
        assertEquals(array[1], 2.2);
        assertEquals(array[2], 3.3);
        assertEquals(removed[0], 2);
        assertEquals(removed[1], 3);
    },
    
    testCallArrayPrototypeSpliceWithOverflowStartIndexWithElements: function() {
        var array = [1, 2, 3];
        
        assertEquals(array.length, 3);
        assertEquals(array.splice(3, 5, 2, 1).length, 0);
        assertEquals(array.length, 5);
        
        assertEquals(array[0], 1);
        assertEquals(array[1], 2);
        assertEquals(array[2], 3);
        assertEquals(array[3], 2);
        assertEquals(array[4], 1);
    },
    
    testCallArrayPrototypeSpliceAtNegativeIndexWithElements: function() {
        var array = [1, 2, 3];
        var removed = array.splice(-2, 5, 2.2, 3.3);
        
        assertEquals(array.length, 3);
        assertEquals(removed.length, 2);
        
        assertEquals(array[0], 1);
        assertEquals(array[1], 2.2);
        assertEquals(array[2], 3.3);
        assertEquals(removed[0], 2);
        assertEquals(removed[1], 3);
    },
    
    testCallArrayPrototypeSliceOnEmptyArray: function() {
        assertEquals([].slice(0).length, 0);
        assertEquals([].slice(0, 0).length, 0);
    },
    
    testCallArrayPrototypeSliceWithNegativeStartPosition: function() {
        var slice1 = [1, 2, 3].slice(-2);
        assertEquals(slice1.length, 2);
        assertEquals(slice1[0], 2);
        assertEquals(slice1[1], 3);
        
        var slice2 = [1, 2, 3].slice(-2, 2);
        assertEquals(slice2.length, 1);
        assertEquals(slice2[0], 2);
    },
    
    testCallArrayPrototypeSliceWithNegativeEndPosition: function() {
        var slice = [1, 2, 3].slice(0, -2);
        assertEquals(slice.length, 1);
        assertEquals(slice[0], 1);
    },
    
    testCallArrayPrototypeSliceWithNegativePositions: function() {
        var slice = [1, 2, 3].slice(-3, -2);
        assertEquals(slice.length, 1);
        assertEquals(slice[0], 1);
    },
    
    testCallArrayPrototypeSlice: function() {
        var slice = [1, 2, 3].slice(1, 2);
        assertEquals(slice.length, 1);
        assertEquals(slice[0], 2);
    },
    
    testCallArrayPrototypeSortOnEmptyArray: function() {
        var array = [];
        assertEquals(array.length, 0);
        assertEquals(array.sort(), array);
        assertEquals(array.length, 0);
    },
    
    testCallArrayPrototypeSort: function() {
        var numbers = [1, 3, 2];
        assertEquals(numbers.length, 3);
        assertEquals(numbers.sort(), numbers);
        assertEquals(numbers.length, 3);
        assertEquals(numbers[0], 1);
        assertEquals(numbers[1], 2);
        assertEquals(numbers[2], 3);
        
        var chars = ['b', 'a', 'c'];
        assertEquals(chars.length, 3);
        assertEquals(chars.sort(), chars);
        assertEquals(chars.length, 3);
        assertEquals(chars[0], 'a');
        assertEquals(chars[1], 'b');
        assertEquals(chars[2], 'c');
    },
    
    testCallArrayPrototypeSortWithCustomComparator: function() {
        var descending = {
            numbers: function(x, y) {
                return y - x;
            },
            chars: function(a, b) {
                return b.charCodeAt(0) - a.charCodeAt(0);
            }
        };
        
        var numbers = [1, 3, 2];
        assertEquals(numbers.length, 3);
        assertEquals(numbers.sort(descending.numbers), numbers);
        assertEquals(numbers.length, 3);
        assertEquals(numbers[0], 3);
        assertEquals(numbers[1], 2);
        assertEquals(numbers[2], 1);
        
        var chars = ['b', 'a', 'c'];
        assertEquals(chars.length, 3);
        assertEquals(chars.sort(descending.chars), chars);
        assertEquals(chars.length, 3);
        assertEquals(chars[0], 'c');
        assertEquals(chars[1], 'b');
        assertEquals(chars[2], 'a');
    },
    
    testCallArrayPrototypeUnshiftWithNoElementsOnEmptyArray: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.unshift(), 0);
        assertEquals(array.length, 0);
    },
    
    testCallArrayPrototypeUnshiftWithNoElements: function() {
        var array = ['a', 'b', 'c'];
        
        assertEquals(array.length, 3);
        assertEquals(array.unshift(), 3);
        assertEquals(array.length, 3);
    },
    
    testCallArrayPrototypeUnshiftWithSingleElementOnEmptyArray: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.unshift('abc'), 1);
        assertEquals(array.length, 1);
        assertEquals(array[0], 'abc');
    },
    
    testCallArrayPrototypeUnshiftWithSingleElement: function() {
        var array = ['a', 'b', 'c'];
        
        assertEquals(array.length, 3);
        assertEquals(array.unshift('abc'), 4);
        assertEquals(array.length, 4);
        assertEquals(array[0], 'abc');
    },
    
    testCallArrayPrototypeUnshiftOnEmptyArray: function() {
        var array = [];
        
        assertEquals(array.length, 0);
        assertEquals(array.unshift(1, 2, 3), 3);
        assertEquals(array.length, 3);
        assertEquals(array[0], 1);
        assertEquals(array[1], 2);
        assertEquals(array[2], 3);
    },
    
    testCallArrayPrototypeUnshift: function() {
        var array = ['a', 'b', 'c'];
        
        assertEquals(array.length, 3);
        assertEquals(array.unshift(1, 2, 3), 6);
        assertEquals(array.length, 6);
        assertEquals(array[0], 1);
        assertEquals(array[1], 2);
        assertEquals(array[2], 3);
    },
    
    testCallStringFromCharCodeWithoutArguments: function() {
        assertEquals(String.fromCharCode(), '');
    },
    
    testCallStringFromCharCode: function() {
        assertEquals(String.fromCharCode(97, 98, 99), 'abc');
    },
    
    testStringPrototypeConstructorPropertyIsStringConstructor: function() {
        assertEquals(String.prototype.constructor, String);
    },
    
    testCallStringPrototypeToString: function() {
        assertEquals(''.toString(), '');
        assertEquals('123'.toString(), '123');
    },
    
    testCallStringPrototypeValueOf: function() {
        assertEquals(''.valueOf(), '');
        assertEquals('123'.valueOf(), '123');
    },
    
    testCallStringPrototypeCharAtWithInvalidPosition: function() {
        assertEquals(''.charAt(0), '');
        assertEquals('123'.charAt(100), '');
    },
    
    testCallStringPrototypeCharAt: function() {
        assertEquals('abc'.charAt(0), 'a');
    },
    
    testCallStringPrototypeCharCodeAtWithInvalidPosition: function() {
        assertNaN(''.charCodeAt(0));
        assertNaN('123'.charCodeAt(100));
    },
    
    testCallStringPrototypeCharCodeAt: function() {
        assertEquals('abc'.charCodeAt(0), 97);
    },
    
    testCallStringPrototypeConcatOnEmptyStringWithNoArguments: function() {
        assertEquals(''.concat(), '');
    },
    
    testCallStringPrototypeConcatOnEmptyString: function() {
        var string = ''.concat(undefined, null, true, 1.23, 'a');
        assertEquals(string, 'undefinednulltrue1.23a');
    },
    
    testCallStringPrototypeConcatWithNoArguments: function() {
        assertEquals('abc'.concat(), 'abc');
    },
    
    testCallStringPrototypeConcat: function() {
        var string = 'abc'.concat(undefined, null, true, 1.23, 'a');
        assertEquals(string, 'abcundefinednulltrue1.23a');
    },
    
    testCallStringPrototypeIndexOfOnEmptyStringWithEmptySearch: function() {
        assertEquals(''.indexOf('', -1), 0);
        assertEquals(''.indexOf(''), 0);
        assertEquals(''.indexOf('', 1), 0);
    },
    
    testCallStringPrototypeIndexOfOnEmptyString: function() {
        assertEquals(''.indexOf('123', -1), -1);
        assertEquals(''.indexOf('123'), -1);
        assertEquals(''.indexOf('123', 1), -1);
    },
    
    testCallStringPrototypeIndexOfWithEmptySearch: function() {
        assertEquals('123'.indexOf('', -1), 0);
        assertEquals('123'.indexOf(''), 0);
        assertEquals('123'.indexOf('', 1), 1);
        assertEquals('123'.indexOf('', 2), 2);
        assertEquals('123'.indexOf('', 3), 3);
        assertEquals('123'.indexOf('', 4), 3);
    },
    
    testCallStringPrototypeIndexOf: function() {
        assertEquals('123123123'.indexOf('123', -1), 0);
        assertEquals('123123123'.indexOf('123'), 0);
        assertEquals('123123123'.indexOf('123', 1), 3);
        assertEquals('123123123'.indexOf('123', 8), -1);
        assertEquals('123123123'.indexOf('123', 9), -1);
        assertEquals('123123123'.indexOf('123', 10), -1);
    },
    
    testCallStringPrototypeLastIndexOfOnEmptyStringWithEmptySearch: function() {
        assertEquals(''.lastIndexOf('', -1), 0);
        assertEquals(''.lastIndexOf(''), 0);
        assertEquals(''.lastIndexOf('', 1), 0);
    },
    
    testCallStringPrototypeLastIndexOfOnEmptyString: function() {
        assertEquals(''.lastIndexOf('123', -1), -1);
        assertEquals(''.lastIndexOf('123'), -1);
        assertEquals(''.lastIndexOf('123', 1), -1);
    },
    
    testCallStringPrototypeLastIndexOfWithEmptySearch: function() {
        assertEquals('123'.lastIndexOf('', -1), 0);
        assertEquals('123'.lastIndexOf(''), 3);
        assertEquals('123'.lastIndexOf('', 1), 1);
        assertEquals('123'.lastIndexOf('', 2), 2);
        assertEquals('123'.lastIndexOf('', 3), 3);
        assertEquals('123'.lastIndexOf('', 4), 3);
    },
    
    testCallStringPrototypeLastIndexOf: function() {
        assertEquals('123123123'.lastIndexOf('123', -1), -1);
        assertEquals('123123123'.lastIndexOf('123'), 6);
        assertEquals('123123123'.lastIndexOf('123', 1), -1);
        assertEquals('123123123'.lastIndexOf('123', 8), 3);
        assertEquals('123123123'.lastIndexOf('123', 9), 6);
        assertEquals('123123123'.lastIndexOf('123', 10), 6);
    },
    
    testCallStringPrototypeLocaleCompare: function() {
        assertEquals('123'.localeCompare('123'), 0);
        assertTrue('123'.localeCompare('') != 0);
    },
    
    testCallStringPrototypeSliceWithNegativeStartAndNoEnd: function() {
        assertEquals(''.slice(-1), '');
        assertEquals('123'.slice(-1), '3');
    },
    
    testCallStringPrototypeSliceWithNegativeStartAndNegativeEnd: function() {
        assertEquals(''.slice(-2, -1), '');
        assertEquals('123'.slice(-2, -1), '2');
    },
    
    testCallStringPrototypeSliceWithNegativeStartAndPositiveEnd: function() {
        assertEquals(''.slice(-1, 1), '');
        assertEquals('123'.slice(-1, 1), '');
        assertEquals('123'.slice(-1, 3), '3');
    },
    
    testCallStringPrototypeSliceWithPositiveStartAndNoEnd: function() {
        assertEquals(''.slice(1), '');
        assertEquals('123'.slice(1), '23');
    },
    
    testCallStringPrototypeSliceWithPositiveStartAndNegativeEnd: function() {
        assertEquals(''.slice(1, -1), '');
        assertEquals('123'.slice(1, -1), '2');
    },
    
    testCallStringPrototypeSliceWithPositiveStartAndPositiveEnd: function() {
        assertEquals(''.slice(1, 1), '');
        assertEquals('123'.slice(1, 1), '');
        assertEquals('123'.slice(1, 3), '23');
    }
});
