package joi.exceptions;


@SuppressWarnings("serial")
public class InvalidSyntaxException extends RuntimeException {
    public InvalidSyntaxException() {
        super("Invalid syntax (or value).");
    }
}
