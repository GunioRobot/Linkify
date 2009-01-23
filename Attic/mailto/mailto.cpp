/**
 * Fixes the mailto protocol between Opera and Thunderbird.
 *
 * In Opera, go to Tools > Preferences > Advanced > Programs, edit the mailto
 * protocol and use "mailto:?to=%t&subject=%s&body=%m&cc=%c" as the parameter
 * for this application.
 */


#include <iostream>
#include <string>

#ifdef _WIN32
#   include <Windows.h>
#endif


static bool Thunderbird(std::string mailto) {
#ifdef _WIN32
    STARTUPINFO startup_info;
    PROCESS_INFORMATION process_info;
    std::string path = "thunderbird.exe " + mailto;
    
    ZeroMemory(&startup_info, sizeof(startup_info));
    ZeroMemory(&process_info, sizeof(process_info));
    startup_info.cb = sizeof(startup_info);
    
    BOOL success = CreateProcess(
        NULL, const_cast<char*>(path.c_str()), NULL, NULL, FALSE, 0, NULL,
        NULL, &startup_info, &process_info);
    
    return static_cast<bool>(success);
#else
    return false;
#endif
}


static std::string escape(std::string str, char* chars = "<>") {
    for (std::string::size_type pos; *chars != '\0'; ++chars) {
        while ((pos = str.find(*chars)) != std::string::npos) {
            str.replace(pos, 1, "");
        }
    }
    
    return str;
}


static std::string join(char** array, size_t length, char* separator = " ") {
    std::string buffer;
    
    if (length == 0) {
        return buffer;
    }
    
    buffer = *array++;
    --length;
    
    for (size_t i = 0; i < length; ++i) {
        buffer += std::string(separator) + std::string(array[i]);
    }
    
    return buffer;
}


#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE instance, HINSTANCE previous_instance,
                     LPTSTR command_line, int command_show) {
    Thunderbird(escape(command_line));
    return 0;
}
#else
int main(int argc, char** argv) {
    Thunderbird(escape(join(argv + 1, argc - 1)));
    return 0;
}
#endif
