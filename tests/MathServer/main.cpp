#include <iostream>
#include "mathserver.hpp"

int main(int argc, char *argv[]) {
  MathServer rcosMath;

  rcosMath.RegisterObject("127.0.0.1:8081", 8082);
  getchar();

  return 0;
}
