package com.comoj.marcio.joi.ui.text;


import com.comoj.marcio.joi.Inspectable;
import com.comoj.marcio.joi.exceptions.InvalidIndexException;
import com.comoj.marcio.joi.exceptions.WrongNumberOfArgumentsException;


/**
 * Inspects an object.
 */
public class InspectCommand extends ArgumentsCommand {
    private InspectorConsole _console;
    

    public InspectCommand(InspectorConsole console) {
        _console = console;
    }
    

    public String getDescription() {
        return "Inspects an object (or the current one).";
    }
    

    public String getName() {
        return "i";
    }
    

    public String getUsage() {
        return "[index]";
    }
    

    public void run(String[] arguments) {
        if (arguments.length == 0) {
            _console.inspectCurrent();
            return;
        }
        else if (arguments.length != 1) {
            throw new WrongNumberOfArgumentsException();
        }
        
        int index;
        
        try {
            index = Integer.parseInt(arguments[0]) - 1;
        }
        catch (NumberFormatException exception) {
            throw new InvalidIndexException();
        }
        
        Inspectable next;
        
        try {
            next = _console.getCurrent().inspect().get(index);
        }
        catch (IndexOutOfBoundsException exception) {
            throw new InvalidIndexException();
        }
        
        _console.inspectNew(next);
    }
}
