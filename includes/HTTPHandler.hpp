#pragma once

#include <string>
#include "EnvConfig.hpp"
#include "httplib.h"

struct GeminiResponse {
    std::string body;
    bool success = false;
    int status = -1;
    std::string err;
};

class HTTPHandler {
private:
    httplib::SSLClient client = httplib::SSLClient("generativelanguage.googleapis.com");
    std::string base_path = "/v1beta/interactions/";

public:
    HTTPHandler(const EnvConfig& env_config, int timeout)
    {
        client.set_read_timeout(timeout, 0);
        client.set_default_headers({
            { "x-goog-api-key", env_config.api_key() },
            { "Content-Type"  , "application/json" }
        });
    }

    GeminiResponse send_req(const std::string& request_body) {
        GeminiResponse gemini_res;

        httplib::Result res = client.Post(
            base_path,
            request_body,
            "application/json"
        );

        if (!res) {
            gemini_res.status  = 0;
            gemini_res.success = false;
            gemini_res.err     = "Request failed to send or timed out.";
            return gemini_res;
        }

        if (res->status != 200) {
            gemini_res.status  = res->status;
            gemini_res.success = false;
            gemini_res.err     = "HTTP Status: " + std::to_string(res->status) + "\n" + res->body;
            return gemini_res;
        }

        gemini_res.status   = res->status;
        gemini_res.success  = true;
        gemini_res.body = res->body;

        return gemini_res;
    }
};
