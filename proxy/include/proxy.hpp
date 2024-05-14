#pragma once

#include <future>
#include <zmqpp/zmqpp.hpp>

class Proxy {
public:
  //----------------------------------------------------------------------//
  Proxy(const std::string &subscriberEndpoint,
        const std::string &publisherEndpoint)
      : m_subscriberEndpoint{subscriberEndpoint},
        m_publisherEndpoint{publisherEndpoint}, m_running{false} {}
  //----------------------------------------------------------------------//
  void start();
  //----------------------------------------------------------------------//
  void stop();

private:
  //----------------------------------------------------------------------//
  void run();

  //----------------------------------------------------------------------//
  void worker(zmqpp::context &, std::string);
  void subscribe(const std::string &, const std::string &);
  bool unsubscribe(const std::string &, const std::string &);
  void unsubscribe(const std::string &, zmqpp::socket &);
  void forwardToWorker(zmqpp::socket &, zmqpp::socket &);
  void forwardToSubscribers(zmqpp::socket &, zmqpp::socket &);
  void handleSubscriptions(zmqpp::socket &, zmqpp::socket &,
                           std::vector<std::future<void>> &, zmqpp::context &);

  //----------------------------------------------------------------------//
  std::string m_subscriberEndpoint;
  std::string m_publisherEndpoint;
  zmqpp::context m_context;
  std::unordered_map<std::string, std::unique_ptr<zmqpp::socket>> m_subscribers;
  std::unique_ptr<zmqpp::socket> p_publisher;

  //----------------------------------------------------------------------//
  std::atomic<bool> m_running;
  std::unique_ptr<std::future<void>> p_runner;

  std::unordered_map<std::string, std::vector<std::string>> subscriptions_;
  const std::string backend_{"tcp://*:8080"};
  const std::string b_endpoint_{"tcp://localhost:8080"};
  const std::string frontend_{"tcp://*:8081"};
  const std::string inproc_out_{"inproc://workers_out"};
  const std::string inproc_in_{"inproc://workers_in"};
};
