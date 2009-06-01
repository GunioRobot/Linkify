package joi;


import java.lang.reflect.Array;
import java.util.List;

import joi.exceptions.NullInspectionException;
import joi.exceptions.NullValueException;
import joi.exceptions.PrimitiveInspectionException;
import joi.values.ArrayValue;
import joi.values.java.lang.ClassValue;
import joi.values.java.lang.ObjectValue;



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
        _inspectableValue = InspectableFactory.createInspectable(clazz, null);
    }
    

    public String describe() {
        return String.format("public %s %d = %s",
            ClassValue.getClassNameOf(_arrayValue.getComponentType()),
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
