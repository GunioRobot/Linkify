package com.comoj.marcio.joi;


import java.util.List;

import com.comoj.marcio.joi.exceptions.PrimitiveInspectionException;
import com.comoj.marcio.joi.values.ArrayValue;


/**
 * An array length that can be inspected.
 */
public class InspectableArrayLength implements Inspectable {
    private ArrayValue _arrayValue;
    

    public InspectableArrayLength(ArrayValue arrayField) {
        _arrayValue = arrayField;
    }
    

    public String describe() {
        return "public final int length = " + getValueToOutput();
    }
    

    public List<Inspectable> inspect() {
        throw new PrimitiveInspectionException();
    }
    

    public Integer getValue() {
        return _arrayValue.getLength();
    }
    

    public String getValueToOutput() {
        return getValue().toString();
    }
}
