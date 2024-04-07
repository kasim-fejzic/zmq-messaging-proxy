#include <publisher.hpp>
#include <string>

//----------------------------------------------------------------------//
struct PublisherData {
  std::string prefix;
  std::string suffix;
  unsigned int interval;
};

//----------------------------------------------------------------------//
PublisherData extractArgs(std::string);

//----------------------------------------------------------------------//
int main(int argc, char *argv[]) {
  if (argc < 2)
    throw std::invalid_argument("Please provide a valid number of arguments! "
                                "[ex. ./publisher prefix,suffix,interval");

  PublisherData data = extractArgs(std::string(argv[1]));
  auto publisher =
      Publisher("localhost", 8080, data.prefix, data.suffix, data.interval);
  publisher.start();

  while (1) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  return 0;
}

//----------------------------------------------------------------------//
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
