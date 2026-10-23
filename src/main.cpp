#include <array>
#include <iostream>
#include "FlagHandler.hpp"
#include "HTTPHandler.hpp"
#include "InputHandler.hpp"
#include "JSONHandler.hpp"

typedef InputHandler::ParsedInput ParsedInput;

using namespace std;

inline void print_help();

int main(int argc, char* argv[]) {
    vector<std::string_view> args;
    args.reserve(argc);

    for(int i=1; i<argc; i++) {
        args.push_back(argv[i]);
    }

    FlagHandler  flag_handler;
    Flags flags = flag_handler.parse_flags(args);

    if(flags.help) {
        print_help();
        return 0;
    }

    InputHandler input_handler;
    JSONHandler  json_handler;
    HTTPHandler  http_handler;


    ParsedInput parsed_input  = input_handler.parse_input(args, flags);
    string request_body       = json_handler.generate_request_body(parsed_input);
    optional<string> response = http_handler.send_post_request(request_body, flags);

    if(!response.has_value()) {
        cerr << "Failed to process request." << endl;
        return 1;
    }

    string parsed_response = json_handler.parse_response_body(response.value());
    cout << parsed_response << endl;

    return 0;
}

inline void print_help() {
    cout
        << "gemini_cli - terminal client for Gemini API\n\n"
        << "Usage:\n"
        << "  gemini_cli [options] <prompt>\n"
        << "  echo \"context\" | gemini_cli [options] <prompt>\n\n"
        << "Options:\n"
        << "  -h, --help      Show this help message and exit\n"
        << "  -l, --long      Allow longer responses (disable short response instruction)\n"
        << "  -m, --markdown  Enable markdown output (disable plain-text instruction)\n"
        << "  -a, --ansii     Enable ansii escape code for color and formatting on terminal\n"
        << "  -v, --verbose   Prints model names as it tries one after another\n"
        << "  -t, --timeout   Network request timeout when requesting each models. (5 <= timeout <= 60)\n"
        << "  --model         Provide custom model id (e.g: gemini-3-flash-preview)\n\n"
        << "Notes:\n"
        << "  - Set GEMINI_API_KEY in your environment before running.\n"
        << "  - Piped stdin is added as context data to the prompt.\n";
}
