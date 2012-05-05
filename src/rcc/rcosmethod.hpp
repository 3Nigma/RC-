#ifndef _RCOSMETHOD_HPP_
#define _RCOSMETHOD_HPP_

#include <iostream>
#include <vector>
#include <boost/regex.hpp>

#include "methodinterface.hpp"

class RCOSMethod : public MethodInterface {
public:
  RCOSMethod(const std::string &completeFct);

  std::string getFormattedCallCode();
};

#endif
