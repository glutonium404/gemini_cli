#pragma once

#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include "FlagHandler.hpp"
#include "httplib.h"

class HTTPHandler {
public:
    HTTPHandler() {
        if (!api_key) {
            std::cerr << "Error: GEMINI_API_KEY environment variable is not set." << std::endl;
            std::exit(1);
        }
    }

    std::optional<std::string> send_post_request(const std::string& request_body, const Flags& flags) {
        std::vector<std::string_view> models;
        models.reserve(3);

        if(flags.custom_model_id.empty()) {
            models.push_back("gemini-3-flash-preview");
            models.push_back("gemini-3.1-flash-lite-preview");
            models.push_back("gemini-2.5-flash");
        }else {
            assert_model_existence(flags.custom_model_id);
            models.push_back(flags.custom_model_id);
        }

        for(size_t i=0; i<models.size(); i++) {
            if(flags.verbose) {
                std::cout << "\n\n\n[[[  Trying " << models[i] << "  ]]]" << "\n\n\n" << std::endl;
            }

            auto ret = _send_post_request(request_body, models[i], flags.timeout);

            if(ret.has_value() == true) return ret;
        }

        return std::nullopt;
    }

private:
    httplib::SSLClient client = httplib::SSLClient("generativelanguage.googleapis.com");
    std::string base_path = "/v1beta/models/";
    const char* api_key = std::getenv("GEMINI_API_KEY");

    std::optional<std::string> _send_post_request(const std::string& request_body, std::string_view model, int timeout) {
        client.set_read_timeout(timeout, 0);

        std::string path = base_path + std::string(model) + ":generateContent?key=" + api_key;

        httplib::Result res = client.Post(path, request_body, "application/json");

        if (!res) {
            std::cerr << "Network error: Request failed to send or timed out." << std::endl;
            return std::nullopt;
        }

        if (res->status != 200) {
            std::cerr << "HTTP Error " << res->status << ":\n" << res->body << std::endl;
            return std::nullopt;
        }

        return res->body;
    }

    void assert_model_existence(std::string_view model_id) {
        std::string path = base_path + std::string(model_id) + "?key=" + api_key;
        auto res = client.Get(path);

        if(res->status != 200) {
            std::cerr << "Provided custom model id: " << model_id << " is invalid\nHTTP Error: " << res->status << ":\n" << res->body << std::endl;
            std::exit(1);
        }
    }
};
