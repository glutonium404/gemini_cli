#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include "EnvConfig.hpp"
#include "Utils.hpp"
#include "httplib.h"
#include "json.hpp"

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

    GeminiResponse get_streamed_res(const std::string& request_body) {
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

        // expected example SSE (server sent event)
        /*
            event: step.delta
            data: {"index":1,"delta":{"text":" to assist [...] without markdown","type":"text"},"event_type":"step.delta"}

            event: step.delta
            data: {"index":1,"delta":{"text":" symbols \nlike [...] or scripts","type":"text"},"event_type":"step.delta"}
        */
        while(result.next()) {
            std::string_view res_data{ result.data(), result.size() };

            std::string_view event_prefix = "event: ";
            auto event_end = res_data.find_first_of("\r\n");
            auto event = res_data.substr(event_prefix.size(), event_end - event_prefix.size());

            // std::cout << "======= RAW ========\n";
            // std::cout.write(result.data(), result.size());

            // std::cout << "\n======= Parsed ========\n";
            // std::cout << "[EVNT] : " << event << std::endl;
            // std::cout << "[DATA]  : " << data << std::endl;
            // std::cout << "\n\n";

            if(event != "step.delta") {
                continue;
            }

            std::string_view data_prefix = "data: ";
            auto data_begin = res_data.find(data_prefix);
            auto data_end = res_data.find("\n\n");
            auto data = res_data.substr(data_begin + data_prefix.size(), data_end - data_prefix.size());


            try {
                auto parsed_data = nlohmann::json::parse(data);

                if(!parsed_data.contains("delta") || !parsed_data.at("delta").is_object()) {
                    Utils::log_error("data does not contain property 'delta' or 'delta' is not of type 'object'");
                    continue;
                }

                auto delta = parsed_data.at("delta");

                if(delta.value("type", "") != "text" || !delta.contains("text") || !delta.at("text").is_string()) {
                    Utils::log_error("delat.type is expected to be 'text' and dela.text to be of type 'string'");
                    continue;
                }

                std::cout << delta.at("text").get<std::string>();
            } catch(const std::exception& e) {
                Utils::log_error("Unable to parse chunk\n[CHUNK]: ");
                Utils::log_error(std::string_view(result.data(), result.size()));
                Utils::log_error(e.what());
            }
        }
        std::cout << std::endl;

        response.success = true;
        return response;
    }
};
