#ifndef _REMOTEOBJECTSERVER_HPP_
#define _REMOTEOBJECTSERVER_HPP_

#include <iostream>
#include "cJSON/cJSON.h"
#include "replyer.hpp"

class RemoteObjectServer : public Replyer {
public:
  RemoteObjectServer(const std::string &name, const std::string &ip, const std::string &port);
  
  void setInterface(const std::string &in);
  const std::string getInterface();
  unsigned int getInterfaceReturnCnt();
  cJSON *echoInterface();

  virtual bool handleGetRequest(struct mg_connection *client, const std::string &uri);

protected:
  std::string mIp;
  std::string mPort;
  std::string mInterface;
  unsigned int mInReturnCount;
};

#endif
