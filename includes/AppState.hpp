#pragma once

#include "EnvConfig.hpp"
#include "Flags.hpp"
#include <vector>
#include <string>

struct AppState {
public:
    std::string prompt;
    std::string sys_instruction;

public:
    AppState(const EnvConfig& env_config, const Flags& flags) {
        init_models(env_config, flags);
    }

    const std::vector<std::string>& models() const {
        return _models;
    }

private:
    std::vector<std::string> _models;

private:
    void init_models(const EnvConfig& env_config, const Flags& flags) {
        if(flags.model.empty()) {
            _models = env_config.def_models();
        }else {
            _models = { flags.model };
        }
    }
};
