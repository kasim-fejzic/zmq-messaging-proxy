#include "publisher.hpp"

Publisher::Publisher(int interval, const std::string& prefix,
                     const std::string& suffix)
    : interval_{interval}, prefix_{prefix}, suffix_{suffix}, counter_{0} {}

void Publisher::start() {
    zmqpp::context context;
    zmqpp::socket socket{context, zmqpp::socket_type::publish};
    socket.connect(endpoint_);

    while (true) {
        unsigned long timestamp =
            std::chrono::system_clock::now().time_since_epoch() /
            std::chrono::milliseconds(1);

        zmqpp::message message;
        message.add(prefix_);
        message.add("_" + std::to_string(timestamp) + "_");
        message.add(suffix_);
        message.add(std::to_string(counter_));

        socket.send(message);
        ++counter_;
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_));
    }
}
