package joi.ui.text;


import java.util.LinkedList;
import java.util.List;
import java.util.regex.Matcher;


/**
 * Console command that uses an array of arguments.
 */
public abstract class ArgumentsCommand extends Command {
    public final void run(String commandLine) {
        List<String> arguments = new LinkedList<String>();
        Matcher matcher = Console.ARGUMENTS_PATTERN.matcher(commandLine);
        
        while (matcher.find()) {
            arguments.add(matcher.group());
        }
        
        run(arguments.toArray(new String[] {}));
    }
    

    /**
     * Executes this command.
     * 
     * @param arguments command line arguments
     */
    public abstract void run(String[] arguments);
}
