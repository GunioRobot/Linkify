package joi.exceptions;


@SuppressWarnings("serial")
public class InvalidIndexException extends RuntimeException {
    public InvalidIndexException() {
        super("Invalid index.");
    }
}
