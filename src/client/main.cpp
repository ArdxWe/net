#include "Client.h"

using namespace std::string_literals;

int main(int argc, char **argv) {
  Client demo{"127.0.0.1"s, 9900};
  demo.connect_server();
  demo.start();
  return 0;
}
