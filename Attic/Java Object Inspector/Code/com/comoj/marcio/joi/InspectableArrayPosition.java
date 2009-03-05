package com.comoj.marcio.joi;


import java.lang.reflect.Array;
import java.util.List;

import com.comoj.marcio.joi.exceptions.NullInspectionException;
import com.comoj.marcio.joi.exceptions.NullValueException;
import com.comoj.marcio.joi.exceptions.PrimitiveInspectionException;
import com.comoj.marcio.joi.value.ArrayValue;
import com.comoj.marcio.joi.value.java.lang.ObjectValue;


/**
 * An array position that can be inspected.
 */
public class InspectableArrayPosition implements Inspectable, Writeable {
    private ArrayValue _arrayValue;
    private int _position;
    private ObjectValue _inspectableValue = null;
    

    public InspectableArrayPosition(ArrayValue arrayValue, int position) {
        _arrayValue = arrayValue;
        _position = position;
        
        Class<?> clazz = _arrayValue.getComponentType();
        _inspectableValue = Inspector.createInspectable(clazz, null);
    }
    

    public String describe() {
        return String.format("public %s %d = %s",
            Inspector.getClassNameOf(_arrayValue.getComponentType()),
            _position, getValueToOutput());
    }
    

    public List<Inspectable> inspect() {
        Object value = getValue();
        
        if (value == null) {
            throw new NullInspectionException();
        }
        
        if (_arrayValue.getComponentType().isPrimitive()) {
            throw new PrimitiveInspectionException();
        }
        
        _inspectableValue.setValue(value);
        return _inspectableValue.inspect();
    }
    

    public Object getValue() {
        return Array.get(_arrayValue.getValue(), _position);
    }
    

    public String getValueToOutput() {
        _inspectableValue.setValue(getValue());
        return _inspectableValue.getValueToOutput();
    }
    

    public void setValue(Object newValue) {
        Class<?> clazz = _arrayValue.getComponentType();
        
        if (clazz.isPrimitive() && (newValue == null)) {
            throw new NullValueException();
        }
        
        Array.set(_arrayValue.getValue(), _position, newValue);
    }
    

    public void setValueFromInput(String input) {
        _inspectableValue.setValueFromInput(input);
        setValue(_inspectableValue.getValue());
    }
}
