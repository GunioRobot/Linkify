package joi;


/**
 * Something that contains a value that can be read.
 */
public interface Readable {
    /**
     * Gets the contained value.
     * 
     * @return the contained value
     */
    public Object getValue();
    
    
    /**
     * Gets the contained value in a format suitable for output.
     * 
     * @return the contained value for output
     */
    public String getValueToOutput();
}
