#include <iostream>
#include "rcos_MathServer.cpp"

int main(int argc, char *argv[]) {
  MathServer *rcosMath = MathServer::getInstance("127.0.0.1:8081");

  std::cout << "1.5 + 1.8 = " << rcosMath->do_add(1.5, 1.8) << std::endl;
  std::cout << "sqrt(3.0) = " << rcosMath->do_sqr(3.0) << std::endl;

  delete rcosMath;

  return 0;
}
