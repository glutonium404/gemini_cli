#include <iostream>
#include "HTTPHandler.hpp"
#include "InputHandler.hpp"
#include "JSONHandler.hpp"

int main(int argc, char* argv[]) {
    InputHandler input_handler(argc, argv);
    std::string full_prompt = input_handler.build_final_prompt();

    std::string request_body = JSONHandler::generate_request_body(full_prompt);

    auto [success, response_body] = HTTPHandler::send_post_request(request_body);

    if (success) {
        std::string parsed_response = JSONHandler::parse_response_body(response_body);
        std::cout << parsed_response << std::endl;
    } else {
        std::cerr << "Failed to process request." << std::endl;
        return 1;
    }

    return 0;
}
