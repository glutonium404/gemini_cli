#include <cstdlib>
#include <stdexcept>
#include "ANSI.hpp"
#include "EnvConfig.hpp"
#include "FlagHandler.hpp"
#include "AppState.hpp"
#include "HTTPHandler.hpp"
#include "PromptBuilder.hpp"
#include "JSONHandler.hpp"
#include "PayLoad.hpp"
#include "Utils.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    try {
        vector<std::string_view> args;
        args.reserve(argc);

        for(int i=1; i<argc; i++) {
            args.push_back(argv[i]);
        }

        FlagHandler flag_handler;
        Flags flags = flag_handler.parse_flags(args);

        if((!IS_PIPED_DATA(stdin) && args.empty()) || flags.help) {
            Utils::print_help();
            return 0;
        }

        EnvConfig env_config{ flags.model };
        AppState  app_state{ env_config, flags };

        PromptBuilder prompt_builder;
        JSONHandler   json_handler;
        HTTPHandler   http_handler{ env_config, flags.timeout };

        prompt_builder.build_prompt(args, flags, app_state.prompt, app_state.sys_instruction);

        for(const auto& model : app_state.models()) {
            Payload payload = {
                .model              = model,
                .input              = app_state.prompt,
                .system_instruction = app_state.sys_instruction,
                .stream             = flags.stream,
                .store              = false,
            };

            if(flags.verbose) {
                Utils::log_info("Trying : " + payload.model);
            }

            string request_body = json_handler.generate_req_body(payload);
            GeminiResponse response;

            if(payload.stream) {
                response = http_handler.get_streamed_res(request_body);
            }else {
                response = http_handler.get_res(request_body);
            }

            if(!response.success) {
                if(!flags.quiet) {
                    Utils::log_error(response.err);
                }

                continue;
            }

            if(flags.stream) {
                break;
            }

            string parsed_response = json_handler.parse_response_body(response.body);

            if(flags.ansi) {
                ANSI ansi;
                parsed_response = ansi.parse_to_ansi(parsed_response);
            }

            cout << parsed_response << endl;

            return EXIT_SUCCESS;
        }

        throw runtime_error("All network attempts were unsuccessful");

    } catch(const std::exception& e) {
        Utils::log_error(e.what());

        return EXIT_FAILURE;
    }
}
