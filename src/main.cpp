#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <utility>
#include "json.hpp"
#include "httplib.h"
#include "utils.hpp"

#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif

using json = nlohmann::json;

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

std::string get_final_prompt(const int argc, const char* const argv[]) {
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

/**
 * @param final_prompt std::string
* {
*   "contents": [
*     {
*       "parts": [
*         {
*           "text": final_prompt
*         }
*       ]
*     }
*   ]
* }
*/
std::string get_request_body(const std::string final_prompt) {
    json payload;

    payload["contents"] = json::array({ // <-------------- array initializer list
        { // <-------------------------------------------- the array itsel
            { // <---------------------------------------- the array item , i.e object
                "parts", // <----------------------------- key value pair
                json::array({ // <------------------------ the initializer object
                    { // <-------------------------------- the array itself
                        { "text", final_prompt }
                    }
                })
            }
        }
    });

    return payload.dump();
}
// {
//   "candidates": [
//     {
//       "content": {
//         "parts": [
//           {
//             "text": "Generated response string"
//           }
//         ]
//       }
//     }
//   ]
// }
std::string parse_json_response(const std::string res_body) {
    std::string response = "";

    try {
        json response_json = json::parse(res_body);
        response = response_json["candidates"][0]["content"]["parts"][0]["text"];
    } catch (const std::exception& e) {
        std::cerr << "JSON Parsing error: " << e.what() << std::endl;
    }

    return response;
}

std::pair<bool, std::string> make_request(const std::string request_body) {
    httplib::Client cli("https://generativelanguage.googleapis.com");

    // 30s timeout
    cli.set_read_timeout(15, 0);

    const std::string API_KEY = std::getenv("GEMINI_API_KEY");
    const std::string path = "/v1beta/models/gemini-2.5-flash:generateContent?key=" + API_KEY;

    auto res = cli.Post(path.c_str(), request_body, "application/json");

    if (!res) {
        std::cerr << "Network error: Request failed to send." << std::endl;
        return std::pair(false, "");
    }

    if (res->status != 200) {
        std::cerr << "HTTP Error " << res->status << ": " << res->body << std::endl;
        return std::pair(false, "");
    }

    return std::pair(true, parse_json_response(res->body));
}

int main(int argc, char* argv[]) {

    if(!is_piped_data()) Utils::assert_prompt(argc);

    Utils::assert_api_key();

    auto response = make_request(
        get_request_body(
            get_final_prompt(argc, argv)
        )
    );

    if (response.first) {
        std::cout << response.second << std::endl;
    }else {
        std::cout << "Something went wrong :'(" << std::endl;
    }

    return 0;
}
