package com.comoj.marcio.joi;


import java.lang.reflect.Constructor;

import com.comoj.marcio.joi.values.java.lang.ClassValue;


/**
 * A constructor that can be inspected.
 */
public class InspectableConstructor extends InspectableFunction {
    public InspectableConstructor(Constructor<?> constructor) {
        super(constructor);
    }
    

    public String getFunctionName() {
        return ClassValue.getClassNameOf(getValue().getDeclaringClass());
    }
    

    public Constructor<?> getValue() {
        return (Constructor<?>) super.getValue();
    }
    

    protected int getModifiers() {
        return getValue().getModifiers();
    }
    

    protected Class<?>[] getParameterTypes() {
        return getValue().getParameterTypes();
    }
}
