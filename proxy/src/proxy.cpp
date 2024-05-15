#include <future>
#include <memory>
#include <proxy.hpp>
#include <zmqpp/poller.hpp>

//----------------------------------------------------------------------//
void Proxy::start() {
  // zmqpp::context context;
  // zmqpp::socket subscriber(context, zmqpp::socket_type::subscribe);
  // subscriber.bind(backend_);
  //
  // zmqpp::socket publisher(context, zmqpp::socket_type::stream);
  // publisher.bind(frontend_);
  //
  // zmqpp::socket inForwarder(context, zmqpp::socket_type::publish);
  // inForwarder.bind(inproc_in_);
  //
  // zmqpp::socket outForwarder(context, zmqpp::socket_type::subscribe);
  // outForwarder.subscribe("");
  // outForwarder.bind(inproc_out_);
  //
  // std::vector<std::future<void>> workerThreads;
  // zmqpp::poller poller;
  // poller.add(subscriber);
  // poller.add(outForwarder);
  // poller.add(publisher);
  //
  // bool first = true;
  // while (true) {
  //   poller.poll();
  //
  //   if (poller.events(subscriber)) {
  //     forwardToWorker(subscriber, inForwarder);
  //   }
  //   if (poller.events(outForwarder)) {
  //     forwardToSubscribers(outForwarder, publisher);
  //   }
  //   if (poller.events(publisher)) {
  //     handleSubscriptions(publisher, subscriber, workerThreads, context);
  //   }
  // }

  p_publisher =
      std::make_unique<zmqpp::socket>(m_context, zmqpp::socket_type::xpub);
  p_publisher->bind(m_publisherEndpoint);
  m_running = true;
  // LOG CONNECT

  m_runners.emplace_back(std::make_unique<std::future<void>>(std::async(
      std::launch::async, &Proxy::listenForMessagesOnPublisher, this)));
  m_runners.emplace_back(std::make_unique<std::future<void>>(std::async(
      std::launch::async, &Proxy::listenForMessagesOnSubscriber, this)));
}

//----------------------------------------------------------------------//
void Proxy::stop() {}

//----------------------------------------------------------------------//
void Proxy::listenForMessagesOnPublisher() {
  // Log starting listener
  zmqpp::poller poller;
  poller.add(*p_publisher);

  while (m_running) {
    if (poller.poll() && poller.has_input(*p_publisher)) {
      zmqpp::message message;
      p_publisher->receive(message);
      std::string subscription;
      message >> subscription;
      subscribe(subscription); // TODO check to start as async?
      // Log received message
    }
  }
}

//----------------------------------------------------------------------//
void Proxy::listenForMessagesOnSubscriber() {
  // Log starting listener

  while (m_running) {
    if (m_subsribersPoller.poll()) {
      for (const auto &[topic, socket] : m_subscribers) {
        zmqpp::message message;
        socket->receive(message);
        // forwardToSubs
        // Log received message
      }
    }
  }
}

//----------------------------------------------------------------------//
void Proxy::subscribe(const std::string &subscriptionMsg) {
  bool isSubscription = subscriptionMsg[0] == 1;

  std::string topic = subscriptionMsg.substr(1);
  // if (isSubscription && m_subscribers.find(topic) == m_subscribers.end()) {
  //   auto subscriber =
  //       std::make_unique<zmqpp::socket>(m_context, zmqpp::socket_type::xsub);
  //   subscriber->connect(m_subscriberEndpoint);
  //   subscriber->set(zmqpp::socket_option::subscribe, topic);
  //   m_subscribers[topic] = std::move(subscriber);
  //   // Log subscribe
  // } else if (!isSubscription &&
  //            m_subscribers.find(topic) != m_subscribers.end()) {
  //
  //   // Log unsubscribe
  // } else {
  //   // Log invalid request
  // }
}

