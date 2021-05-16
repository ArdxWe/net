#include "Server.h"

int main() {
  Server server{9900};
  server.start();
  return 0;
}
