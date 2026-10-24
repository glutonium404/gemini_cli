#pragma once

#include <string_view>

namespace Instructions {
    inline constexpr std::string_view short_res = "keep the ans short and concise withing 15 to 20 lines if possible, write more if needed.";

    inline constexpr std::string_view plain_format = "i am on terminal which can not render markdown. so i want you to write the response in plain text. and format or beautify using plain text. you are allowed to use nerd fonts to beautify";

    inline constexpr std::string_view ansii =
        "Use only these markup tags for terminal formatting: "
        "<%reset%>, <%bold%>, <%dim%>, <%italic%>, <%underline%>, <%blink%>, "
        "<%reverse%>, <%hidden%>, <%strike%>, "
        "<%black%>, <%red%>, <%green%>, <%yellow%>, <%blue%>, <%magenta%>, "
        "<%cyan%>, <%white%>, <%gray%>, "
        "<%bg-black%>, <%bg-red%>, <%bg-green%>, <%bg-yellow%>, <%bg-blue%>, "
        "<%bg-magenta%>, <%bg-cyan%>, <%bg-white%>, <%bg-gray%>, "
        "<%bright-black%>, <%bright-red%>, <%bright-green%>, <%bright-yellow%>, "
        "<%bright-blue%>, <%bright-magenta%>, <%bright-cyan%>, <%bright-white%>, "
        "and <%heading%>. "
        "Write tags exactly as shown, for example <%green%>text<%reset%>. "
        "Do not emit ANSI escape sequences, [31m-style sequences, backslash escapes, "
        "cursor movement, OSC, hyperlinks, or any other terminal control codes.";
}
