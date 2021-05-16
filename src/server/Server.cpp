//
// Created by ardxwe on 3/11/21.
//

#include "Server.h"

#include <arpa/inet.h>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include <vector>

using std::endl;
using std::put_time;
using std::runtime_error;
using std::string;
using std::stringstream;
using std::to_string;
using std::unique_ptr;
using std::vector;
using sockaddr_in_t = struct sockaddr_in;
using namespace std::string_literals;
using namespace std::chrono;

Server::Server(uint32_t port) : port_{port} {
}

void Server::start() {
  sockaddr_in_t server;
  int listen_fd;
  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    throw runtime_error{"call socket error."s};
  }

  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(port_);
  bind(listen_fd, (sockaddr *) &server, sizeof(server));

  listen(listen_fd, 5);

  sockaddr_in_t client;
  socklen_t len;
  connect_fd_ = accept(listen_fd, (sockaddr *) &client, &len);

  stringstream stream = hello(client);
  string flag{"sh> "};
  stream << flag;
  send(stream);
  loop();
}

stringstream Server::hello(const sockaddr_in &client) {
  vector<char> addr(0x100);
  inet_ntop(AF_INET, &client.sin_addr, addr.data(), addr.size());

  stringstream stream;
  stream << "address: ";
  stream.write(addr.data(), addr.size());
  stream << endl;

  stream << "port   : " << client.sin_port << endl;

  stream << "Welcome to my machine :)" << endl;
  auto time = std::chrono::system_clock::to_time_t(
    std::chrono::system_clock::now());
  stream << put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << endl;
  return stream;
}

void Server::loop() {

  vector<char> buff(0x100);
  size_t n;
  string flag{"sh> "};
  stringstream stream;

  auto close_file = [](FILE *file) { pclose(file); };
  while ((n = read(connect_fd_, buff.data(), buff.size())) > 0) {
    stream.write(buff.data(), n);
    if (get_full_package(stream)) {
      string cmd{stream.str(), index_};
      unique_ptr<FILE, decltype(close_file)> pipe{popen((cmd + " 2>&1"s).c_str(), "r"), close_file};

      stream.str("");
      while ((n = fread(buff.data(), sizeof(buff[0]), buff.size(), pipe.get())) > 0) {
        stream.write(buff.data(), n);
      }
      stream << flag;
      send(stream);
      stream.str("");
    }
  }
  close(connect_fd_);
}

void Server::send(std::stringstream &stream) const {
  string res = stream.str();
  string head = to_string(res.size()) + '\0';

  write(connect_fd_, head.data(), head.size());
  write(connect_fd_, res.c_str(), res.size());
}

bool Server::get_full_package(std::stringstream &stream) {
  string res = stream.str();
  for (size_t i = 0; i < res.length(); i++) {
    if (res[i] == '\0') {
      index_ = i + 1;
      int len = std::stoi(string(&res[0]));

      return len + i + 1 == res.size();
    }
  }
  return false;
}