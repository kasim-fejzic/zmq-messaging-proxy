#include "proxy.hpp"

int main() {
  Proxy proxy;
  auto j = std::async(std::launch::async, &Proxy::start, &proxy);
  return 0;
}
