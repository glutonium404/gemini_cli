#pragma once

#include "json.hpp"
#include <iostream>

class JSONHandler {
private:
    JSONHandler() = delete;

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
    static std::string parse_response_body(const std::string res_body) {
        std::string response = "";

        try {
            nlohmann::json response_json = nlohmann::json::parse(res_body);
            response = response_json["candidates"][0]["content"]["parts"][0]["text"];
        } catch (const std::exception& e) {
            std::cerr << "JSON Parsing error: " << e.what() << std::endl;
        }

        return response;
    }

    /**
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
    static std::string generate_request_body(const std::string final_prompt) {
        nlohmann::json payload;

        payload["contents"] = nlohmann::json::array({ // <-------------- array initializer list
            { // <-------------------------------------------- the array itsel
                { // <---------------------------------------- the array item , i.e object
                    "parts", // <----------------------------- key value pair
                    nlohmann::json::array({ // <------------------------ the initializer object
                        { // <-------------------------------- the array itself
                            { "text", final_prompt }
                        }
                    })
                }
            }
        });

        return payload.dump();
    }
};