//----------------------------------------------------------------------//
// void Proxy::worker(zmqpp::context &context, std::string topic) {
//   zmqpp::socket subscriber(context, zmqpp::socket_type::subscribe);
//   subscriber.subscribe(topic);
//   subscriber.connect(inproc_in_);
//
//   zmqpp::socket publisher(context, zmqpp::socket_type::publish);
//   publisher.connect(inproc_out_);
//
//   while (true) {
//     zmqpp::message message;
//     subscriber.receive(message);
//
//     zmqpp::message msg;
//     msg.add(message.get(0));
//     msg.add(message.get(1));
//     msg.add(message.get(2));
//     msg.add(message.get(3));
//     publisher.send(msg);
//   }
// }
//
// //----------------------------------------------------------------------//
// void Proxy::subscribe(const std::string &subscriberId,
//                       const std::string &topic) {
//   auto result = std::find(subscriptions_[topic].begin(),
//                           subscriptions_[topic].end(), subscriberId);
//   if (result != subscriptions_[topic].end()) {
//     unsubscribe(subscriberId, topic);
//     return;
//   }
//
//   subscriptions_[topic].push_back(subscriberId);
//   std::cout << "Proxy received a subscription to topic [" << topic << "]"
//             << std::endl;
// }
//
// //----------------------------------------------------------------------//
// bool Proxy::unsubscribe(const std::string &subscriberId,
//                         const std::string &topic) {
//   auto result = std::find(subscriptions_.at(topic).begin(),
//                           subscriptions_.at(topic).end(), subscriberId);
//   if (result == subscriptions_.at(topic).end())
//     return false;
//
//   std::swap(*result, subscriptions_.at(topic).back());
//   subscriptions_.at(topic).pop_back();
//   std::cout << "Proxy received an unsubscribe from topic [" << topic << "]"
//             << std::endl;
//   return true;
// }
//
// //----------------------------------------------------------------------//
// void Proxy::unsubscribe(const std::string &subscriberId,
//                         zmqpp::socket &subscriber) {
//   for (auto it = subscriptions_.begin(); it != subscriptions_.end(); ++it)
//     if (unsubscribe(subscriberId, it->first) && it->second.empty())
//       subscriber.unsubscribe(it->first);
// }
//
// //----------------------------------------------------------------------//
// void Proxy::forwardToWorker(zmqpp::socket &subscriber,
//                             zmqpp::socket &inForwarder) {
//   zmqpp::message message;
//   subscriber.receive(message);
//
//   std::cout << "Proxy received a message from publisher [" << message.get(0)
//             << "]" << std::endl;
//
//   inForwarder.send(message);
// }
//
// //----------------------------------------------------------------------//
// void Proxy::forwardToSubscribers(zmqpp::socket &outForwarder,
//                                  zmqpp::socket &publisher) {
//   zmqpp::message message;
//   outForwarder.receive(message);
//   std::string topic = message.get(0);
//   if (subscriptions_.find(topic) != subscriptions_.end()) {
//     std::string msgContent = message.get(1);
//     std::string suffix = message.get(2);
//     std::string count = message.get(3);
//     for (const auto &subscriber : subscriptions_[topic]) {
//       zmqpp::message msg;
//       msg.add(subscriber);
//       msg.add(topic + msgContent + suffix + count);
//
//       publisher.send(msg);
//     }
//   }
// }
//
// //----------------------------------------------------------------------//
// void Proxy::handleSubscriptions(zmqpp::socket &publisher,
//                                 zmqpp::socket &subscriber,
//                                 std::vector<std::future<void>>
//                                 &workerThreads, zmqpp::context &context) {
//   zmqpp::message message;
//   publisher.receive(message);
//   std::string subscriberId = message.get(0);
//   std::string topic = message.get(1);
//
//   if (topic.empty()) {
//     unsubscribe(subscriberId, subscriber);
//   } else {
//     if (subscriptions_.find(topic) == subscriptions_.end()) {
//       workerThreads.push_back(std::async(std::launch::async, &Proxy::worker,
//                                          *this, std::ref(context),
//                                          std::ref(topic)));
//     }
//
//     subscribe(subscriberId, topic);
//     if (subscriptions_[topic].size() == 1)
//       subscriber.subscribe(topic);
//   }
// }
