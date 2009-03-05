package com.comoj.marcio.joi.ui.text;


import com.comoj.marcio.joi.exceptions.WrongNumberOfArgumentsException;
import com.comoj.marcio.joi.value.java.lang.ClassValue;


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
