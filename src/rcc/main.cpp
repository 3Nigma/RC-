#include "rcosmethod.hpp"
#include "rcoscreator.hpp"

int main(int argc, char *argv[]) {
  RCOSCreator rcosc(argv[1]);

  rcosc.parseHeader();
  rcosc.composeSource();

  return 0;
}
