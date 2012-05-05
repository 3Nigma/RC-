#ifndef _RCOSCREATOR_HPP_
#define _RCOSCREATOR_HPP_

#include <iostream>
#include <fstream>
#include <list>
#include <boost/regex.hpp>

#include "rcosinterface.hpp"
#include "rcosmethod.hpp"

class RCOSCreator : public RCOSInterface {
public:
  RCOSCreator(const std::string &cLoc);

  virtual bool parseCode();
  virtual void composeSource();
private:
  std::list<RCOSMethod> mServerMethods;

  std::string getClientWebCallCode();
  std::string getVirtualClientFctDeclarations();
  std::string getJPostedInterface(const std::string &jRoot, const std::string &jMethodHolder, const std::string &jArgListHolder);
};

#endif
