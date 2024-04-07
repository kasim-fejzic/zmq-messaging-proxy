#include <publisher.hpp>

#include <future>
#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <zmqpp/zmqpp.hpp>

//----------------------------------------------------------------------//
Publisher::Publisher(const std::string &host, uint16_t port,
                     const std::string &prefix, const std::string &suffix,
                     const uint64_t interval)
    : m_endpoint{fmt::format("tcp://{}:{}", host, port)},
      m_socket{zmqpp::socket(m_context, zmqpp::socket_type::publish)},
      m_prefix{prefix}, m_suffix{suffix}, m_interval{interval},
      m_outgoingMessageCount{0}, m_running{false} {
  p_logger = spdlog::basic_logger_mt(fmt::format("{}_{}", prefix, suffix),
                                     fmt::format("logs/publisher.log"), false);
  p_logger->set_level(spdlog::level::info);
  p_logger->flush_on(spdlog::level::info);

  p_logger->info("[Publisher::Publisher] Publisher created with prefix: {}, "
                 "and suffix: {}.",
                 prefix, suffix);
}

//----------------------------------------------------------------------//
bool Publisher::start() {
  p_logger->info("[Publisher::Start] Starting publisher.");

  m_socket.connect(m_endpoint);
  m_running = true;

  p_runner = std::make_unique<std::future<void>>(
      std::async(std::launch::async, &Publisher::publish, this));

  return m_running;
}

//----------------------------------------------------------------------//
bool Publisher::stop() {
  p_logger->info("[Publisher::Stop] Stopping publisher.");

  m_running = false;
  m_socket.disconnect(m_endpoint);
  m_outgoingMessageCount = 0;

  if (p_runner->valid())
    p_runner->wait();

  return !m_running;
}

//----------------------------------------------------------------------//
void Publisher::publish() {
  while (m_running) {
    auto message = generateMessage();
    p_logger->info("[Publisher::Publish] Publishing message: {}",
                   getZMQMessageString(message));
    m_socket.send(message);

    std::this_thread::sleep_for(std::chrono::milliseconds(m_interval));
  }
}

//----------------------------------------------------------------------//
zmqpp::message Publisher::generateMessage() {
  auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::system_clock::now().time_since_epoch())
                       .count();

  zmqpp::message message;
  message.add(m_prefix);
  message.add(fmt::format("_{}_", timestamp));
  message.add(m_suffix);
  message.add(std::to_string(m_outgoingMessageCount++));

  return message;
}

//----------------------------------------------------------------------//
std::string Publisher::getZMQMessageString(const zmqpp::message &message) {
  std::string msg;
  for (size_t i = 0; i < message.parts(); ++i) {
    std::string part;
    message.get(part, i);
    msg += part;
  }

  return msg;
}
