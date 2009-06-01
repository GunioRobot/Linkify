package joi.values.java.lang;


import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import joi.Inspectable;
import joi.Writeable;
import joi.exceptions.InvalidSyntaxException;
import joi.exceptions.NullInspectionException;
import joi.values.ArrayValue;


/**
 * A generic value that can be inspected.
 */
public class ObjectValue implements Inspectable, Writeable {
    private static final Map<Class<?>, Class<?>> _WRAPPER_TYPES =
        new HashMap<Class<?>, Class<?>>();
    

    static {
        _WRAPPER_TYPES.put(boolean.class, Boolean.class);
        _WRAPPER_TYPES.put(byte.class, Byte.class);
        _WRAPPER_TYPES.put(char.class, Character.class);
        _WRAPPER_TYPES.put(double.class, Double.class);
        _WRAPPER_TYPES.put(float.class, Float.class);
        _WRAPPER_TYPES.put(int.class, Integer.class);
        _WRAPPER_TYPES.put(long.class, Long.class);
        _WRAPPER_TYPES.put(short.class, Short.class);
        _WRAPPER_TYPES.put(void.class, Void.class);
    }
    

    /**
     * Creates a value that can be inspected.
     * 
     * @param clazz class of the value
     * @return a suitable value that can be inspected
     */
    public static ObjectValue detectFromClass(Class<?> clazz) {
        return detectFromClass(clazz, null);
    }
    
    
    /**
     * Creates a value that can be inspected.
     * 
     * @param clazz class of the value
     * @param value value to be used (may be null)
     * @return a suitable value that can be inspected
     */
    public static ObjectValue detectFromClass(Class<?> clazz, Object value) {
        if (clazz.isArray()) {
            return new ArrayValue(clazz.getComponentType(), value);
        }
        if (clazz.isPrimitive()) {
            clazz = _WRAPPER_TYPES.get(clazz);
        }
        
        String packageName = ArrayValue.class.getPackage().getName();
        String className = packageName + "." + clazz.getName() + "Value";
        
        try {
            Class<?> inspectable = Class.forName(className);
            Class<?>[] parameters = new Class<?>[] {clazz};
            Constructor<?> constructor = inspectable.getConstructor(parameters);
            
            return (ObjectValue) constructor.newInstance(new Object[] {value});
        }
        catch (ClassNotFoundException exception) {
            // Do nothing.
        }
        catch (NoSuchMethodException exception) {
            // Ditto.
        }
        catch (InstantiationException exception) {
            // Ditto.
        }
        catch (IllegalAccessException exception) {
            // Ditto.
        }
        catch (InvocationTargetException exception) {
            // Ditto.
        }
        
        // If it failed, create a generic one.
        return new ObjectValue(value);
    }
    
    
    /**
     * Gets the set of primitive types.
     * 
     * @return a read-only set of all primitive types
     */
    public static Set<Class<?>> getPrimitiveTypes() {
        return Collections.unmodifiableSet(_WRAPPER_TYPES.keySet());
    }
    
    
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
