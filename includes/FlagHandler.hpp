#pragma once

#include "Utils.hpp"
#include "Flags.hpp"
#include <algorithm>
#include <charconv>
#include <initializer_list>
#include <string_view>
#include <vector>
#include <array>

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
            BoolFlagMapping{{"-q", "--quiet"},     flags.quiet},
            BoolFlagMapping{{"--stream"},          flags.stream},
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


                // convert the flag value to an int and store in 't'
                auto [ptr, ec] = std::from_chars(val.data(), val.data() + val.size(), t);

                Utils::throw_if(
                    ec != std::errc() || ptr != (val.data() + val.size()) || t < 5 || t > 60,
                    "Invalid value: -t|--timeout can only accept integers within (5, 60) inclusive"
                );

                flags.timeout = t;
                timeout_processed = true;

                it = args.erase(it); // remove flag
                it = args.erase(it); // remove value
                continue;
            }

            if(flags.model.empty() && check_flag(*it, {"--model"})) {
                flags.model = get_flag_value(it, args, {"--model"});

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
        Utils::throw_if(
            it + 1 == args.end(),
            "[" + std::string(flag_name) + "]" + " flag was used but no value was provided.\nUse [-h|--help]"
        );

        return *(it+1);
    }
};
