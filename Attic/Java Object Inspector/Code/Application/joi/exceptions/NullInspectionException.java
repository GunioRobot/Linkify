package joi.exceptions;


@SuppressWarnings("serial")
public class NullInspectionException extends RuntimeException {
    public NullInspectionException() {
        super("null can't be inspected.");
    }
}
