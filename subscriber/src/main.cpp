#include "subscriber.hpp"
#include <iostream>
#include <future>

std::vector<std::string> extractTopics(std::string);

int main(int argc, char* argv[]) {
  if (argc < 2)
    throw std::invalid_argument(
        "Please provide a valid number of arguments! "
        "[ex. ./subscriber topic1,topic2 topic2,topic4,topic7 ...]");

  std::vector<Subscriber> subscribers;
  std::vector<std::future<void>> futs;
  for (int i = 1; i < argc; ++i)
    subscribers.push_back(Subscriber("Subscriber" + std::to_string(i),
                                     extractTopics(std::string(argv[i]))));

  for (auto& sub : subscribers)
    futs.push_back(
        std::async(std::launch::async, &Subscriber::subscribe, &sub));

  return 0;
}

std::vector<std::string> extractTopics(std::string rawTopics) {
  std::string delimiter = ",";

  size_t pos = 0;
  std::vector<std::string> topics;
  while ((pos = rawTopics.find(delimiter)) != std::string::npos) {
    topics.push_back(rawTopics.substr(0, pos));
    rawTopics.erase(0, pos + delimiter.length());
  }
  topics.push_back(rawTopics);

  return topics;
}
