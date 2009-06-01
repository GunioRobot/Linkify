package joi.ui.text;


import joi.exceptions.WrongNumberOfArgumentsException;
import joi.values.java.lang.ClassValue;


/**
 * Inspects the class of an object.
 */
public class InspectClassCommand extends ArgumentsCommand {
    private InspectorConsole _console;
    

    public InspectClassCommand(InspectorConsole console) {
        _console = console;
    }
    

    public String getDescription() {
        return "Inspects the class.";
    }
    

    public String getName() {
        return "c";
    }
    

    public String getUsage() {
        return null;
    }
    

    public void run(String[] arguments) {
        if (arguments.length != 0) {
            throw new WrongNumberOfArgumentsException();
        }
        
        Class<?> clazz = _console.getCurrent().getValue().getClass();
        _console.inspectNew(new ClassValue(clazz));
    }
}
