package joi.exceptions;


@SuppressWarnings("serial")
public class PrimitiveInspectionException extends RuntimeException {
    public PrimitiveInspectionException() {
        super("Primitive types can't be inspected.");
    }
}
