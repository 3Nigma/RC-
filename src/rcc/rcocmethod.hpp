#ifndef _RCOCMETHOD_HPP_
#define _RCOCMETHOD_HPP_

#include "methodinterface.hpp"

class RCOCMethod : public MethodInterface {
public:
  RCOCMethod(cJSON *methNode);

  std::string getInvocationCode(const std::string &objServerAddress, const std::string &localObjCallback);
};

#endif
