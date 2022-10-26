#include "publisher.hpp"
#include <future>
#include <string>

struct PublisherData {
    std::string prefix;
    std::string suffix;
    unsigned int interval;
};

PublisherData extractArgs(std::string);

int main(int argc, char* argv[]) {
    if (argc < 2)
        throw std::invalid_argument(
            "Please provide a valid number of arguments! "
            "[ex. ./publisher prefix,suffix,interval prefix,suffix,interval "
            "...]");

    std::vector<Publisher> publishers;
    std::vector<std::future<void>> futs;
    for (int i = 1; i < argc; ++i) {
        PublisherData data = extractArgs(std::string(argv[i]));
        publishers.push_back(
            Publisher(data.interval, data.prefix, data.suffix));
    }

    for (auto& pub : publishers)
        futs.push_back(std::async(std::launch::async, &Publisher::start, &pub));

    return 0;
}

PublisherData extractArgs(std::string rawData) {
    std::string delimiter = ",";

    size_t pos = 0;
    PublisherData data;

    pos = rawData.find(delimiter);
    data.prefix = rawData.substr(0, pos);
    rawData.erase(0, pos + delimiter.length());

    pos = rawData.find(delimiter);
    data.suffix = rawData.substr(0, pos);
    rawData.erase(0, pos + delimiter.length());

    data.interval = atoi(rawData.c_str());

    return data;
}
