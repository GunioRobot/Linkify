package joi.exceptions;


@SuppressWarnings("serial")
public class InstanceFieldException extends RuntimeException {
    public InstanceFieldException() {
        super("An instance field requires an instance of its class.");
    }
}
