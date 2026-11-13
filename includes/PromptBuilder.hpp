#pragma once

#include "Flags.hpp"
#include "PromptInstructions.hpp"
#include "Utils.hpp"
#include <iostream>
#include <string>
#include <vector>

class PromptBuilder {
public:
    PromptBuilder() {
        if(IS_PIPED_DATA(stdin)) { // if no piped data then some sort of argument must be provided
            std::string line;
            while (std::getline(std::cin, line)) {
                piped_data_cache += line + "\n";
            }
        }
    }

    void build_prompt(
        const std::vector<std::string_view>& args,
        const Flags& flags,
        std::string& prompt,
        std::string& sys_instruction
    ) const {

        if(piped_data_cache.empty())
            validate_prompt(args.empty());

        prompt = "USER QUERY:\n\n";

        for(std::string_view arg : args) {
            prompt += arg;
            prompt += " ";
        }

        prompt += "\n\n";

        if(!flags.long_res)  sys_instruction.append(Instructions::short_res).append("\n");
        if(!flags.markdown)  sys_instruction.append(Instructions::plain_format).append("\n");
        if( flags.ansi)      sys_instruction.append(Instructions::ansi).append("\n");
        if( flags.nerd_icon) sys_instruction.append(Instructions::nerd_icon);

        prompt += get_piped_data(true);
    }

private:
    std::string piped_data_cache = "";

    inline void validate_prompt(bool is_empty) const {
        Utils::throw_if(is_empty, "Prompt not provided.");
    }

    std::string get_piped_data(bool format = false) const {
        if (piped_data_cache.empty()) return "";
        if (format)
            return "CONTEXT DATA:\n```\n" + piped_data_cache + "\n```";
        return piped_data_cache;
    }
};
