#include <cstring>
#include "rcosrefurbisher.hpp"
#include "rcoscreator.hpp"

void printUsage() {
  printf("Usage : rcc <-s|-c> <input_file> <output_dir>\n"
         "Where : -s = scan input_file for object server creation\n"
         "        -c = scan input)file for object client creation\n"
         "        input_file = file to scan\n"
         "        output_dir = resulting file directory location\n"
         "All options are mandatory!\n");
}

int main(int argc, char *argv[]) {
  if(argc != 4) {
    printUsage();
    return 1; 
  }

  RCOSInterface *rcosc = nullptr;

  if(strcmp(argv[1], "-s") == 0) {
    /* build server object */
    rcosc = new RCOSCreator(argv[2]);
  } else if(strcmp(argv[1], "-c") == 0) {
    /* build client object */
    rcosc = new RCOSRefurbisher(argv[2]);
  }  

  rcosc->parseCode();
  rcosc->composeSource();
  rcosc->saveRCOS(argv[3]);

  delete rcosc;
  return 0;
}
