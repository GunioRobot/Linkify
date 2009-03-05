package com.comoj.marcio.joi;


import java.lang.reflect.Constructor;


/**
 * A constructor that can be inspected.
 */
public class InspectableConstructor extends InspectableFunction {
    public InspectableConstructor(Constructor<?> constructor) {
        super(constructor);
    }
    

    public String getFunctionName() {
        return Inspector.getClassNameOf(getValue().getDeclaringClass());
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
