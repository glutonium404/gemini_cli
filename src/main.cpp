#include <iostream>
#include "ANSII.hpp"
#include "FlagHandler.hpp"
#include "HTTPHandler.hpp"
#include "InputHandler.hpp"
#include "JSONHandler.hpp"

typedef InputHandler::ParsedInput ParsedInput;

using namespace std;

inline void print_help();
inline string test_string();

int main(int argc, char* argv[]) {

    // ANSII ansi;
    // auto str = test_string();
    // cout << ansi.parse_to_ansii(str) << endl;
    // return 0;

    vector<std::string_view> args;
    args.reserve(argc);

    for(int i=1; i<argc; i++) {
        args.push_back(argv[i]);
    }

    FlagHandler  flag_handler;
    Flags flags = flag_handler.parse_flags(args);

    if(flags.help) {
        print_help();
        return 0;
    }

    InputHandler input_handler;
    JSONHandler  json_handler;
    HTTPHandler  http_handler;

    ParsedInput parsed_input  = input_handler.parse_input(args, flags);
    string request_body       = json_handler.generate_request_body(parsed_input);
    optional<string> response = http_handler.send_post_request(request_body, flags);

    if(!response.has_value()) {
        cerr << "Failed to process request." << endl;
        return 1;
    }

    string parsed_response = json_handler.parse_response_body(response.value());

    if(flags.ansii) {
        ANSII ansii;
        parsed_response = ansii.parse_to_ansii(parsed_response);
    }

    cout << parsed_response << endl;

    return 0;
}

inline void print_help() {
    cout
        << ">>> GEMINI CLI <<<\n\n"
        << "gemini_cli - terminal client for Gemini API\n\n"
        << "Usage:\n"
        << "  gemini_cli [options] <prompt>\n"
        << "  echo \"context\" | gemini_cli [options] <prompt>\n\n"
        << "  e.g: cat file.txt | gemini_cli \"summarise this for me\"\n\n"
        << "Options:\n"
        << "  -h, --help        Show this help message and exit\n"
        << "  -l, --long        Allow longer responses (disable short response instruction)\n"
        << "  -m, --markdown    Enable markdown output (disable plain-text instruction)\n"
        << "  -a, --ansii       Enable ansii escape code for color and formatting on terminal\n"
        << "  -v, --verbose     Prints model names as it tries one after another\n"
        << "  -t, --timeout     Network request timeout when requesting each models. (5 <= timeout <= 60)\n"
        << "  --model           Provide custom model id (e.g: gemini-3-flash-preview)\n"
        << "  --                Instructs the application to treat any following argument as plain argument.\n"
        << "                    Any flag that follows this will also be treated as a normal argument\n\n"
        << "Notes:\n"
        << "  - Set GEMINI_API_KEY in your environment before running.\n"
        << "  - Piped stdin is added as context data to the prompt.\n";
}

inline string test_string() {
    return "<%heading%>CLI Application Test<%/heading%>\n"
    "\n"
    "<%subheading%>1. Basic Formatting<%/subheading%>\n"
    "This is normal text.\n"
    "<%success%>Operation completed successfully.<%/success%>\n"
    "<%error%>An unexpected error occurred.<%/error%>\n"
    "<%warning%>Configuration file not found. Using "
    "defaults.<%/warning%>\n"
    "<%info%>Loading application settings...<%/info%>\n"
    "\n"
    "<%subheading%>2. Secondary Information<%/subheading%>\n"
    "<%muted%>This is a less important piece of information.<%/muted%>\n"
    "<%highlight%>IMPORTANT: Changes will take effect after "
    "restarting.<%/highlight%>\n"
    "\n"
    "<%subheading%>3. Keys and References<%/subheading%>\n"
    "<%key%>DATABASE_HOST<%/key%> = localhost\n"
    "<%key%>DATABASE_PORT<%/key%> = 3306\n"
    "<%key%>MAX_CONNECTIONS<%/key%> = 100\n"
    "\n"
    "Documentation: <%link%>https://example.com/docs<%/link%>\n"
    "\n"
    "<%subheading%>4. Multiple Formatting Regions<%/subheading%>\n"
    "Status: <%success%>CONNECTED<%/success%>\n"
    "Status: <%error%>DISCONNECTED<%/error%>\n"
    "Status: <%warning%>RECONNECTING<%/warning%>\n"
    "\n"
    "<%subheading%>5. Formatting With Normal Text Between "
    "Tags<%/subheading%>\n"
    "The server at <%key%>192.168.0.1<%/key%> responded with "
    "<%success%>HTTP 200 OK<%/success%> in 42 ms.\n"
    "\n"
    "<%subheading%>6. Reset Test<%/subheading%>\n"
    "<%heading%>This heading should be formatted.<%/heading%>\n"
    "<%reset%>This text should use the terminal's default formatting.\n"
    "\n"
    "<%subheading%>7. Longer Output Simulation<%/subheading%>\n"
    "<%info%>Initializing application...<%/info%>\n"
    "<%success%>Configuration loaded.<%/success%>\n"
    "<%info%>Connecting to database...<%/info%>\n"
    "<%success%>Database connection established.<%/success%>\n"
    "<%info%>Starting HTTP server...<%/info%>\n"
    "<%success%>Server listening on port 3000.<%/success%>\n"
    "\n"
    "<%heading%>Application started successfully.<%/heading%>\n\n"
    "<%subheading%>8. Inline Code<%/subheading%>\n"
    "<%code%>int x = 10;<%/code%>\n\n"
    "<%subheading%>9. Block Code<%/subheading%>\n"
    "<%codeblock%>\n"
    "int sum(int a, int b) {\n"
    "    return a + b;\n"
    "}"
    "<%/codeblock%>\n\n"
    "<%muted%>Some other not so important muted/gray info<%/muted%>\n";
}
