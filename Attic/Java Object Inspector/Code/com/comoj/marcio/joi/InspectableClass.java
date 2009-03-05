package com.comoj.marcio.joi;


import java.util.List;

import com.comoj.marcio.joi.value.java.lang.ClassValue;


/**
 * A class that can be inspected.
 */
public class InspectableClass implements Inspectable {
    private ClassValue _classValue;
    

    public InspectableClass(Class<?> clazz) {
        _classValue = new ClassValue(clazz);
    }
    

    public String describe() {
        return _classValue.describe();
    }
    

    /**
     * Gets the class name.
     * 
     * @return the class name
     */
    public String getClassName() {
        return _classValue.getClassName();
    }
    

    public Class<?> getValue() {
        return _classValue.getValue();
    }
    

    public String getValueToOutput() {
        return _classValue.getValueToOutput();
    }
    

    public List<Inspectable> inspect() {
        return _classValue.inspect();
    }
}
