#pragma once

#include <zmqpp/zmqpp.hpp>
#include <future>

class Proxy {
  public:
  Proxy() = default;
  void start();

  private:
  std::unordered_map<std::string, std::vector<std::string>> subscriptions_;
  const std::string backend_{"tcp://*:8080"};
  const std::string b_endpoint_{"tcp://localhost:8080"};
  const std::string frontend_{"tcp://*:8081"};
  const std::string inproc_out_{"inproc://workers_out"};
  const std::string inproc_in_{"inproc://workers_in"};

  void worker(zmqpp::context&, std::string);
  void subscribe(const std::string&, const std::string&);
  bool unsubscribe(const std::string&, const std::string&);
  void unsubscribe(const std::string&, zmqpp::socket&);
  void forwardToWorker(zmqpp::socket&, zmqpp::socket&);
  void forwardToSubscribers(zmqpp::socket&, zmqpp::socket&);
  void handleSubscriptions(zmqpp::socket&, zmqpp::socket&,
                           std::vector<std::future<void>>&, zmqpp::context&);
};
