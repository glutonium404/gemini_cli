#pragma once

#include "InputHandler.hpp"
#include "json.hpp"
#include <initializer_list>
#include <iostream>

class JSONHandler {
public:
    /**
     *
     * {
     *  "candidates": [
     *    {
     *      "content": {
     *        "parts": [
     *          {
     *            "text": "Generated response string"
     *          }
     *        ]
     *      }
     *    }
     *  ]
     * }
     */
    std::string parse_response_body(const std::string& res_body) {
        std::string response = "";

        try {
            nlohmann::json response_json = nlohmann::json::parse(res_body);
            response = response_json
                .at("candidates")
                .at(0)
                .at("content")
                .at("parts")
                .at(0)
                .at("text");

        } catch (const std::exception& e) {
            std::cerr << "JSON Parsing error: " << e.what() << std::endl;
        }

        return response;
    }

    /**
    * {
    *   "system_instruction": {
    *     "parts": [
    *       {
    *         "text": instruction
    *       }
    *     ]
    *   },
    *   "contents": [
    *     {
    *       "parts": [
    *         {
    *           "text": prompt
    *         }
    *       ]
    *     }
    *   ]
    * }
    */
    std::string generate_request_body(const InputHandler::ParsedInput& parsed_input) {
        nlohmann::json instruction_parts = nlohmann::json::array();

        for(const std::string& instruction : parsed_input.instructions) {
            instruction_parts.push_back({
                {"text", instruction}
            });
        }

        nlohmann::json payload = {
            {"system_instruction", {
                {"parts", instruction_parts}
            }},
            {"contents", nlohmann::json::array({
                {
                    {"parts", nlohmann::json::array({
                        {
                            {"text", parsed_input.prompt}
                        }
                    })}
                }
            })}
        };

        return payload.dump();
    }
};


