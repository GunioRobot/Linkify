package com.comoj.marcio.joi;


/**
 * Something that holds a value that can be read.
 */
public interface Readable {
    /**
     * Gets the value.
     * 
     * @return the underlying value
     */
    public Object getValue();
    

    /**
     * Gets the value in a format suitable to output.
     * 
     * @return the output value
     */
    public String getValueToOutput();
}
