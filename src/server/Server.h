//
// Created by ardxwe on 3/11/21.
//

#ifndef NET_SERVER_H
#define NET_SERVER_H

#include <cstdint>
#include <string>

class Server {
  public:
  explicit Server(uint32_t port);
  void start();
  void send(std::stringstream &stream) const;
  bool get_full_package(std::stringstream &stream);

  private:
  [[nodiscard]] static std::stringstream hello(const struct sockaddr_in &client);
  void loop();

  uint32_t port_;
  int connect_fd_ = 0;
  size_t index_ = 0;
};


#endif//NET_SERVER_H
