#pragma once

#include <string_view>
#include <string>
#include <unordered_map>

class ANSI {
public:
    std::string parse_to_ansi(const std::string& str) const {
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
        // Full reset
        {"reset",       "\033[0m"},

        // Semantic opening styles
        {"heading",     "\033[1;38;2;25;25;25;48;2;245;245;245m "}, // Black on off-white
        {"subheading",  "\033[1;38;2;100;170;255m"},                // Light blue
        {"success",     "\033[1;38;2;80;220;130m"},                 // Green
        {"error",       "\033[1;38;2;255;90;90m"},                  // Red
        {"warning",     "\033[1;38;2;255;195;70m"},                 // Yellow-orange
        {"info",        "\033[1;38;2;80;200;230m"},                 // Cyan
        {"muted",       "\033[38;2;145;150;160m"},                  // Gray
        {"highlight",   "\033[1;38;2;30;30;30;48;2;255;210;90m"},   // Dark text on gold
        {"key",         "\033[1;38;2;210;140;255m"},                // Purple
        {"link",        "\033[4;38;2;90;190;255m"},                 // Blue, underlined

        // Code formatting
        {"code",       "\033[38;2;255;210;130;48;2;40;44;52m"},
        {"codeblock",  "\033[38;2;220;225;235;48;2;30;33;40m"},

        // Corresponding closing styles
        {"/heading",    " \033[22;39;49m"},
        {"/subheading", "\033[22;39m"},
        {"/success",    "\033[22;39m"},
        {"/error",      "\033[22;39m"},
        {"/warning",    "\033[22;39m"},
        {"/info",       "\033[22;39m"},
        {"/muted",      "\033[22;39m"},
        {"/highlight",  "\033[22;39;49m"},
        {"/key",        "\033[22;39m"},
        {"/link",       "\033[24;39m"},
        {"/code",      "\033[22;39;49m"},
        {"/codeblock", "\033[22;39;49m"}
    };

    inline bool is_opening(const std::string& str, size_t i) const {
        return i + 1 < str.size() && str[i] == '<' && str[i + 1] == '%';
    }
};
