#include <subscriber.hpp>

#include <asio/io_context.hpp>
#include <asio/read_until.hpp>
#include <asio/streambuf.hpp>
#include <asio/write.hpp>

#include <chrono>
#include <spdlog/common.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <thread>

//----------------------------------------------------------------------//
Subscriber::Subscriber(const std::string &host, uint16_t port,
                       const std::string &name)
    : m_host{host}, m_port{port}, m_name{name}, m_running{false} {
  m_endpoint = asio::ip::tcp::endpoint(asio::ip::make_address(host), port);
  p_logger = spdlog::basic_logger_mt(
      m_name, fmt::format("logs/subscriber.log", m_name), false);
  p_logger->set_level(spdlog::level::info);
  p_logger->flush_on(spdlog::level::info);
  p_logger->info("[Subsciber::Subscriber] Subscriber created with name: {}.",
                 m_name);
}

//----------------------------------------------------------------------//
Subscriber::Subscriber(const Subscriber &other)
    : m_host{other.m_host}, m_port{other.m_port}, m_name{other.m_name},
      p_logger(std::move(other.p_logger)), m_endpoint(other.m_endpoint) {}

//----------------------------------------------------------------------//
bool Subscriber::start() {
  std::atomic<bool> resultReady = false;

  p_socket = std::make_unique<asio::ip::tcp::socket>(m_ioService);
  p_socket->async_connect(m_endpoint, [this, &resultReady](
                                          const std::error_code &ec) {
    if (!ec) {
      p_logger->info(
          "[Subsciber::Start] Connected to endpoint, subscriber started.");
      m_running = true;

      listenForMessages();
    } else {
      p_logger->error("[Subsciber::Start] Error connecting to endpoint, {}.",
                      ec.message());
    }

    resultReady = true;
  });

  p_ioFuture = std::make_unique<std::future<void>>(
      std::async(std::launch::async, [&]() { m_ioService.run(); }));
  while (!resultReady) {
    std::this_thread::sleep_for(std::chrono::microseconds(200));
  }

  return m_running;
}

//----------------------------------------------------------------------//
bool Subscriber::stop() {
  if (!m_running)
    return true;

  try {
    m_running = false;

    p_socket->cancel();
    p_socket->close();

    m_ioService.stop();
    m_ioService.reset();

    if (p_ioFuture->valid())
      p_ioFuture->wait();

    p_logger->info(
        "[Subsciber::Stop] Disconnected from endpoint, subscriber stopped.");
  } catch (std::exception e) {
    p_logger->error("[Subsciber::Stop] Error disconnecting from endpoint, {}.",
                    e.what());
  }

  return !m_running;
}

//----------------------------------------------------------------------//
void Subscriber::subscribe(const std::string &topic) {
  if (!m_running) {
    p_logger->error("[Subsciber::Subscibe] Error! Cannot subscribe,"
                    "subscriber not started/connected!");
    return;
  }

  auto sendBuffer = std::make_shared<std::string>(topic);
  asio::async_write(
      *p_socket, asio::buffer(*sendBuffer),
      [&, sendBuffer](const std::error_code &ec,
                      std::size_t bytes_transferred) {
        if (!ec) {
          p_logger->info("[Subsciber::Subscribe] Sending "
                         "subscribe message to proxy, topic: {}.",
                         topic);
        } else {
          p_logger->error(
              "[Subsciber::Subscribe] Error subscribing to topic, {}",
              ec.message());
        }
      });
}

//----------------------------------------------------------------------//
void Subscriber::listenForMessages() {
  if (!m_running)
    return;

  auto receiveBuffer = std::make_shared<asio::streambuf>();
  auto buffer = asio::buffer(receiveBuffer->prepare(1024));

  p_socket->async_read_some(
      buffer, [this, receiveBuffer](const asio::error_code &ec,
                                    std::size_t bytes_transferred) {
        if (!ec) {
          receiveBuffer->commit(bytes_transferred);

          std::istream is(receiveBuffer.get());
          std::string receivedMessage(std::istreambuf_iterator<char>{is}, {});

          if (!receivedMessage.empty())
            p_logger->info("[Subsciber::ListenForMessages] Received: {}",
                           receivedMessage);

          receiveBuffer->consume(bytes_transferred);

          listenForMessages();
        } else {
          p_logger->error(
              "[Subsciber::ListenForMessages] Error receiving message: {}.",
              ec.message());
        }
      });
}
