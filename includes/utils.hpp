#pragma once

#include <cstring>
#include <string_view>

namespace Utils {
constexpr std::string_view short_res = "USER INSTRUCTION:\nkeep the ans short and concise withing 15 to 20 lines if possible, if needed write more. just give me the reponse directly and do not add any extra text\n\n";

constexpr std::string_view plain_format = "USER INSTRUCTION:\ni am on terminal which can not render markdown. so i want you to write the response in plain text. and format or beautify using plain text.\n\n";

inline bool check_is_long(char* arg) { return std::strcmp(arg, "-l") == 0 || strcmp(arg, "--long") == 0; }
inline bool check_is_markdown(char* arg) { return strcmp(arg, "-m") == 0 || strcmp(arg, "--markdown") == 0; }
}
