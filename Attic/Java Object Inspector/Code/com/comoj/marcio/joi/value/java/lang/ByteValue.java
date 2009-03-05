package com.comoj.marcio.joi.value.java.lang;


import com.comoj.marcio.joi.exceptions.UnsupportedTypeException;


/**
 * A byte that can be inspected.
 */
public class ByteValue extends NumberValue {
    public ByteValue(Byte b) {
        super(b);
    }
    

    public Byte getValue() {
        return (Byte) super.getValue();
    }
    

    public void setValue(Object newValue) {
        if ((newValue == null) || (newValue instanceof Byte)) {
            super.setValue(newValue);
        }
        else {
            throw new UnsupportedTypeException();
        }
    }
    

    public void setValueFromInput(String input) {
        try {
            setValue(Byte.parseByte(input));
        }
        catch (NumberFormatException exception) {
            super.setValueFromInput(input);
        }
    }
}
