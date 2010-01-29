package joi.ui.text;


/**
 * Console command that uses a command line.
 */
public abstract class Command implements Comparable<Command> {
    public int compareTo(Command command) {
        return getName().compareTo(command.getName());
    }
    

    /**
     * Gets the description of this command.
     * 
     * @return a detailed description of this command
     */
    public abstract String getDescription();
    

    /**
     * Gets the name of this command.
     * 
     * @return this command's name
     */
    public abstract String getName();
    

    /**
     * Gets usage information on this command.
     * 
     * @return usage information
     */
    public abstract String getUsage();
    

    /**
     * Executes this command.
     * 
     * @param commandLine command line
     */
    public abstract void run(String commandLine);
}
