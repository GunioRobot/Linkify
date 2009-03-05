package com.comoj.marcio.joi.value.java.lang;


import com.comoj.marcio.joi.exceptions.UnsupportedTypeException;


/**
 * A boolean that can be inspected.
 */
public class BooleanValue extends ObjectValue {
    public BooleanValue(Boolean bool) {
        super(bool);
    }
    

    public Boolean getValue() {
        return (Boolean) super.getValue();
    }
    

    public String getValueToOutput() {
        Boolean bool = getValue();
        
        if (bool == true) {
            return "true";
        }
        else if (bool == false) {
            return "false";
        }
        else {
            return super.getValueToOutput();
        }
    }
    

    public void setValue(Object newValue) {
        if ((newValue == null) || (newValue instanceof Boolean)) {
            super.setValue(newValue);
        }
        else {
            throw new UnsupportedTypeException();
        }
    }
    

    public void setValueFromInput(String input) {
        if (input.equals("true")) {
            setValue(true);
        }
        else if (input.equals("false")) {
            setValue(false);
        }
        else {
            super.setValueFromInput(input);
        }
    }
}
