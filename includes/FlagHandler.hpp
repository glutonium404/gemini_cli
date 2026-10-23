#pragma once

#include <algorithm>
#include <charconv>
#include <iostream>
#include <string_view>
#include <vector>

struct Flags {
    /** @brief Allow long response */
    bool long_res = false;
    /** @brief Allow markdown formatted response */
    bool markdown = false;
    /** @brief Allow ansii escape code formatting */
    bool ansii    = false;
    /** @brief Help flag */
    bool help     = false;
    /** @brief Print additional information while prompting */
    bool verbose  = false;
    /** @brief Network request timeout per model in seconds. `5s <= timeout <= 60s` */
    int timeout   = 8;

};

class FlagHandler {
public:
    Flags parse_flags(std::vector<std::string_view>& args) {
        Flags flags;

        if(args.empty()) return flags;

        for(auto it = args.begin(); it != args.end();) {
            if(!flags.long_res && check_flag(*it, { "-l", "--long" })) {
                flags.long_res = true;
                it = args.erase(it);
                continue;
            }

            if(!flags.markdown && check_flag(*it, { "-m", "--markdown" })) {
                flags.markdown = true;
                it = args.erase(it);
                continue;
            }

            if(!flags.ansii && check_flag((*it), { "-a", "--ansii" })) {
                flags.ansii = true;
                it = args.erase(it);
                continue;
            }

            if(check_flag((*it), { "-h", "--help" })) {
                flags.help = true;
                it = args.erase(it);
                continue;
            }

            if(check_flag((*it), { "-v", "--verbose" })) {
                flags.verbose = true;
                it = args.erase(it);
                continue;
            }

            if(check_flag((*it), { "-t", "--timeout" })) {
                std::string_view val = get_flag_value(it, args, "-t|--timeout");
                int t = 0;

                auto [ptr, ec] = std::from_chars(val.data(), val.data() + val.size(), t);

                if(ec != std::errc() || ptr != (val.data() + val.size()) || t < 5 || t > 60) {
                    std::cerr << "Invalid value: -t|--timeout can only accept integers within (5, 60) inclusive" << std::endl;
                    std::exit(1);
                }

                flags.timeout = t;

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
