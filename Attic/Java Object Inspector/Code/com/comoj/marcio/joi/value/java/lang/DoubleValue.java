package com.comoj.marcio.joi.value.java.lang;


import com.comoj.marcio.joi.exceptions.UnsupportedTypeException;


/**
 * A double that can be inspected.
 */
public class DoubleValue extends NumberValue {
    public DoubleValue(Double d) {
        super(d);
    }
    

    public Double getValue() {
        return (Double) super.getValue();
    }
    

    public void setValue(Object newValue) {
        if ((newValue == null) || (newValue instanceof Double)) {
            super.setValue(newValue);
        }
        else {
            throw new UnsupportedTypeException();
        }
    }
    

    public void setValueFromInput(String input) {
        try {
            setValue(Double.parseDouble(input));
        }
        catch (NumberFormatException exception) {
            super.setValueFromInput(input);
        }
    }
}
