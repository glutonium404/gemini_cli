#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include "EnvConfig.hpp"
#include "Flags.hpp"
#include "Utils.hpp"
#include "httplib.h"

class HTTPHandler {
private:
    const EnvConfig& env_config;

    httplib::SSLClient client = httplib::SSLClient("generativelanguage.googleapis.com");
    std::string base_path = "/v1beta/models/";

public:
    HTTPHandler(const EnvConfig& env_config): env_config(env_config)
    {}

    std::string send_post_request(const std::string& request_body, const Flags& flags) {
        std::vector<std::string> models;
        std::string response;
        bool no_response = true;

        if(flags.model.empty()) {
            models = env_config.def_models();
        }else {
            models = { flags.model };
        }

        for(const auto& model : models) {
            if(flags.verbose) {
                std::cout << "\n\n\n[[[  Trying " << model << "  ]]]" << "\n\n\n" << std::endl;
            }

            auto ret = make_post_req(request_body, model, flags.timeout, flags.quiet);

            if(ret.has_value()) {
                no_response = false;
                response = ret.value();
                break;
            };
        }

        Utils::throw_if(no_response, "Failed to process request.");

        return response;
    }

private:
    std::optional<std::string> make_post_req(const std::string& request_body, std::string_view model, int timeout, bool quiet) {
        client.set_read_timeout(timeout, 0);

        std::string path = base_path + std::string(model) + ":generateContent?key=" + env_config.api_key();

        httplib::Result res = client.Post(path, request_body, "application/json");

        if (!res) {
            if(!quiet)
                std::cerr << "Network error: Request failed to send or timed out." << "\n" << res.error() << "\n";

            return std::nullopt;
        }

        if (res->status != 200) {
            if(!quiet)
                std::cerr << "HTTP Error: " << res->status << "\n" << res->body << "\n";

            return std::nullopt;
        }

        return res->body;
    }
};
