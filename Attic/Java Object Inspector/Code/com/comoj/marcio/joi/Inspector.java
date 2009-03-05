package com.comoj.marcio.joi;


import java.lang.annotation.Annotation;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import com.comoj.marcio.joi.exceptions.PrimitiveInspectionException;
import com.comoj.marcio.joi.ui.text.InspectorConsole;
import com.comoj.marcio.joi.value.ArrayValue;
import com.comoj.marcio.joi.value.java.lang.ObjectValue;


/**
 * Java object inspector.
 */
public class Inspector {
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
     * @param value value to be used (may be null)
     * @return a suitable value that can be inspected
     */
    public static ObjectValue createInspectable(Class<?> clazz, Object value) {
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
            Class<?>[] params = new Class<?>[] {clazz};
            Constructor<?> ctor = inspectable.getConstructor(params);
            Object instance = ctor.newInstance(new Object[] {value});
            
            return (ObjectValue) instance;
        }
        catch (ClassNotFoundException exception) {
            // TODO: Should we recurse here to find a suitable super class?
        }
        catch (NoSuchMethodException exception) {
            // Do nothing.
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
    

    /**
     * Gets the set of primitive types.
     * 
     * @return a read-only set of all primitive types
     */
    public static Set<Class<?>> getPrimitiveTypes() {
        return Collections.unmodifiableSet(_WRAPPER_TYPES.keySet());
    }
    

    /**
     * Inspects a class. Synthetic slots will not be listed.
     * 
     * @param clazz class to be inspected
     * @param instance class instance (may be null)
     * @return a list of values that can be inspected
     */
    public static List<Inspectable> inspect(Class<?> clazz, Object instance) {
        if (clazz.isPrimitive()) {
            throw new PrimitiveInspectionException();
        }
        
        List<Inspectable> values = new LinkedList<Inspectable>();
        
        values.addAll(getFieldsOf(clazz, instance));
        values.addAll(getClassesOf(clazz));
        values.addAll(getAnnotationsOf(clazz));
        values.addAll(getConstructorsOf(clazz));
        values.addAll(getMethodsOf(clazz));
        
        return values;
    }
    

    public static void main(String[] arguments) {
        @SuppressWarnings("unused")
        class Test {
            class A {
                String s = null;

                class C {
                }
            }

            private String string = "Hello \"world\"!\nTesting...";
            public int i = 123;
            public Number n = 1;
            protected double d = 0.123;
            public float f = 0.1f;
            public Float F = 1.1f;
            public char c = '\'';
            public Character C = '\n';
            public boolean bool = true;
            int[][] array = { {1}, {2}, {3}};
            String[] args = {"\n", "\"'\"", "ABC"};
            String[][][][] arrayS = {{{ {"a", "b", "c"}, {"d"}}}};
            short sh = 1;
            char[] arrayChars = {'\'', 'a'};
            Character[] arrayCharacters = {};
            Object instance = new Inspector();
            byte b = 0;
            Class<?> self = Test.class;
            Class<?> anonymous = (new Object() {}).getClass();
            Class<?> annotation = Deprecated.class;
        }
        
        new Inspector().inspect(new Test());
    }
    

    private static List<Inspectable> getAnnotationsOf(Class<?> clazz) {
        List<Inspectable> values = new LinkedList<Inspectable>();
        
        for (; clazz != null; clazz = clazz.getSuperclass()) {
            for (Annotation a : clazz.getDeclaredAnnotations()) {
                if (!a.annotationType().isSynthetic()) {
                    values.add(new InspectableAnnotation(a));
                }
            }
        }
        
        return values;
    }
    

    private static List<Inspectable> getClassesOf(Class<?> clazz) {
        List<Inspectable> values = new LinkedList<Inspectable>();
        
        for (; clazz != null; clazz = clazz.getSuperclass()) {
            for (Class<?> c : clazz.getDeclaredClasses()) {
                if (!c.isSynthetic()) {
                    values.add(new InspectableClass(c));
                }
            }
        }
        
        return values;
    }
    

    private static List<Inspectable> getConstructorsOf(Class<?> clazz) {
        List<Inspectable> values = new LinkedList<Inspectable>();
        
        for (; clazz != null; clazz = clazz.getSuperclass()) {
            for (Constructor<?> c : clazz.getDeclaredConstructors()) {
                if (!c.isSynthetic()) {
                    values.add(new InspectableConstructor(c));
                }
            }
        }
        
        return values;
    }
    

    private static List<Inspectable> getFieldsOf(Class<?> clazz, Object instance)
    {
        List<Inspectable> values = new LinkedList<Inspectable>();
        
        for (; clazz != null; clazz = clazz.getSuperclass()) {
            for (Field f : clazz.getDeclaredFields()) {
                if (!f.isSynthetic()) {
                    values.add(new InspectableField(f, instance));
                }
            }
        }
        
        return values;
    }
    

    private static List<Inspectable> getMethodsOf(Class<?> clazz) {
        List<Inspectable> values = new LinkedList<Inspectable>();
        
        for (; clazz != null; clazz = clazz.getSuperclass()) {
            for (Method m : clazz.getDeclaredMethods()) {
                if (!m.isSynthetic()) {
                    values.add(new InspectableMethod(m));
                }
            }
        }
        
        return values;
    }
    

    /**
     * Inspects an object.
     * 
     * @param object object to be inspected
     * @throws NullPointerException if the object is null
     */
    public void inspect(Object object) {
        if (object == null) {
            throw new NullPointerException("Can't inspect null.");
        }
        
        new InspectorConsole().start(object);
    }
}
