package com.comoj.marcio.joi.ui.text;


import com.comoj.marcio.joi.exceptions.WrongNumberOfArgumentsException;


/**
 * Inspects the previous object.
 */
public class BackCommand extends ArgumentsCommand {
    private InspectorConsole _console;
    

    public BackCommand(InspectorConsole console) {
        _console = console;
    }
    

    public String getDescription() {
        return "Inspects the previous object.";
    }
    

    public String getName() {
        return "b";
    }
    

    public String getUsage() {
        return null;
    }
    

    public void run(String[] arguments) {
        if (arguments.length != 0) {
            throw new WrongNumberOfArgumentsException();
        }
        
        _console.inspectPrevious();
    }
}
