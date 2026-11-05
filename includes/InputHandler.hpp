#pragma once

#include "FlagHandler.hpp"
#include "PromptInstructions.hpp"
#include "Utils.hpp"
#include <iostream>
#include <string>
#include <vector>

class InputHandler {
public:
    struct ParsedInput {
        std::string prompt;
        std::vector<std::string_view> instructions;
    };
public:
    InputHandler() {
        if(IS_PIPED_DATA(stdin)) { // if no piped data then some sort of argument must be provided
            std::string line;
            while (std::getline(std::cin, line)) {
                piped_data_cache += line + "\n";
            }
        }
    }

    ParsedInput parse_input(const std::vector<std::string_view>& args, const Flags& flags) const {

        if(piped_data_cache.empty())
            assert_prompt(args.empty());

        ParsedInput pi;

        pi.prompt = "USER QUERY:\n\n";

        for(std::string_view arg : args) {
            pi.prompt += arg;
            pi.prompt += " ";
        }

        pi.prompt += "\n\n";

        if(!flags.long_res)  pi.instructions.push_back(Instructions::short_res);
        if(!flags.markdown)  pi.instructions.push_back(Instructions::plain_format);
        if( flags.ansi)      pi.instructions.push_back(Instructions::ansi);
        if( flags.nerd_icon) pi.instructions.push_back(Instructions::nerd_icon);

        pi.prompt += get_piped_data(true);
        return pi;
    }

private:
    std::string piped_data_cache = "";

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
};
