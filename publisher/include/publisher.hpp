#pragma once

#include <future>
#include <spdlog/logger.h>
#include <string>
#include <zmqpp/context.hpp>
#include <zmqpp/socket.hpp>

class Publisher {
public:
  //----------------------------------------------------------------------//
  Publisher(const std::string &host, uint16_t port, const std::string &prefix,
            const std::string &suffix, const uint64_t interval = 1000);
  //----------------------------------------------------------------------//
  bool start();
  //----------------------------------------------------------------------//
  bool stop();

private:
  //----------------------------------------------------------------------//
  void publish();
  //----------------------------------------------------------------------//
  zmqpp::message generateMessage();
  //----------------------------------------------------------------------//
  std::string getZMQMessageString(const zmqpp::message &message);

  //----------------------------------------------------------------------//
  const std::string m_endpoint;
  zmqpp::context m_context;
  zmqpp::socket m_socket;

  //----------------------------------------------------------------------//
  const std::string m_prefix;
  const std::string m_suffix;
  const uint64_t m_interval;
  uint64_t m_outgoingMessageCount;
  std::atomic<bool> m_running;
  std::unique_ptr<std::future<void>> p_runner;
  std::shared_ptr<spdlog::logger> p_logger;
};
