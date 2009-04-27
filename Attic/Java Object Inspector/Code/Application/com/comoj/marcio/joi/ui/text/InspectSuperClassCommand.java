package com.comoj.marcio.joi.ui.text;


import com.comoj.marcio.joi.exceptions.WrongNumberOfArgumentsException;
import com.comoj.marcio.joi.values.java.lang.ClassValue;


/**
 * Inspects the super class of an object.
 */
public class InspectSuperClassCommand extends ArgumentsCommand {
    private InspectorConsole _console;
    

    public InspectSuperClassCommand(InspectorConsole console) {
        _console = console;
    }
    

    public String getDescription() {
        return "Inspects the super class.";
    }
    

    public String getName() {
        return "s";
    }
    

    public String getUsage() {
        return null;
    }
    

    public void run(String[] arguments) {
        if (arguments.length != 0) {
            throw new WrongNumberOfArgumentsException();
        }
        
        Object value = _console.getCurrent().getValue();
        Class<?> superClass;
        
        if (value instanceof Class) {
            superClass = ((Class<?>) value).getSuperclass();
        }
        else {
            superClass = value.getClass().getSuperclass();
        }
        
        if (superClass == null) {
            throw new IllegalStateException("Root of the class hierarchy.");
        }
        
        _console.inspectNew(new ClassValue(superClass));
    }
}
