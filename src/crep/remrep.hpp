#ifndef _REMREP_HPP_
#define _REMREP_HPP_

#include <iostream>
#include <algorithm>
#include <functional>
#include <tuple>
#include <list>
#include <cstring>
#include "mongoose/mongoose.h"
#include "exceptions.hpp"
#include "replyer.hpp"
#include "remoteobjectserver.hpp"

class RemRep : public Replyer {
public:
  RemRep(const std::string &lports);
  ~RemRep();

  void StartRepServer();
  void StopRepServer();

protected:
  std::list<Replyer *> mRMpool;
  std::string mListenPorts;
  struct mg_context *mCtx;

  virtual cJSON *echoIndexHook();

  static void *WebCallback(enum mg_event event, struct mg_connection *client, const struct mg_request_info *request_info);
};


#endif
