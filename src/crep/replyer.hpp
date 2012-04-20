#ifndef _REPLYER_HPP_
#define _REPLYER_HPP_

#include <iostream>
#include "cJSON/cJSON.h"
#include "requestinfo.hpp"

class Replyer {
public:
  Replyer(const std::string &name);
 
  virtual bool handleGetRequest(RequestInfo &ri) = 0;
protected:
  std::string mName;
  struct tm *mCreationTime;

  virtual cJSON *echoIndexHook();
  virtual cJSON *echoIndex();
};

#endif
