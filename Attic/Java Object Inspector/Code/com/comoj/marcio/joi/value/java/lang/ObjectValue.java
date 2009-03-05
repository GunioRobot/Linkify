package com.comoj.marcio.joi.value.java.lang;


import java.util.Collections;
import java.util.List;

import com.comoj.marcio.joi.Inspectable;
import com.comoj.marcio.joi.Inspector;
import com.comoj.marcio.joi.Writeable;
import com.comoj.marcio.joi.exceptions.InvalidSyntaxException;
import com.comoj.marcio.joi.exceptions.NullInspectionException;
import com.comoj.marcio.joi.exceptions.PrimitiveInspectionException;


/**
 * A generic value (Object) that can be inspected.
 */
public class ObjectValue implements Inspectable, Writeable {
    private Object _object;
    private List<Inspectable> _cachedValues = null;
    

    public ObjectValue(Object object) {
        setValue(object);
    }
    

    public String describe() {
        return getValueToOutput();
    }
    

    public Object getValue() {
        return _object;
    }
    

    public String getValueToOutput() {
        Object object = getValue();
        
        if (object == null) {
            return "null";
        }
        
        return String.format("<[%s@%x]>",
            object.getClass().getName(),
            System.identityHashCode(object));
    }
    

    public List<Inspectable> inspect() {
        Object object = getValue();
        
        if (object == null) {
            throw new NullInspectionException();
        }
        
        Class<?> clazz = object.getClass();
        
        if (clazz.isPrimitive()) {
            throw new PrimitiveInspectionException();
        }
        
        if (_cachedValues == null) {
            _cachedValues = Inspector.inspect(clazz, object);
        }
        
        return Collections.unmodifiableList(_cachedValues);
    }
    

    public void setValue(Object newValue) {
        _object = newValue;
    }
    

    public void setValueFromInput(String input) {
        if (input.equals("null")) {
            setValue(null);
        }
        else {
            throw new InvalidSyntaxException();
        }
    }
}
