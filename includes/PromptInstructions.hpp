#pragma once

#include <string_view>

namespace Instructions {
    inline constexpr std::string_view short_res = "keep the ans short and concise withing 15 to 20 lines if possible, write more if needed.";

    inline constexpr std::string_view nerd_icon = "User nerd font glyphs to beautify";

    inline constexpr std::string_view plain_format = "i am on terminal which can not render markdown. so i want you to write the response in plain text. and format or beautify using plain text.";

    inline constexpr std::string_view ansii =
        "Use only these markup tags for terminal formatting: "

        // Opening tags
        "<%reset%>, "
        "<%heading%>, <%subheading%>, "
        "<%success%>, <%error%>, <%warning%>, <%info%>, "
        "<%muted%>, <%highlight%>, <%key%>, and <%link%>. "
        "<%code%>, <%codeblock%>. "

        // Closing tags
        "Every formatting tag except <%reset%> has a corresponding closing tag: "
        "<%/heading%>, <%/subheading%>, "
        "<%/success%>, <%/error%>, <%/warning%>, <%/info%>, "
        "<%/muted%>, <%/highlight%>, <%/key%>, and <%/link%>. "
        "<%/code%>, <%/codeblock%>. "

        // Usage
        "Use <%reset%> to reset all active formatting. "
        "Always close formatting tags using their corresponding closing tags. "
        "Use <%heading%> for section headings and <%subheading%> for subsections. "
        "Use <%success%>, <%error%>, <%warning%>, and <%info%> "
        "for their corresponding messages. "
        "Use <%muted%> for secondary information, "
        "<%highlight%> for particularly important information, "
        "<%key%> for identifiers and important values, "
        "and <%link%> for URLs and references. "
        "and <%code%> for inline code. "
        "and <%codeblock%> for code block. "

        // Example
        "For example: "
        "<%heading%>Database Configuration<%/heading%> "
        "<%success%>Connection established.<%/success%> "
        "<%key%>DATABASE_HOST<%/key%> = localhost. "

        // Formatting rules
        "Use formatting sparingly and consistently. "
        "Prefer semantic tags over plain text decoration. "
        "Do not invent additional tags or use unsupported formatting keywords. "
        "Keep formatting regions properly nested. "
        "Do not assume closing a tag restores previously active formatting. "

        // Restrictions
        "Do not emit ANSI escape sequences, [31m-style sequences, "
        "backslash escapes, cursor movement, OSC, hyperlinks, "
        "or any other terminal control codes."
        "Do not provide random information or information that are not relavant to the given prompt. keep things relavant";
}
