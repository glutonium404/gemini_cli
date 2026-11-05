#pragma once

#include <algorithm>
#include <charconv>
#include <cstdlib>
#include <initializer_list>
#include <iostream>
#include <string_view>
#include <vector>
#include <array>

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
    /** @brief Allow nerd icon when responding */
    bool nerd_icon = false;
    /** @brief Network request timeout per model in seconds. `5s <= timeout <= 60s` */
    int timeout    = 8;
    /** @brief Network Custom model idk */
    std::string_view custom_model_id;
};

class FlagHandler {
private:
    struct BoolFlagMapping {
        std::initializer_list<std::string_view>  tokens;
        bool& target;
    };

public:
    Flags parse_flags(std::vector<std::string_view>& args) {
        Flags flags;

        if(args.empty()) return flags;

        std::array bool_flags{
            BoolFlagMapping{{"-h", "--help"},      flags.help},
            BoolFlagMapping{{"-l", "--long"},      flags.long_res},
            BoolFlagMapping{{"-m", "--markdown"},  flags.markdown},
            BoolFlagMapping{{"-a", "--ansi"},      flags.ansi},
            BoolFlagMapping{{"-v", "--verbose"},   flags.verbose},
            BoolFlagMapping{{"--nerd-icon"},       flags.nerd_icon},
        };

        bool timeout_processed = false;

        for(auto it = args.begin(); it != args.end();) {

            // "--" as a command line argument instructs that anything following it should treated as normal argument
            // any flags following it will also be treated as an argument
            if(*it == "--") {
                it = args.erase(it);
                return flags;
            }

            auto item = std::ranges::find_if(bool_flags, [&](const BoolFlagMapping& bf) {
                return !bf.target && check_flag(*it, bf.tokens);
            });

            if(item != bool_flags.end()) {
                item->target = true;
                it = args.erase(it);
                continue;
            }

            if(!timeout_processed && check_flag((*it), { "-t", "--timeout" })) {
                std::string_view val = get_flag_value(it, args, "-t|--timeout");
                int t = 0;

                auto [ptr, ec] = std::from_chars(val.data(), val.data() + val.size(), t);

                if(ec != std::errc() || ptr != (val.data() + val.size()) || t < 5 || t > 60) {
                    std::cerr << "Invalid value: -t|--timeout can only accept integers within (5, 60) inclusive" << std::endl;
                    std::exit(1);
                }

                flags.timeout = t;
                timeout_processed = true;

                it = args.erase(it); // remove flag
                it = args.erase(it); // remove value
                continue;
            }

            if(flags.custom_model_id.empty() && check_flag(*it, {"--model"})) {
                flags.custom_model_id = get_flag_value(it, args, {"--model"});

                it = args.erase(it); // remove flag
                it = args.erase(it); // remove value
                continue;
            }

            it++;
        }

        return flags;
    }

private:
    inline bool check_flag(std::string_view arg, std::initializer_list<std::string_view> flags) const {
        return std::ranges::any_of(flags, [&](std::string_view f) { return f == arg; });
    }

    inline std::string_view get_flag_value(auto& it, const std::vector<std::string_view>& args, std::string_view flag_name) {
        if(it + 1 == args.end()) {
            std::cerr << "[" << flag_name << "]" << " flag was used but no value was provided.\nUse [-h|--help]" << std::endl;
            std::exit(1);
        }

        return *(it+1);
    }
};
