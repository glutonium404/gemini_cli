#pragma once

#include <string>

struct Payload {
    std::string model;
    std::string input;
    std::string system_instruction;
    bool stream = false;
    bool store = true;
};
