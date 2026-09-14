#include <cstdlib>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include "json.hpp"
#include "utils.hpp"

#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif

using json = nlohmann::json;

void assert_api_key() {
    char* API_KEY = std::getenv("GEMINI_API_KEY");

    if(!API_KEY) {
        throw std::runtime_error("GEMINI_API_KEY not set");
        exit(-1);
    }
}

void assert_prompt(int argc) {
    if(argc <= 1) {
        std::cout << "Prompt not provided." << std::endl;
        exit(-1);
    }
}

bool is_piped_data() {
#if defined(_WIN32)
    return _isatty(_fileno(stdin)) == 0;
#else
    return isatty(fileno(stdin)) == 0;
#endif
}

std::string get_formatted_piped_data() {
    if(!is_piped_data()) return "";

    std::string line;
    std::string data = "";

    while(std::getline(std::cin, line)) {
        data += line + "\n";
    }

    return "CONTEXT DATA:\n```\n" + data + "\n```";
}

std::string get_final_prompt(int argc, char* argv[]) {
    bool is_long = false;
    bool is_markdown = false;

    std::string prompt = "";

    if(argc > 1) {
        prompt = "USER QUERY:\n";

        for(int i=1; i<argc; i++) {
            if(!is_long && Utils::check_is_long(argv[i])) {
                is_long = true;
                continue;
            }

            if(!is_markdown && Utils::check_is_markdown(argv[i])) {
                is_markdown = true;
                continue;
            }

            prompt += argv[i];
            prompt += " ";
        }

        prompt += "\n\n";
    }

    if(!is_long)        prompt = std::string(Utils::short_res)    + prompt;
    if(!is_markdown)    prompt = std::string(Utils::plain_format) + prompt;

    return prompt + get_formatted_piped_data();
}

int main(int argc, char* argv[]) {

    if(!is_piped_data()) assert_prompt(argc);

    assert_api_key();

    std::string final_prompt = get_final_prompt(argc, argv);

    std::cout << final_prompt << std::endl;

    return 0;
}
