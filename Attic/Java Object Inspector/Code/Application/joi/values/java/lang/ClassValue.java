package joi.values.java.lang;


import java.lang.annotation.Annotation;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import joi.Inspectable;
import joi.InspectableAnnotation;
import joi.InspectableClass;
import joi.InspectableConstructor;
import joi.InspectableFactory;
import joi.InspectableField;
import joi.InspectableMethod;
import joi.exceptions.NullInspectionException;
import joi.exceptions.PrimitiveInspectionException;
import joi.exceptions.UnsupportedTypeException;



/**
 * A class that can be inspected.
 */
public class ClassValue extends ObjectValue {
    private static final Map<String, Class<?>> _PRIMITIVE_TYPES_NAMES;
    
    
    static {
        _PRIMITIVE_TYPES_NAMES = new HashMap<String, Class<?>>();
        
        for (Class<?> type : InspectableFactory.getPrimitiveTypes()) {
            _PRIMITIVE_TYPES_NAMES.put(type.getSimpleName(), type);
        }
    }


    /**
     * Gets the class name of a class.
     * 
     * @param clazz class for which to get the class name
     * @return the class name of the given class
     * @throws NullPointerException if the given class is null
     */
    public static String getClassNameOf(Class<?> clazz) {
        if (clazz == null) {
            throw new NullPointerException("null is not a class.");
        }
        
        if (clazz.getSimpleName().length() > 0) {
            return clazz.getSimpleName();
        }
        else if (clazz.getCanonicalName() != null) {
            return clazz.getCanonicalName();
        }
        else {
            return clazz.getName();
        }
    }
    
    
    private Object _instance;
    private List<Inspectable> _cachedValues = null;
    

    /**
     * Creates a class that can be inspected.
     * 
     * @param clazz class to be inspected
     */
    public ClassValue(Class<?> clazz) {
        this(clazz, null);
    }
    

    /**
     * Creates a class that can be inspected.
     * 
     * @param clazz class to be inspected
     * @param instance class instance (may be null)
     */
    public ClassValue(Class<?> clazz, Object instance) {
        super(clazz);
        setInstance(instance);
    }
    

    public String describe() {
        Class<?> clazz = getValue();
        
        if (clazz == null) {
            return super.describe();
        }
        
        String modifiers = Modifier.toString(clazz.getModifiers());
        String type = clazz.isInterface() ? "interface" : "class";
        
        return String.format("%s%s%s %s",
            modifiers,
            (modifiers.length() > 0 ? " " : ""),
            type,
            getClassName());
    }
    

    /**
     * Gets the class name.
     * 
     * @return the class name
     */
    public String getClassName() {
        return ClassValue.getClassNameOf(getValue());
    }
    

    /**
     * Gets the instance associated with this class.
     * 
     * @return the class instance (may be null)
     */
    public Object getInstance() {
        return _instance;
    }
    

    public Class<?> getValue() {
        return (Class<?>) super.getValue();
    }
    

    public String getValueToOutput() {
        Class<?> clazz = getValue();
        
        if (clazz == null) {
            return super.getValueToOutput();
        }
        else {
            return clazz.getName() + ".class";
        }
    }
    

    public List<Inspectable> inspect() {
        Class<?> clazz = getValue();
        
        if (clazz == null) {
            throw new NullInspectionException();
        }
        if (clazz.isPrimitive()) {
            throw new PrimitiveInspectionException();
        }
        if (_cachedValues == null) {
            _cachedValues = inspectDeclarations();
        }
        
        return Collections.unmodifiableList(_cachedValues);
    }
    

    /**
     * Sets the instance associated with this class.
     * 
     * @param instance class instance (may be null)
     */
    public void setInstance(Object instance) {
        _instance = instance;
    }
    

    public void setValue(Object newValue) {
        if ((newValue == null) || (newValue instanceof Class<?>)) {
            super.setValue(newValue);
        }
        else {
            throw new UnsupportedTypeException();
        }
    }
    

    public void setValueFromInput(String input) {
        Pattern syntax = Pattern.compile("^([_\\p{Alpha}][\\w.$]*)\\.class$");
        Matcher matcher = syntax.matcher(input);
        
        if (!matcher.find()) {
            super.setValueFromInput(input);
            return;
        }
        
        String className = matcher.group(1);
        Class<?> clazz;
        
        try {
            clazz = Class.forName(className);
        }
        catch (ClassNotFoundException exception) {
            if (_PRIMITIVE_TYPES_NAMES.containsKey(className)) {
                clazz = _PRIMITIVE_TYPES_NAMES.get(className);
            }
            else {
                throw new IllegalArgumentException("Class not found.");
            }
        }
        
        setValue(clazz);
    }
    
    
    /**
     * Inspects all class declarations.
     * 
     * @return a list with class declarations that can be inspected
     */
    private List<Inspectable> inspectDeclarations() {
        List<Inspectable> values = new LinkedList<Inspectable>();
        Class<?> clazz = getValue();
        
        for (; clazz != null; clazz = clazz.getSuperclass()) {
            for (Annotation a : clazz.getDeclaredAnnotations()) {
                if (!a.annotationType().isSynthetic()) {
                    values.add(new InspectableAnnotation(a));
                }
            }
            for (Class<?> c : clazz.getDeclaredClasses()) {
                if (!c.isSynthetic()) {
                    values.add(new InspectableClass(c));
                }
            }
            for (Constructor<?> c : clazz.getDeclaredConstructors()) {
                if (!c.isSynthetic()) {
                    values.add(new InspectableConstructor(c));
                }
            }
            for (Field f : clazz.getDeclaredFields()) {
                if (!f.isSynthetic()) {
                    values.add(new InspectableField(f, getInstance()));
                }
            }
            for (Method m : clazz.getDeclaredMethods()) {
                if (!m.isSynthetic()) {
                    values.add(new InspectableMethod(m));
                }
            }
        }
        
        return values;
    }
}
