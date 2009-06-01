package joi.values.java.lang;


import joi.exceptions.UnsupportedTypeException;


/**
 * A short that can be inspected.
 */
public class ShortValue extends NumberValue {
    public ShortValue(Short s) {
        super(s);
    }
    

    public Short getValue() {
        return (Short) super.getValue();
    }
    

    public void setValue(Object newValue) {
        if ((newValue == null) || (newValue instanceof Short)) {
            super.setValue(newValue);
        }
        else {
            throw new UnsupportedTypeException();
        }
    }
    

    public void setValueFromInput(String input) {
        try {
            setValue(Short.parseShort(input));
        }
        catch (NumberFormatException exception) {
            super.setValueFromInput(input);
        }
    }
}
