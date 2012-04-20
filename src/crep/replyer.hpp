#ifndef _REPLYER_HPP_
#define _REPLYER_HPP_

#include <iostream>
#include <functional>
#include <tuple>
#include <list>
#include "cJSON/cJSON.h"
#include "requestinfo.hpp"

class Replyer {
public:
  Replyer(const std::string &name);
  ~Replyer();

  bool handleGetRequest(RequestInfo &ri);
  bool handlePostRequest(RequestInfo &ri);

  void addPostFilter(const std::tuple<std::string, std::function<void(RequestInfo &)> > &newpfil);
  void addGetFilter(const std::tuple<std::string, std::function<void(RequestInfo &)> > &newgfil);
protected:
  std::string mName;
  std::list<std::tuple<std::string, std::function<void(RequestInfo &)>>> mGETFilters;
  std::list<std::tuple<std::string, std::function<void(RequestInfo &)>>> mPOSTFilters;
  struct tm *mCreationTime;

  virtual cJSON *echoIndexHook();
  virtual cJSON *echoIndex();
  bool handleRequest(RequestInfo &ri, std::list<std::tuple<std::string, std::function<void(RequestInfo &)>>> &fFilters);
};

#endif
