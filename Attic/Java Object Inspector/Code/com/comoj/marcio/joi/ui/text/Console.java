package com.comoj.marcio.joi.ui.text;


import static java.lang.System.err;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;


/**
 * Command interpreter console.
 */
public class Console {
    public static final Pattern ARGUMENTS_PATTERN = Pattern.compile("([^\\s]+)");
    
    private String _prompt;
    private BufferedReader _input;
    private Map<String, Command> _commands = new HashMap<String, Command>();
    private Thread _consoleThread = Thread.currentThread();
    private boolean _isRunning = false;
    

    /**
     * Creates a new console.
     * 
     * @param prompt command prompt to be used
     */
    public Console(String prompt) {
        _prompt = prompt;
        _input = new BufferedReader(new InputStreamReader(System.in));
    }
    

    /**
     * Gets all registered commands.
     * 
     * @return a list with all registered commands
     */
    public List<Command> getRegisteredCommands() {
        List<Command> commands = new ArrayList<Command>(_commands.values());
        Collections.sort(commands);
        return commands;
    }
    

    /**
     * Registers a command.
     * 
     * @param command command to be registered
     */
    public void registerCommand(Command command) {
        _commands.put(command.getName(), command);
    }
    

    /**
     * Removes a registered command.
     * 
     * @param command registered command to be removed
     */
    public void removeCommand(Command command) {
        _commands.remove(command.getName());
    }
    

    /**
     * Starts this console.
     * 
     * @throws IOException if an I/O errors occurs
     */
    public void start() throws IOException {
        if (_isRunning) {
            throw new IllegalStateException("Console already started.");
        }
        
        try {
            _isRunning = true;
            
            while (true) {
                String input = readInputLine();
                
                if (input == null) {
                    break;
                }
                
                Matcher matcher = Console.ARGUMENTS_PATTERN.matcher(input);
                matcher.find();
                
                String commandName = matcher.group();
                String commandLine = input.substring(commandName.length());
                Command command = _commands.get(commandName);
                
                if (command == null) {
                    err.println("Error: Unknown command.");
                    continue;
                }
                
                try {
                    runCommand(command, commandLine.trim());
                }
                catch (InterruptedException exception) {
                    break;
                }
            }
        }
        finally {
            _isRunning = false;
        }
    }
    

    /**
     * Stops this console.
     */
    public void stop() {
        while (_isRunning) {
            _consoleThread.interrupt();
        }
    }
    

    /**
     * Reads a command line from the input stream.
     * 
     * @return the input line read
     * @throws IOException if an I/O error occurs
     */
    private String readInputLine() throws IOException {
        String input = "";
        
        while (input.length() == 0) {
            err.print(_prompt);
            input = _input.readLine();
            
            if (input == null) {
                return null;
            }
            
            input = input.trim();
        }
        
        return input;
    }
    

    /**
     * Runs the given command.
     * 
     * @param command command to execute
     * @param commandLine command line
     * @throws InterruptedException if the command wants to stop the console
     */
    private void runCommand(final Command command, final String commandLine)
    throws InterruptedException
    {
        Thread commandThread = new Thread() {
            public void run() {
                try {
                    command.run(commandLine);
                }
                catch (Throwable exception) {
                    err.print("Error: ");
                    
                    if (exception.getMessage() != null) {
                        err.println(exception.getMessage());
                    }
                    else {
                        err.println("Exception caught: " + exception);
                    }
                }
            }
        };
        
        commandThread.start();
        commandThread.join();
    }
}
