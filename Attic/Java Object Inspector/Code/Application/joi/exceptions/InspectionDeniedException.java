package joi.exceptions;


@SuppressWarnings("serial")
public class InspectionDeniedException extends RuntimeException {
    public InspectionDeniedException() {
        super("Access denied for inspection.");
    }
}
