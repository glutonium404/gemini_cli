#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <utility>

struct SseEvent {
    std::string event;
    std::string data;
};

class SseHandler {
public:
    using EventHandler = std::function<void(const SseEvent&)>;

    explicit SseHandler(EventHandler handler)
    : _handler(std::move(handler)) {}

    bool feed(std::string_view chunk) {
        _buffer.append(chunk);

        size_t event_end;

        std::string_view delimiter = _buffer.find("\n\n") != std::string::npos
            ? "\n\n"
            : "\r\n\r\n";

        while((event_end = _buffer.find(delimiter)) != std::string::npos) {
            SseEvent event;
            parse_event(_buffer.substr(0, event_end), event);
            _handler(event);
            _buffer.erase(0, event_end + delimiter.size());

            if(event.event == "interaction.completed") {
                _completed = true;
                return true;
            }
        }

        return false;
    }

    bool completed() const {
        return _completed;
    }

private:
    EventHandler _handler;
    std::string _buffer;
    bool _completed = false;

    void parse_event(std::string_view raw_event, SseEvent& event) {
        event.event.clear();
        event.data.clear();

        std::string_view line;
        size_t start = 0;

        while(start < raw_event.size()) {
            size_t end = raw_event.find("\n", start);

            if(end == std::string::npos) {
                end = raw_event.size();
            }

            line = raw_event.substr(start, end - start);

            if(!line.empty() && line.back() == '\r') {
                line.remove_suffix(1);
            }

            if(line.starts_with("event:")) {
                event.event = line.substr(6); // "event:" -> 6 char
                if(!event.event.empty() && event.event.front() == ' ') {
                    event.event.erase(0, 1);
                }
            } else if(line.starts_with("data:")) {
                auto data = line.substr(5);
                if(!data.empty() && data.front() == ' ') {
                    data.remove_prefix(1);
                }

                if(!event.data.empty()) {
                    event.data += '\n';
                }
                event.data += data;
            }

            start = end + 1;
        }
    }
};
