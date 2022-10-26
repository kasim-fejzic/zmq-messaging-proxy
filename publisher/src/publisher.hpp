#pragma once

#include <zmqpp/zmqpp.hpp>
#include <chrono>

class Publisher {
  public:
  Publisher(int, const std::string&, const std::string&);
  void start();

  private:
  const int interval_;
  const std::string prefix_;
  const std::string suffix_;
  unsigned long counter_;
  const std::string endpoint_{"tcp://localhost:8080"};
};
