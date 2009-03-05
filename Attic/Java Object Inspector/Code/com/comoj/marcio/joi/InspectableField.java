package com.comoj.marcio.joi;


import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.List;

import com.comoj.marcio.joi.exceptions.InspectionDeniedException;
import com.comoj.marcio.joi.exceptions.InstanceFieldException;
import com.comoj.marcio.joi.exceptions.NullInspectionException;
import com.comoj.marcio.joi.exceptions.NullValueException;
import com.comoj.marcio.joi.exceptions.PrimitiveInspectionException;
import com.comoj.marcio.joi.value.java.lang.ObjectValue;


/**
 * A field that can be inspected.
 */
public class InspectableField implements Inspectable, Writeable {
    private Field _field;
    private Object _owner;
    private ObjectValue _inspectableValue = null;
    

    /**
     * Creates a field that can be inspected.
     * 
     * @param field field to be inspected
     * @param owner field owner (may be null for static fields)
     */
    public InspectableField(Field field, Object owner) {
        _field = field;
        _owner = owner;
        
        _field.setAccessible(true);
        _inspectableValue = Inspector.createInspectable(field.getType(), null);
    }
    

    public String describe() {
        String modifiers = Modifier.toString(getField().getModifiers());
        String value;
        
        try {
            value = getValueToOutput();
        }
        catch (InspectionDeniedException error) {
            value = "?";
        }
        catch (InstanceFieldException error) {
            value = null;
        }
        
        String description = String.format("%s%s%s %s",
            modifiers,
            (modifiers.length() > 0 ? " " : ""),
            Inspector.getClassNameOf(getField().getType()),
            getField().getName());
        
        return description + (value == null ? "" : " = " + value);
    }
    

    public Object getValue() {
        try {
            return getField().get(_owner);
        }
        catch (IllegalAccessException exception) {
            throw new InspectionDeniedException();
        }
        catch (NullPointerException exception) {
            throw new InstanceFieldException();
        }
    }
    

    public String getValueToOutput() {
        _inspectableValue.setValue(getValue());
        return _inspectableValue.getValueToOutput();
    }
    

    public List<Inspectable> inspect() {
        Object value = getValue();
        
        if (value == null) {
            throw new NullInspectionException();
        }
        
        if (getField().getType().isPrimitive()) {
            throw new PrimitiveInspectionException();
        }
        
        _inspectableValue.setValue(value);
        return _inspectableValue.inspect();
    }
    

    public void setValue(Object value) {
        if (getField().getType().isPrimitive() && (value == null)) {
            throw new NullValueException();
        }
        
        try {
            getField().set(_owner, value);
        }
        catch (IllegalAccessException exception) {
            throw new InspectionDeniedException();
        }
        catch (NullPointerException exception) {
            throw new InstanceFieldException();
        }
    }
    

    public void setValueFromInput(String input) {
        _inspectableValue.setValueFromInput(input);
        setValue(_inspectableValue.getValue());
    }
    

    /**
     * Gets the field.
     * 
     * @return the underlying field
     */
    protected Field getField() {
        return _field;
    }
}
