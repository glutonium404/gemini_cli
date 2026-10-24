#pragma once

#include <string_view>
#include <string>
#include <unordered_map>

class ANSII {
public:
    std::string parse_to_ansii(const std::string& str) const {
        std::string res;
        size_t i = 0;

        while(i < str.size()) {
            if(!is_opening(str, i)) {
                res.push_back(str[i]);
                i++;
                continue;
            }

            const size_t closing = str.find("%>", i + 2);
            if(closing == std::string::npos) {
                res.append(str, i, std::string::npos);
                break;
            }

            const std::string_view keyword(
                str.data() + i + 2,
                closing - (i + 2)
            );
            const auto it = keywords.find(keyword);

            if(it == keywords.end()) {
                res.append(str, i, closing + 2 - i);
            } else {
                res += it->second;
            }

            i = closing + 2;
        }

        res += keywords.at("reset");
        return res;
    }

private:
    inline static const std::unordered_map<std::string_view, std::string_view> keywords = {
        {"reset",        "\033[0m"},
        {"bold",         "\033[1m"},
        {"dim",          "\033[2m"},
        {"italic",       "\033[3m"},
        {"underline",    "\033[4m"},
        {"blink",        "\033[5m"},
        {"reverse",      "\033[7m"},
        {"hidden",       "\033[8m"},
        {"strike",       "\033[9m"},
        {"black",        "\033[30m"},
        {"red",          "\033[31m"},
        {"green",        "\033[32m"},
        {"yellow",       "\033[33m"},
        {"blue",         "\033[34m"},
        {"magenta",      "\033[35m"},
        {"cyan",         "\033[36m"},
        {"white",        "\033[37m"},
        {"gray",         "\033[90m"},
        {"grey",         "\033[90m"},
        {"bg-black",     "\033[40m"},
        {"bg-red",       "\033[41m"},
        {"bg-green",     "\033[42m"},
        {"bg-yellow",    "\033[43m"},
        {"bg-blue",      "\033[44m"},
        {"bg-magenta",   "\033[45m"},
        {"bg-cyan",      "\033[46m"},
        {"bg-white",     "\033[47m"},
        {"bg-gray",      "\033[100m"},
        {"bg-grey",      "\033[100m"},
        {"bright-black", "\033[90m"},
        {"bright-red",   "\033[91m"},
        {"bright-green", "\033[92m"},
        {"bright-yellow","\033[93m"},
        {"bright-blue",  "\033[94m"},
        {"bright-magenta","\033[95m"},
        {"bright-cyan",  "\033[96m"},
        {"bright-white", "\033[97m"},
        {"heading",      "\033[1;36m"}
    };

    inline bool is_opening(const std::string& str, size_t i) const {
        return i + 1 < str.size() && str[i] == '<' && str[i + 1] == '%';
    }
};
