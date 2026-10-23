#pragma once

#include <string_view>

namespace Instructions {
    inline constexpr std::string_view short_res = "keep the ans short and concise withing 15 to 20 lines if possible, write more if needed.";

    inline constexpr std::string_view plain_format = "i am on terminal which can not render markdown. so i want you to write the response in plain text. and format or beautify using plain text. you are allowed to use nerd fonts to beautify";

    inline constexpr std::string_view ansii =
        "Use ANSI SGR terminal colors when they improve readability. "
        "Emit the actual ESC control character before every sequence, such as ESC[32m, "
        "and do not write the literal words ESC, backslash-e, backslash-zero-three-three, "
        "or bracket-only sequences such as [32m. "
        "Reset the formatting with ESC[0m after each colored section. "
        "Use only basic color and style SGR codes; do not emit cursor movement, "
        "OSC, hyperlinks, or other terminal control sequences.";
}
