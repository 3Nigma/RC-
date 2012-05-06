#include <cstdio>
#include "rcos_InfoServer.cpp"

int main(int argc, char *argv[]) {
  InfoServer *rcosInfo = InfoServer::getInstance("127.0.0.1:8081");

  std::cout << "Informatii despre drumul 0 : " << rcosInfo->getRoadInfo(0) << std::endl;
  std::cout << "Temperatura pentru Timisoara : " << rcosInfo->getTemp("Timisoara") << std::endl;

  delete rcosInfo;
  return 0;
}
