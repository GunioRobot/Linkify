package com.comoj.marcio.joi.ui.text;


import static java.lang.System.err;

import com.comoj.marcio.joi.exceptions.WrongNumberOfArgumentsException;


/**
 * Lists all registered commands in a console.
 */
public class HelpCommand extends ArgumentsCommand {
    private Console _console;
    

    public HelpCommand(Console console) {
        _console = console;
    }
    

    public String getDescription() {
        return "Lists available commands.";
    }
    

    public String getName() {
        return "h";
    }
    

    public String getUsage() {
        return null;
    }
    

    public void run(String[] arguments) {
        if (arguments.length != 0) {
            throw new WrongNumberOfArgumentsException();
        }
        
        err.println("Available commands:");
        
        for (Command command : _console.getRegisteredCommands()) {
            err.print("- " + command.getName());
            
            if (command.getUsage() != null) {
                err.print(" " + command.getUsage());
            }
            
            err.printf(": %s\n", command.getDescription());
        }
    }
}
