package com.comoj.marcio.joi.values.java.lang;


import java.util.List;

import com.comoj.marcio.joi.Inspectable;
import com.comoj.marcio.joi.Writeable;
import com.comoj.marcio.joi.exceptions.InvalidSyntaxException;
import com.comoj.marcio.joi.exceptions.NullInspectionException;


/**
 * A generic value that can be inspected.
 */
public class ObjectValue implements Inspectable, Writeable {
    private Object _object;
    private ClassValue _classValue;
    

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
        if (getValue() == null) {
            throw new NullInspectionException();
        }
        
        return _classValue.inspect();
    }
    

    public void setValue(Object newValue) {
        _object = newValue;
        _classValue = new ClassValue(newValue.getClass(), newValue);
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
