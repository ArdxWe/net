//
// Created by ardxwe on 3/11/21.
//

#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include <fstream>
#include <string>

class Client {
  public:
  Client(std::string ip, uint16_t port);
  Client(Client &) = delete;
  Client &operator=(Client &) = delete;
  ~Client();

  void connect_server();
  void start();

  private:
  bool get_full_package(std::stringstream &stream);
  void send(std::string &str) const;

  std::string ip_;
  uint16_t port_;
  int sock_fd_ = 0;
  size_t res_index_ = 0;
  std::ofstream file_;
};


#endif//NET_CLIENT_H
