package joi.inspectables;


import java.util.List;

import joi.Inspectable;
import joi.values.java.lang.ClassValue;


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
