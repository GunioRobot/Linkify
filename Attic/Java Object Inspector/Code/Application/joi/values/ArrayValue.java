package joi.values;


import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

import joi.Inspectable;
import joi.InspectableArrayLength;
import joi.InspectableArrayPosition;
import joi.exceptions.InvalidSyntaxException;
import joi.exceptions.NullInspectionException;
import joi.exceptions.UnsupportedTypeException;
import joi.values.java.lang.ClassValue;
import joi.values.java.lang.ObjectValue;
import joi.values.java.lang.StringValue;



/**
 * An array that can be inspected.
 */
public class ArrayValue extends ObjectValue {
    private List<Inspectable> _cachedValues = null;
    private Class<?> _componentType;
    

    public ArrayValue(Class<?> componentType, Object array) {
        super(array);
        _componentType = componentType;
    }
    

    /**
     * Gets the component type of the array.
     * 
     * @return the class representing the component type
     */
    public Class<?> getComponentType() {
        return _componentType;
    }
    

    /**
     * Gets the length of the array.
     * 
     * @return the array length
     */
    public int getLength() {
        return Array.getLength(getValue());
    }
    

    public String getValueToOutput() {
        Object array = getValue();
        
        if (array == null) {
            return super.getValueToOutput();
        }
        
        if (isCharArray()) {
            StringBuffer buffer = new StringBuffer(getLength());
            
            for (int i = 0; i < getLength(); ++i) {
                buffer.append(Array.get(array, i));
            }
            
            return StringValue.escape(buffer.toString());
        }
        
        if (getLength() == 0) {
            return "{}";
        }
        else {
            StringBuffer buffer = new StringBuffer();
            
            for (InspectableArrayPosition position : inspectPositions()) {
                buffer.append(", " + position.getValueToOutput());
            }
            
            return "{" + buffer.substring(2) + "}";
        }
    }
    

    public List<Inspectable> inspect() {
        Object array = getValue();
        
        if (array == null) {
            throw new NullInspectionException();
        }
        
        if (_cachedValues == null) {
            List<Inspectable> values = new LinkedList<Inspectable>();
            
            values.add(new InspectableArrayLength(this));
            values.addAll(inspectPositions());
            values.addAll(new ClassValue(array.getClass(), array).inspect());
            
            _cachedValues = values;
        }
        
        return Collections.unmodifiableList(_cachedValues);
    }
    

    public void setValue(Object newValue) {
        if ((newValue == null) || (newValue instanceof Object)) {
            // Invalidate the cache because the array length may change.
            _cachedValues = null;
            
            super.setValue(newValue);
        }
        else {
            throw new UnsupportedTypeException();
        }
    }
    

    public void setValueFromInput(String input) {
        if (isCharArray()) {
            try {
                char[] chars = StringValue.unescape(input).toCharArray();
                Class<?> type = getComponentType();
                Object array = Array.newInstance(type, chars.length);
                
                for (int i = 0; i < chars.length; ++i) {
                    Array.set(array, i, chars[i]);
                }
                
                setValue(array);
                return;
            }
            catch (InvalidSyntaxException error) {
                // Continue.
            }
        }
        
        super.setValueFromInput(input);
    }
    

    /**
     * Inspects all array positions.
     * 
     * @return a list with array positions that can be inspected
     */
    private List<InspectableArrayPosition> inspectPositions() {
        List<InspectableArrayPosition> values;
        values = new ArrayList<InspectableArrayPosition>(getLength());
        
        for (int i = 0; i < getLength(); ++i) {
            values.add(new InspectableArrayPosition(this, i));
        }
        
        return values;
    }
    

    /**
     * Checks whether or not the array is a character array.
     * 
     * @return true if the array is a character array
     */
    private boolean isCharArray() {
        Class<?> type = getComponentType();
        return (type == char.class) || (type == Character.class);
    }
}
