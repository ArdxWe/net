//
// Created by ardxwe on 3/11/21.
//

#include "Client.h"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

using std::runtime_error;
using std::string;
using std::stringstream;
using std::vector;
using namespace std::string_literals;
using std::memset;
using std::move;
using sockaddr_in_t = struct sockaddr_in;
using std::cin;
using std::cout;
using std::endl;
using std::to_string;

Client::Client(std::string ip, uint16_t port) : ip_{move(ip)}, port_{port}, file_{"log.txt", std::ios_base::app} {
}

Client::~Client() {
  file_.close();
}

void Client::connect_server() {
  sockaddr_in_t server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port_);

  if (inet_pton(AF_INET, ip_.c_str(), &server_addr.sin_addr) <= 0) {
    throw runtime_error{"inet_pton call error: "s + ip_};
  }

  if ((sock_fd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    throw runtime_error{"socket error."s};
  }

  if (connect(sock_fd_, (const sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
    throw runtime_error{"connect error."s};
  }
}

bool Client::get_full_package(std::stringstream &stream) {
  string res = stream.str();
  for (size_t i = 0; i < res.length(); i++) {
    if (res[i] == '\0') {
      int len = std::stoi(string(&res[0]));
      res_index_ = i + 1;
      return len + i + 1 == res.size();
    }
  }
  return false;
}

void Client::start() {
  size_t n;
  vector<char> receive(0x100);

  string cmd;
  stringstream stream;
  while ((n = read(sock_fd_, receive.data(), receive.size())) > 0) {
    stream.write(receive.data(), n);
    if (get_full_package(stream)) {
      string res = string{stream.str(), res_index_};
      cout << res;
      if (file_.is_open()) {
        file_ << res;
      }
      getline(cin, cmd);
      if (cmd == "q"s || cmd == "Q"s) {
        break;
      }
      send(cmd);
      stream.str("");
    }
  }

  close(sock_fd_);
}

void Client::send(std::string &str) const {
  string head = to_string(str.size()) + '\0';

  write(sock_fd_, head.data(), head.size());
  write(sock_fd_, str.c_str(), str.size());
}
