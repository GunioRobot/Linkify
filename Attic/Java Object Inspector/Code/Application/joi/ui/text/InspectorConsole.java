package joi.ui.text;


import static java.lang.System.err;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import joi.Inspectable;
import joi.values.java.lang.ObjectValue;



/**
 * Java object inspector console.
 */
public class InspectorConsole extends Console {
    /**
     * Inspects a value.
     * 
     * @param inspectable value to be inspected
     */
    private static void inspect(Inspectable inspectable) {
        List<Inspectable> values = inspectable.inspect();
        Object value = inspectable.getValue();
        Class<?> type = value.getClass();
        
        String typeName = (type.getCanonicalName() != null) ?
            type.getCanonicalName() : type.getName();
        
        err.printf("%s@%x instance of %s\n", type.getName(),
            System.identityHashCode(value), typeName);
        
        err.println("----------------------------------------");
        
        if (values.isEmpty()) {
            err.println("(Empty.)");
        }
        else {
            int nrDigits = String.valueOf(values.size()).length();
            String format = "%" + nrDigits + "d: %s\n";
            
            for (int i = 1; i <= values.size(); ++i) {
                err.printf(format, i, values.get(i - 1).describe());
            }
        }
    }
    

    private List<Inspectable> _history = new LinkedList<Inspectable>();
    

    public InspectorConsole() {
        super("> ");
        
        registerCommand(new BackCommand(this));
        registerCommand(new HelpCommand(this));
        registerCommand(new InspectClassCommand(this));
        registerCommand(new InspectCommand(this));
        registerCommand(new InspectSuperClassCommand(this));
        registerCommand(new ModifyCommand(this));
        registerCommand(new QuitCommand(this));
    }
    

    /**
     * Gets the current value being inspected.
     * 
     * @return the current value
     */
    public Inspectable getCurrent() {
        return _history.get(_history.size() - 1);
    }
    

    /**
     * Inspects the current value.
     */
    public void inspectCurrent() {
        inspect(getCurrent());
    }
    

    /**
     * Inspects a new value.
     * 
     * @param inspectable new value to be inspected
     */
    public void inspectNew(Inspectable inspectable) {
        inspect(inspectable);
        _history.add(inspectable);
    }
    

    /**
     * Inspects the previous inspected value.
     * 
     * @throws IllegalStateException if there ins't a previous value
     */
    public void inspectPrevious() {
        if (_history.size() <= 1) {
            throw new IllegalStateException("No previous object.");
        }
        
        _history.remove(_history.size() - 1);
    }
    

    /**
     * Starts this console.
     * 
     * @param object object to be inspected
     */
    public void start(Object object) {
        try {
            inspectNew(new ObjectValue(object));
            super.start();
        }
        catch (IOException exception) {
            // End of input.
        }
        finally {
            _history.clear();
        }
    }
}
