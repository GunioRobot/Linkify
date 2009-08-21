package joi;


import java.util.List;


/**
 * A value that can be inspected.
 */
public interface Inspectable extends Readable {
    /**
     * Describes this value.
     * 
     * @return a meaningful description of this value
     */
    public String describe();
    
    
    /**
     * Inspects this value.
     * 
     * @return a read-only list of values contained in this value
     */
    public List<Inspectable> inspect();
}
