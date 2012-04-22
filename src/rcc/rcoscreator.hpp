#ifndef _RCOSCREATOR_HPP_
#define _RCOSCREATOR_HPP_

#include <iostream>
#include <fstream>
#include <list>
#include <boost/regex.hpp>
#include "rcosmethod.hpp"

class RCOSCreator {
public:
  RCOSCreator(const std::string &hLoc);

  bool parseHeader();
  void composeSource();
  void saveRCOS(std::string dir);

private:
  std::string mSContent;
  std::string mServerClassName;
  std::string mHRawContent;
  std::string mHProcessedContent;
  std::list<RCOSMethod> mServerMethods;

  std::string getClientWebCallCode();
  std::string getVirtualClientFctDeclarations();
  std::string getFileSaveName();
};

#endif
