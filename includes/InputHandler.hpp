#pragma once

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif

class InputHandler {
public:
    InputHandler(const int argc, const char* const* argv)
    : argc(argc), argv(argv)
    {
        if(!is_piped_data()) { // if no piped data then some soft of argument must be provided
            assert_prompt(argc);
        }else {
            std::string line;
            while (std::getline(std::cin, line)) {
                piped_data_cache += line + "\n";
            }
        }
    }

    std::string get_piped_data(bool formatted = false) const {
        if (piped_data_cache.empty()) return "";
        if (formatted)
            return "CONTEXT DATA:\n```\n" + piped_data_cache + "\n```";
        return piped_data_cache;
    }

    std::string build_final_prompt() const {
        bool is_long = false;
        bool is_markdown = false;

        std::string prompt = "";

        if(argc > 1) {
            prompt = "USER QUERY:\n";

            for(int i=1; i<argc; i++) {
                if(!is_long && check_flag_l(argv[i])) {
                    is_long = true;
                    continue;
                }

                if(!is_markdown && check_flag_m(argv[i])) {
                    is_markdown = true;
                    continue;
                }

                if(check_flag_h(argv[i])) {
                    std::cout
                        << "gemini_cli - terminal client for Gemini API\n\n"
                        << "Usage:\n"
                        << "  gemini_cli [options] <prompt>\n"
                        << "  echo \"context\" | gemini_cli [options] <prompt>\n\n"
                        << "Options:\n"
                        << "  -h, --help      Show this help message and exit\n"
                        << "  -l, --long      Allow longer responses (disable short response instruction)\n"
                        << "  -m, --markdown  Enable markdown output (disable plain-text instruction)\n\n"
                        << "Notes:\n"
                        << "  - Set GEMINI_API_KEY in your environment before running.\n"
                        << "  - Piped stdin is added as context data to the prompt.\n";
                    std::exit(0);
                }

                prompt += argv[i];
                prompt += " ";
            }

            prompt += "\n\n";
        }

        if(!is_long)        prompt = std::string(short_res)    + prompt;
        if(!is_markdown)    prompt = std::string(plain_format) + prompt;

        return prompt + get_piped_data(true);
    }

private:
    std::string piped_data_cache = "";
    const int argc;
    const char* const* argv;

    std::string short_res = "USER INSTRUCTION:\nkeep the ans short and concise withing 15 to 20 lines if possible, if needed write more. just give me the reponse directly and do not add any extra text\n\n";

    std::string plain_format = "USER INSTRUCTION:\ni am on terminal which can not render markdown. so i want you to write the response in plain text. and format or beautify using plain text.\n\n";

    inline bool check_flag_l(const char* arg) const {
        return std::strcmp(arg, "-l") == 0 || strcmp(arg, "--long") == 0;
    }

    inline bool check_flag_m(const char* arg) const {
        return strcmp(arg, "-m") == 0 || strcmp(arg, "--markdown") == 0;
    }

    inline bool check_flag_h(const char* arg) const {
        return strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0;
    }

    inline void assert_prompt(const int argc) const {
        if(argc <= 1) {
            std::cerr << "Prompt not provided." << std::endl;
            exit(-1);
        }
    }

    bool is_piped_data() const {
#if defined(_WIN32)
        return _isatty(_fileno(stdin)) == 0;
#else
        return isatty(fileno(stdin)) == 0;
#endif
    }
};
