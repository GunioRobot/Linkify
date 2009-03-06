package com.comoj.marcio.joi.values.java.lang;


import com.comoj.marcio.joi.exceptions.UnsupportedTypeException;


/**
 * A float that can be inspected.
 */
public class FloatValue extends NumberValue {
    public FloatValue(Float f) {
        super(f);
    }
    

    public Float getValue() {
        return (Float) super.getValue();
    }
    

    public void setValue(Object newValue) {
        if ((newValue == null) || (newValue instanceof Float)) {
            super.setValue(newValue);
        }
        else {
            throw new UnsupportedTypeException();
        }
    }
    

    public void setValueFromInput(String input) {
        try {
            setValue(Float.parseFloat(input));
        }
        catch (NumberFormatException exception) {
            super.setValueFromInput(input);
        }
    }
}
