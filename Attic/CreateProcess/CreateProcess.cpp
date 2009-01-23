#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <Windows.h>


#define CONFIGURATION_FILE "CreateProcess.lst"

typedef std::vector<std::string> Arguments;

static std::map<std::string, bool (*)(Arguments)> _commands;


static bool command_wait(Arguments arguments) {
    if (arguments.size() != 2) {
        std::cerr << "Not enough arguments.\n";
        return false;
    }
    
    Sleep(atoi(arguments[1].c_str()) * 1000);
    return true;
}


static bool create_process(std::string path) {
    STARTUPINFO startup_info;
    PROCESS_INFORMATION process_info;
    
    ZeroMemory(&startup_info, sizeof(startup_info));
    ZeroMemory(&process_info, sizeof(process_info));
    startup_info.cb = sizeof(startup_info);
    
    BOOL success = CreateProcess(
        NULL, const_cast<char*>(path.c_str()), NULL, NULL, FALSE, 0, NULL,
        NULL, &startup_info, &process_info);
    
    return static_cast<bool>(success);
}


static void initialize_commands() {
    _commands["wait"] = command_wait;
}


static void read_command(std::string command) {
    Arguments arguments;
    std::string separator = " ";
    std::string::size_type position, previous = 0;
    
    while (true) {
        position = command.find(separator, previous);
        
        if (position == std::string::npos) {
            arguments.push_back(command.substr(previous));
            break;
        }
        
        arguments.push_back(command.substr(previous, position));
        previous = position + separator.size();
    }
    
    if (_commands.find(arguments[0]) == _commands.end()) {
        std::cerr << "Unknown command: \"" << command << "\".\n";
        exit(1);
    }
    
    if (!_commands[arguments[0]](arguments)) {
        std::cerr << "Command error in \"" << arguments[0] << "\".\n";
        exit(1);
    }
}


static void run() {
    std::ifstream file(CONFIGURATION_FILE);
    
    if (!file) {
        std::cerr << "Can't open configuration file.\n";
        exit(1);
    }
    
    std::string line;
    
    for (char c; file.get(c) != '\0'; ) {
        if (c != '\n') {
            line += c;
            continue;
        }
        
        if (line[0] != ':') {
            if (!create_process(line)) {
                std::cerr << "Can't execute: \"" << line << "\".\n";
                exit(1);
            }
        }
        else {
            read_command(line.substr(1));
        }
        
        line = "";
    }
}


int APIENTRY WinMain(HINSTANCE instance, HINSTANCE previous_instance,
                     LPTSTR command_line, int command_show) {
    initialize_commands();
    run();
	return 0;
}
