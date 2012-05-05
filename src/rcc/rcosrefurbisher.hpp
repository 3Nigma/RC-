#ifndef _RCOSREFURBISHER_HPP_
#define _RCOSREFURBISHER_HPP_

#include "rcosinterface.hpp"

class RCOSRefurbisher : public RCOSInterface {
public:
  RCOSRefurbisher(const std::string &codeFile);

  virtual bool parseCode();
  virtual void composeSource();
};

#endif
