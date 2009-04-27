package com.comoj.marcio.joi;


import java.lang.reflect.Method;

import com.comoj.marcio.joi.values.java.lang.ClassValue;


/**
 * A method that can be inspected.
 */
public class InspectableMethod extends InspectableFunction {
    public InspectableMethod(Method method) {
        super(method);
    }
    

    public String describe() {
        return String.format("%s : %s",
            super.describe(),
            ClassValue.getClassNameOf(getValue().getReturnType()));
    }
    

    public String getFunctionName() {
        return getValue().getName();
    }
    

    public Method getValue() {
        return (Method) super.getValue();
    }
    

    protected int getModifiers() {
        return getValue().getModifiers();
    }
    

    protected Class<?>[] getParameterTypes() {
        return getValue().getParameterTypes();
    }
}
