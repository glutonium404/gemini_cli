#pragma once

#include "FlagHandler.hpp"
#include <iostream>
#include <string>
#include <vector>

#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif

class InputHandler {
public:
    struct ParsedInput {
        std::string prompt;
        std::vector<std::string> instructions;
    };
public:
    InputHandler() {
        if(is_piped_data()) { // if no piped data then some soft of argument must be provided
            std::string line;
            while (std::getline(std::cin, line)) {
                piped_data_cache += line + "\n";
            }
        }
    }

    ParsedInput parse_input(const std::vector<std::string_view>& args, const Flags& flags) const {

        if(piped_data_cache.empty())
            assert_prompt(args.empty());

        std::string prompt = "USER QUERY:\n\n";
        std::vector<std::string> instructions;

        for(std::string_view arg : args) {
            prompt += arg;
            prompt += " ";
        }

        prompt += "\n\n";

        if(!flags.long_res) instructions.push_back(inst_short_res)   ;
        if(!flags.markdown) instructions.push_back(inst_plain_format);
        if( flags.ansii)    instructions.push_back(inst_ansii);

        return {
            .prompt       = prompt + get_piped_data(true),
            .instructions = instructions
        };
    }

private:
    std::string piped_data_cache = "";

    std::string inst_short_res = "keep the ans short and concise withing 15 to 20 lines if possible, write more if needed.";

    std::string inst_plain_format = "i am on terminal which can not render markdown. so i want you to write the response in plain text. and format or beautify using plain text. you are allowed to use nerd fonts to beautify";

    std::string inst_ansii = "i am on terminal. so you can use ansii escape codes to color and format texts. so do use that";

    inline void assert_prompt(bool is_empty) const {
        if(is_empty) {
            std::cerr << "Prompt not provided." << std::endl;
            exit(-1);
        }
    }

    std::string get_piped_data(bool format = false) const {
        if (piped_data_cache.empty()) return "";
        if (format)
            return "CONTEXT DATA:\n```\n" + piped_data_cache + "\n```";
        return piped_data_cache;
    }

    bool is_piped_data() const {
#if defined(_WIN32)
        return _isatty(_fileno(stdin)) == 0;
#else
        return isatty(fileno(stdin)) == 0;
#endif
    }
};
