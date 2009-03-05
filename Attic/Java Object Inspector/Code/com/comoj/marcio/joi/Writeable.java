package com.comoj.marcio.joi;


/**
 * Something that holds a value that can be written.
 */
public interface Writeable {
    /**
     * Sets the value.
     * 
     * @param newValue new value
     */
    public void setValue(Object newValue);
    

    /**
     * Sets the value from input.
     * 
     * @param input input value
     */
    public void setValueFromInput(String input);
}
