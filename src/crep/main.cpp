#include <iostream>
#include <cstdio>
#include "requestinfo.hpp"
#include "replyer.hpp"
#include "remoteobjectserver.hpp"
#include "remrep.hpp"

int main(int argc, char *argv[]) {
  RemRep *rep = new RemRep("8081");

  rep->StartRepServer();
  std::cout << "CRepo service started ... press any key to stop it." << std::endl;
  getchar();
  std::cout << "CRepo service stopped ... exiting application." << std::endl;

  delete rep;
  return 0;
}
