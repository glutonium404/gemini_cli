#pragma once

#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "httplib.h"

class HTTPHandler {
private:
    HTTPHandler() = delete;

    static std::pair<bool, std::string> _send_post_request(const std::string& request_body, std::string model) {
        const char* api_key_env = std::getenv("GEMINI_API_KEY");

        if (!api_key_env) {
            std::cerr << "Error: GEMINI_API_KEY environment variable is not set." << std::endl;
            return {false, ""};
        }

        std::string api_key = api_key_env;

        httplib::SSLClient cli("generativelanguage.googleapis.com");
        cli.set_read_timeout(15, 0);

        std::string path = "/v1beta/models/" + model + ":generateContent?key=" + api_key;

        auto res = cli.Post(path.c_str(), request_body, "application/json");

        if (!res) {
            std::cerr << "Network error: Request failed to send or timed out." << std::endl;
            return {false, ""};
        }

        if (res->status != 200) {
            std::cerr << "HTTP Error " << res->status << ": " << res->body << std::endl;
            return {false, ""};
        }

        return {true, res->body};
    }
public:
    static std::pair<bool, std::string> send_post_request(const std::string& request_body) {

        std::string models[] = {
            "gemini-3.1-flash-lite-preview",
            "gemini-3-flash-preview",
            "gemini-2.5-flash",
        };

        for(int i=0; i<models->size(); i++) {
            std::cout << "\n\n\n[[[  Trying " + models[i] + "  ]]]" + "\n\n\n" << std::endl;
            auto ret = _send_post_request(request_body, models[i]);

            if(ret.first == true) return ret;
        }

        return {false, ""};
    }
};
