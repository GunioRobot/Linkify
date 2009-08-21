package joi;


/**
 * Something that contains a value that can be written.
 */
public interface Writeable {
    /**
     * Sets the contained value.
     * 
     * @param newValue new value to use
     */
    public void setValue(Object newValue);
    
    
    /**
     * Sets the contained value from input.
     * 
     * @param input new value to use
     */
    public void setValueFromInput(String input);
}
