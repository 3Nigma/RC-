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

  std::string getName();
  void addStatusMsg(cJSON **root, const std::string &content);

  cJSON *handleGetRequest(RequestInfo &ri);
  cJSON *handlePostRequest(RequestInfo &ri);

  void addPostFilter(const std::tuple<std::string, std::function<cJSON *(RequestInfo &, const std::string &)> > &newpfil);
  void addGetFilter(const std::tuple<std::string, std::function<cJSON *(RequestInfo &, const std::string &)> > &newgfil);
protected:
  std::string mName;
  std::list<std::tuple<std::string, std::function<cJSON *(RequestInfo &, const std::string &)>>> mGETFilters;
  std::list<std::tuple<std::string, std::function<cJSON *(RequestInfo &, const std::string &)>>> mPOSTFilters;
  struct tm *mCreationTime;

  virtual cJSON *echoIndexHook();
  virtual cJSON *echoIndex();
  cJSON *handleRequest(RequestInfo &ri, std::list<std::tuple<std::string, std::function<cJSON *(RequestInfo &, const std::string &)>>> &fFilters);
};

#endif
