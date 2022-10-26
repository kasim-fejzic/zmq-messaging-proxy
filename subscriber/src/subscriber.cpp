#include "subscriber.hpp"

Subscriber::Subscriber(const std::string& name,
                       const std::vector<std::string>& topics)
    : sleepTime_{200}, name_{name}, topics_{topics} {}

void Subscriber::subscribe() {
  int sock = 0;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) return;

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port_);

  if (inet_pton(AF_INET, address_, &serv_addr.sin_addr) <= 0) return;
  if (::connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    return;

  for (auto topic : topics_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime_));
    send(sock, topic.c_str(), topic.length(), 0);
  }

  std::ofstream file;
  auto listenForMessages = [&]() {
    while (true) {
      char buffer[1024] = {0};
      read(sock, buffer, 1024);
      file.open(name_ + ".log", std::ios::app);
      printf("%s\n", buffer);
      file << buffer << std::endl;
      file.close();
    }
  };

  listenForMessages();
}
