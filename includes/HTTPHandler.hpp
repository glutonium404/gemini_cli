#pragma once

#include <functional>
#include <string>
#include "EnvConfig.hpp"
#include "httplib.h"

struct GeminiResponse {
    std::string body;
    bool success = false;
    int status = -1;
    std::string err;
};

struct Payload {
    std::string model;
    std::string input;
    std::string system_instruction;
    bool stream = false;
    bool store = true;
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

    GeminiResponse get_res(const std::string& request_body) {
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

    using ChunkHandler = std::function<bool(std::string_view)>;

    GeminiResponse get_streamed_res(const std::string& request_body, const ChunkHandler& on_chunk) {
        auto result = httplib::stream::Post(client, base_path, request_body, "application/json");

        GeminiResponse response;

        if(!result) {
            response.success = false;
            response.err = "Request failed to send or timed out.";
            return response;
        }

        if (result.status() != 200) {
            response.success = false;
            response.err = "HTTP Status: " + std::to_string(result.status());
            return response;
        }

        response.status = 200;
        bool stream_success = false;

        // expected example SSE (server sent event)
        /*
            event: step.delta
            data: {"index":1,"delta":{"text":" to assist [...] without markdown","type":"text"},"event_type":"step.delta"}

            event: step.delta
            data: {"index":1,"delta":{"text":" symbols \nlike [...] or scripts","type":"text"},"event_type":"step.delta"}
        */
        while(result.next()) {
            stream_success = on_chunk({result.data(), result.size()});
            if(stream_success)
                break;
        }

        if(!stream_success) {
            response.err = "Stream ended before 'interaction.complete' was received.";
        }

        response.success = stream_success;
        return response;
    }
};
