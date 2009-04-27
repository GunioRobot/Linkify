package com.comoj.marcio.joi.ui.text;


import java.util.regex.Matcher;

import com.comoj.marcio.joi.Inspectable;
import com.comoj.marcio.joi.Writeable;
import com.comoj.marcio.joi.exceptions.InvalidIndexException;
import com.comoj.marcio.joi.exceptions.WrongNumberOfArgumentsException;


/**
 * Modifies an object's field value.
 */
public class ModifyCommand extends Command {
    /**
     * Parses the given command line.
     * 
     * @param commandLine command line to parse
     * @return the command line arguments
     */
    private static String[] parseArguments(String commandLine) {
        Matcher matcher = Console.ARGUMENTS_PATTERN.matcher(commandLine);
        if (!matcher.find()) {
            throw new WrongNumberOfArgumentsException();
        }
        
        String index = matcher.group();
        String value = commandLine.substring(index.length()).trim();
        
        if (value.length() == 0) {
            throw new WrongNumberOfArgumentsException();
        }
        
        return new String[] {index, value};
    }
    

    private InspectorConsole _console;
    

    public ModifyCommand(InspectorConsole console) {
        _console = console;
    }
    

    public String getDescription() {
        return "Modifies a field value.";
    }
    

    public String getName() {
        return "m";
    }
    

    public String getUsage() {
        return "<index> <value>";
    }
    

    public void run(String commandLine) {
        String[] arguments = parseArguments(commandLine);
        int index;
        String value = arguments[1];
        
        try {
            index = Integer.parseInt(arguments[0]) - 1;
        }
        catch (NumberFormatException exception) {
            throw new InvalidIndexException();
        }
        
        Inspectable inspectable;
        
        try {
            inspectable = _console.getCurrent().inspect().get(index);
        }
        catch (IndexOutOfBoundsException exception) {
            throw new InvalidIndexException();
        }
        
        if (inspectable instanceof Writeable) {
            ((Writeable) inspectable).setValueFromInput(value);
        }
        else {
            throw new IllegalArgumentException("Not writeable.");
        }
    }
}
