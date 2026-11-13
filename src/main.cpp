#include <cstdlib>
#include <iostream>
#include "ANSI.hpp"
#include "EnvConfig.hpp"
#include "FlagHandler.hpp"
#include "HTTPHandler.hpp"
#include "InputHandler.hpp"
#include "JSONHandler.hpp"
#include "Utils.hpp"

typedef InputHandler::ParsedInput ParsedInput;

using namespace std;

int main(int argc, char* argv[]) {
    try {
        // ANSI ansi;
        // auto str = Utils::test_string();
        // cout << ansi.parse_to_ansi(str) << endl;
        // return 0;

        vector<std::string_view> args;
        args.reserve(argc);

        for(int i=1; i<argc; i++) {
            args.push_back(argv[i]);
        }

        FlagHandler flag_handler;
        Flags flags = flag_handler.parse_flags(args);

        if((!IS_PIPED_DATA(stdin) && args.empty()) || flags.help) {
            Utils::print_help();
            return 0;
        }

        EnvConfig    env_config{ flags.model };
        InputHandler input_handler;
        JSONHandler  json_handler;
        HTTPHandler  http_handler{ env_config };

        ParsedInput parsed_input  = input_handler.parse_input(args, flags);
        string request_body       = json_handler.generate_request_body(parsed_input);
        std::string response      = http_handler.send_post_request(request_body, flags);

        string parsed_response = json_handler.parse_response_body(response);

        if(flags.ansi) {
            ANSI ansi;
            parsed_response = ansi.parse_to_ansi(parsed_response);
        }

        cout << parsed_response << endl;

        return 0;
    } catch(const std::exception& e) {
        std::cerr << "[Error] : " << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
