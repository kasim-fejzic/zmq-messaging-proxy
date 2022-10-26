#pragma once

#include <string>
#include <chrono>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <fstream>
#include <iostream>

class Subscriber {
  public:
  Subscriber(const std::string&, const std::vector<std::string>&);
  void subscribe();

  private:
  std::vector<std::string> topics_;
  std::string name_;
  const char* address_{"127.0.0.1"};
  const int port_{8081};
  const int sleepTime_;
};
