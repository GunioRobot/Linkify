package joi.values.java.lang;


import joi.exceptions.UnsupportedTypeException;


/**
 * A number that can be inspected.
 */
public class NumberValue extends ObjectValue {
    public NumberValue(Number number) {
        super(number);
    }
    

    public Number getValue() {
        return (Number) super.getValue();
    }
    

    public String getValueToOutput() {
        Number number = getValue();
        
        if (number == null) {
            return super.getValueToOutput();
        }
        else {
            return number.toString();
        }
    }
    

    public void setValue(Object newValue) {
        if ((newValue == null) || (newValue instanceof Number)) {
            super.setValue(newValue);
        }
        else {
            throw new UnsupportedTypeException();
        }
    }
}
