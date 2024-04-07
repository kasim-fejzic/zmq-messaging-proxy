#pragma once

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <atomic>
#include <future>
#include <spdlog/logger.h>
#include <string>

class Subscriber {
public:
  //----------------------------------------------------------------------//
  Subscriber(const std::string &host, uint16_t port, const std::string &name);
  //----------------------------------------------------------------------//
  Subscriber(const Subscriber &other);
  //----------------------------------------------------------------------//
  bool start();
  //----------------------------------------------------------------------//
  bool stop();
  //----------------------------------------------------------------------//
  void subscribe(const std::string &topic);

private:
  //----------------------------------------------------------------------//
  void listenForMessages();

  //----------------------------------------------------------------------//
  const std::string m_host;
  const uint16_t m_port;
  asio::ip::tcp::endpoint m_endpoint;
  std::unique_ptr<asio::ip::tcp::socket> p_socket;

  //----------------------------------------------------------------------//
  const std::string m_name;
  std::atomic<bool> m_running;
  std::shared_ptr<spdlog::logger> p_logger;
  asio::io_context m_ioService;
  std::unique_ptr<std::future<void>> p_ioFuture;
};
