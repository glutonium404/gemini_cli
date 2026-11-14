#pragma once

#include <string>

struct Flags {
    /** @brief Allow long response */
    bool long_res  = false;
    /** @brief Allow markdown formatted response */
    bool markdown  = false;
    /** @brief Allow ansi escape code formatting */
    bool ansi      = false;
    /** @brief Help flag */
    bool help      = false;
    /** @brief Print additional information while prompting */
    bool verbose   = false;
    /** @brief Suppress intermediate errors. Eg: while trying multiple models */
    bool quiet     = false;
    /** @brief Allow nerd icon when responding */
    bool nerd_icon = false;
    /** @brief Stream the output */
    bool stream    = false;
    /** @brief Network request timeout per model in seconds. `5s <= timeout <= 60s` */
    int timeout    = 8;
    /** @brief Network Custom model idk */
    std::string model;
};
