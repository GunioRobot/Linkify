package com.comoj.marcio.joi.ui.text;


import com.comoj.marcio.joi.exceptions.WrongNumberOfArgumentsException;


/**
 * Exits a console.
 */
public class QuitCommand extends ArgumentsCommand {
    private Console _console;
    

    public QuitCommand(Console console) {
        _console = console;
    }
    

    public String getDescription() {
        return "Exits the console.";
    }
    

    public String getName() {
        return "q";
    }
    

    public String getUsage() {
        return null;
    }
    

    public void run(String[] arguments) {
        if (arguments.length != 0) {
            throw new WrongNumberOfArgumentsException();
        }
        
        _console.stop();
    }
}
