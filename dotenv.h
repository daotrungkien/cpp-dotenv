// A header-only library for reading information from (upper item having higher priority):
// - Command-line parameters (in the form: `--VAR_NAME=VALUE`, or `--VAR_NAME`)
// - .env file
// - Environment variables
//
// Dao Trung Kien
// https://github.com/daotrungkien


#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>

#ifdef _WIN32
    #include <Windows.h>
#elif __linux__
    #include <limits.h>
    #include <unistd.h>
#elif
    #pragma message("dotenv::in_program_folder() is not supported on your platform.")
#endif


class dotenv {
protected:
    enum class var_source { command_line, env_variable, dotenv_file };

    std::multimap<std::pair<std::string, var_source>, std::string> variables;

    static std::string trim(const std::string& s) {
        auto wsfront = std::find_if_not(s.begin(), s.end(), [](char c) { return std::isspace(c); });
        auto wsback = std::find_if_not(s.rbegin(), s.rend(), [](char c) { return std::isspace(c); }).base();
        return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
    }

    void read_variable(const std::string& s, var_source source) {
        auto pos = s.find('=');
        if (pos > 0) {
            std::string name = trim(s.substr(0, pos));
            std::string value = s.substr(pos + 1);  // value is not trimed, leading and trailing spaces are kept
            if (!name.empty()) variables.emplace(std::make_pair(std::make_pair(name, source), value));
        } else {
            std::string name = trim(s);
            if (!name.empty()) variables.emplace(std::make_pair(std::make_pair(name, source), std::string()));
        }
    }

    void read_command_line(int argc, const char** argv) {
        for (int i = 1; i < argc; i++) {    // i starts from 1 to skip the first argument which is the command itself
            std::string s = argv[i];
            if (s.substr(0, 2) != "--") continue;

            read_variable(s.substr(2), var_source::command_line);
        }
    }

    void read_dotenv_file(const std::string& dotenv_path) {
        std::ifstream file(dotenv_path, std::ios::in);

        std::string s;
        while (std::getline(file, s)) {
            read_variable(s, var_source::dotenv_file);
        }
    }

public:
    dotenv(
        int argc = 0, const char** argv = nullptr,
        bool env_vars = true,
        const std::initializer_list<std::string> dotenv_paths = {
            dotenv::in_current_folder(".env"),
            dotenv::in_program_folder(".env")
        })
    {
        if (argc > 0) read_command_line(argc, argv);

        for (auto& path : dotenv_paths)
            if (!path.empty()) read_dotenv_file(path);
    }


    dotenv(int argc, const char** argv, bool env_vars, const std::string& dotenv_path) {
        if (argc > 0) read_command_line(argc, argv);

        if (!dotenv_path.empty()) read_dotenv_file(dotenv_path);
    }


    // checks if a variable exists
    bool exists(const std::string& name) const {
        // searches command-line items
        auto itr_cmdline = variables.find(std::make_pair(name, var_source::command_line));
        if (itr_cmdline != variables.end()) return true;

        // searches .env file
        auto itr_dotenv = variables.find(std::make_pair(name, var_source::dotenv_file));
        if (itr_dotenv != variables.end()) return true;
        
        // searches environment variable items
        const char* value = std::getenv(name.c_str());
        if (value) return true;

        return false;
    }


    // returns the value of a variable if it exists, or an empty string if it does not exist
    std::string operator[](const std::string& name) const {
        // searches command-line items
        auto itr_cmdline = variables.find(std::make_pair(name, var_source::command_line));
        if (itr_cmdline != variables.end()) return itr_cmdline->second;

        // searches .env file
        auto itr_dotenv = variables.find(std::make_pair(name, var_source::dotenv_file));
        if (itr_dotenv != variables.end()) return itr_dotenv->second;
        
        // searches environment variable items
        const char* value = std::getenv(name.c_str());
        if (value) return value;

        return std::string();
    }


    static std::string get_program_path() {
#ifdef _WIN32
        char exe_path[MAX_PATH];
        return std::string(exe_path, GetModuleFileNameA(NULL, exe_path, MAX_PATH));
#elif __linux__
        char exe_path[PATH_MAX];
        std::size_t count = readlink("/proc/self/exe", exe_path, PATH_MAX);
        return std::string(exe_path, count > 0 ? count : 0);
#else
        throw std::runtime_error("dovenv");
#endif
    }

    static std::string get_program_folder() {
        std::string exe_path = get_program_path();

        std::size_t last_slash_idx = exe_path.rfind('\\');
        if (std::string::npos == last_slash_idx) last_slash_idx = exe_path.rfind('/');
        if (std::string::npos == last_slash_idx) return std::string();

        return exe_path.substr(0, last_slash_idx);
    }

    static std::string in_current_folder(const std::string& relative_file_path = std::string(".env")) {
        return relative_file_path;
    }

    static std::string in_program_folder(const std::string& relative_file_path = std::string(".env")) {
        return get_program_folder() + "/" + relative_file_path;
    }
};
