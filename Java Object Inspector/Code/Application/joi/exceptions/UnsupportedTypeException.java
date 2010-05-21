package joi.exceptions;


@SuppressWarnings("serial")
public class UnsupportedTypeException extends RuntimeException {
    public UnsupportedTypeException() {
        super("Unsupported type.");
    }
}
