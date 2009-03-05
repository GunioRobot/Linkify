package com.comoj.marcio.joi;


import java.lang.reflect.Modifier;
import java.util.List;

import com.comoj.marcio.joi.value.java.lang.ClassValue;


/**
 * A function that can be inspected.
 */
public abstract class InspectableFunction implements Inspectable {
    private Object _function;
    private ClassValue _classValue;
    

    public InspectableFunction(Object function) {
        _function = function;
        _classValue = new ClassValue(function.getClass(), function);
    }
    

    public String describe() {
        String mods = Modifier.toString(getModifiers());
        String arguments = "";
        
        if (getParameterTypes().length > 0) {
            StringBuffer buffer = new StringBuffer();
            
            for (Class<?> clazz : getParameterTypes()) {
                buffer.append(Inspector.getClassNameOf(clazz) + ", ");
            }
            
            buffer.setLength(buffer.length() - 2);
            arguments = buffer.toString();
        }
        
        return String.format("%s%s%s(%s)",
            mods,
            (mods.length() > 0 ? " " : ""),
            getFunctionName(),
            arguments);
    }
    

    /**
     * Gets the name of this function.
     * 
     * @return the name of this function
     */
    public abstract String getFunctionName();
    

    public Object getValue() {
        return _function;
    }
    

    public String getValueToOutput() {
        return describe();
    }
    

    public List<Inspectable> inspect() {
        return _classValue.inspect();
    }
    

    /**
     * Gets the parameter types for this function.
     * 
     * @return the function parameter types
     */
    protected abstract Class<?>[] getParameterTypes();
    

    /**
     * Gets the modifiers for this function.
     * 
     * @return the language modifiers
     */
    protected abstract int getModifiers();
}
