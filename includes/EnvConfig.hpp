#pragma once

#include "Utils.hpp"
#include <cstdlib>
#include <string>
#include <vector>

class EnvConfig {
public:
    EnvConfig(const std::string& custom_model) {
        get_api_key();
        get_def_models();

        Utils::throw_if(
            _def_models.empty() && custom_model.empty(),
            "Unable to find model id. Either set \"" + std::string(ENV_VAR_DEF_MODELS) + "\" environment variable or pass in --model \"<id>\""
        );
    }

    const std::vector<std::string>& def_models() const {
        return _def_models;
    }

    const std::string& api_key() const {
        return _api_key;
    }

private:
    inline static constexpr char ENV_VAR_API_KEY[]    = "GEMINI_API_KEY";
    inline static constexpr char ENV_VAR_DEF_MODELS[] = "GEMINI_CLI_MODELS";
    // extract individual ids delimited by ":"
    inline static constexpr char DELIMITER = ':';

    std::string _api_key;
    std::vector<std::string> _def_models;

    void get_api_key() {
        const char* key = std::getenv(ENV_VAR_API_KEY);

        Utils::throw_if(
            !key || *key == '\0',
            std::string(ENV_VAR_API_KEY) + " environment variable is not set."
        );

        _api_key = key;
    }

    void get_def_models() {
        // get the env variable and make sure it's not nullptr and not empty
        const char* def_models_env = std::getenv(ENV_VAR_DEF_MODELS);

        if(!def_models_env || *def_models_env == '\0') {
            return;
        }

        std::string def_models_str = def_models_env;

        size_t start = 0;
        size_t end = def_models_str.find(DELIMITER);

        _def_models.reserve(3);

        while(end != std::string::npos) {
            auto id = def_models_str.substr(start, end - start);
            if(!id.empty())
                _def_models.push_back(id);

            start = end+1;
            end = def_models_str.find(DELIMITER, start);
        }

        auto id = def_models_str.substr(start);

        if(!id.empty())
            _def_models.push_back(id);

    }
};
