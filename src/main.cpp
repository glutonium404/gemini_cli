#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include "json.hpp"
#include "utils.hpp"

using json = nlohmann::json;

std::string get_final_prompt(int argc, char* argv[]) {
    bool is_long = false;
    bool is_markdown = false;

    std::string prompt = "USER QUERY:\n";

    if(argc > 1) {
        for(int i=1; i<argc; i++) {
            if(!is_long && Utils::check_is_long(argv[i])) {
                is_long = true;
                continue;
            }

            if(!is_markdown && Utils::check_is_markdown(argv[i])) {
                is_markdown = true;
                continue;
            }

            prompt += " ";
            prompt += argv[i];
        }
    }

    if(!is_long)        prompt = std::string(Utils::short_res)    + prompt;
    if(!is_markdown)    prompt = std::string(Utils::plain_format) + prompt;

    return prompt;
}

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

int main(int argc, char* argv[]) {

    assert_prompt(argc);
    assert_api_key();

    std::string final_prompt = get_final_prompt(argc, argv);

    std::cout << final_prompt << std::endl;

    return 0;
}
