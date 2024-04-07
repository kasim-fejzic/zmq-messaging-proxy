#include <subscriber.hpp>
#include <thread>

std::vector<std::string> extractTopics(std::string);

int main(int argc, char *argv[]) {
  if (argc < 2)
    throw std::invalid_argument("Please provide a valid number of arguments! "
                                "[ex. ./subscriber topic1,topic2,topic3...]");

  auto subscriber = Subscriber("127.0.0.1", 8081, std::string("Subscriber"));
  subscriber.start();

  std::vector<std::string> topics = extractTopics(argv[1]);
  for (const auto &topic : topics)
    subscriber.subscribe(topic);

  while (1) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

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
