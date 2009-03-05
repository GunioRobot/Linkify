package com.comoj.marcio.joi.value.java.lang;


import com.comoj.marcio.joi.exceptions.UnsupportedTypeException;


/**
 * An integer that can be inspected.
 */
public class IntegerValue extends NumberValue {
    public IntegerValue(Integer i) {
        super(i);
    }
    

    public Integer getValue() {
        return (Integer) super.getValue();
    }
    

    public void setValue(Object newValue) {
        if ((newValue == null) || (newValue instanceof Integer)) {
            super.setValue(newValue);
        }
        else {
            throw new UnsupportedTypeException();
        }
    }
    

    public void setValueFromInput(String input) {
        try {
            setValue(Integer.parseInt(input));
        }
        catch (NumberFormatException exception) {
            super.setValueFromInput(input);
        }
    }
}
