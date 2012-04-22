#include <cstdio>
#include "infoserver.hpp"

int main(int argc, char *argv[]) {
  InfoServer rcosInfo;

  rcosInfo.RegisterObject("127.0.0.1:8081", 8082);
  getchar();

  return 0;
}
