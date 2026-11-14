#pragma once

#include "HTTPHandler.hpp"
#include "Utils.hpp"
#include "json.hpp"
#include <stdexcept>
#include <string>

class JSONHandler {
public:
    /**
     *
     * {
     *     "steps": [
     *         {
     *             "type": "model_output",
     *             "content": [
     *                 {
     *                     "type": "text",
     *                     "text": "text"
     *                 }
     *             ]
     *         }
     *     ]
     * }
     */
    std::string parse_response_body(const std::string& res_body) {
        try {
            const auto response_json = nlohmann::json::parse(res_body);
            const auto& steps = response_json.at("steps");

            Utils::throw_if(!steps.is_array(), "Response field 'steps' is not an array.");

            std::string response = "";

            for(const auto& step : steps) {
                if(step.value("type", "") != "model_output") {
                    continue;
                }

                const auto& content = step.at("content");

                Utils::throw_if(
                    !content.is_array(),
                    "Model_output 'content' is not an array"
                );

                for(const auto& item : content) {
                    if(item.value("type", "") == "text" && item.contains("text") && item.at("text").is_string()) {
                        response += item.at("text").get<std::string>();
                    }
                }
            }

            Utils::throw_if(response.empty(), "No text output found in response");

            return response;

        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error while parsing response body\n") + e.what());
        }
    }

    /**
     *
     * {
     *     "model": "$model",
     *     "input": "$input",
     *     "system_instruction": "$systemins",
     *     "stream": false,
     *     "store": false
     * }
     */
    std::string generate_req_body(const Payload& payload) {
        nlohmann::json json = {
            {"model",              payload.model},
            {"input",              payload.input},
            {"system_instruction", payload.system_instruction},
            {"stream",             payload.stream},
            {"store",              payload.store}
        };

        return json.dump();
    }
};


