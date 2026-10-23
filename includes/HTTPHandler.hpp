#pragma once

#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include "FlagHandler.hpp"
#include "httplib.h"

class HTTPHandler {
public:
    std::optional<std::string> send_post_request(const std::string& request_body, const Flags& flags) {
        std::array<std::string, 3> models = {
            "gemini-3-flash-preview",
            "gemini-3.1-flash-lite-preview",
            "gemini-2.5-flash",
        };

        for(size_t i=0; i<models.size(); i++) {
            if(flags.verbose) {
                std::cout << "\n\n\n[[[  Trying " + models[i] + "  ]]]" + "\n\n\n" << std::endl;
            }

            auto ret = _send_post_request(request_body, models[i], flags.timeout);

            if(ret.has_value() == true) return ret;
        }

        return std::nullopt;
    }

private:
    std::optional<std::string> _send_post_request(const std::string& request_body, const std::string& model, int timeout = 8) {
        const char* api_key_env = std::getenv("GEMINI_API_KEY");

        if (!api_key_env) {
            std::cerr << "Error: GEMINI_API_KEY environment variable is not set." << std::endl;
            return std::nullopt;
        }

        httplib::SSLClient cli("generativelanguage.googleapis.com");
        cli.set_read_timeout(timeout, 0);

        std::string path = "/v1beta/models/" + model + ":generateContent?key=" + api_key_env;

        httplib::Result res = cli.Post(path.c_str(), request_body, "application/json");

        if (!res) {
            std::cerr << "Network error: Request failed to send or timed out." << std::endl;
            return std::nullopt;
        }

        if (res->status != 200) {
            std::cerr << "HTTP Error " << res->status << ": " << res->body << std::endl;
            return std::nullopt;
        }

        return res->body;
    }
};
