package com.comoj.marcio.joi.value.java.lang;


import com.comoj.marcio.joi.exceptions.UnsupportedTypeException;


/**
 * A long that can be inspected.
 */
public class LongValue extends NumberValue {
    public LongValue(Long l) {
        super(l);
    }
    

    public Long getValue() {
        return (Long) super.getValue();
    }
    

    public void setValue(Object newValue) {
        if ((newValue == null) || (newValue instanceof Long)) {
            super.setValue(newValue);
        }
        else {
            throw new UnsupportedTypeException();
        }
    }
    

    public void setValueFromInput(String input) {
        try {
            setValue(Long.parseLong(input));
        }
        catch (NumberFormatException exception) {
            super.setValueFromInput(input);
        }
    }
}
