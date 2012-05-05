#ifndef _REMOTEOBJECTSERVER_HPP_
#define _REMOTEOBJECTSERVER_HPP_

#include <iostream>
#include "cJSON/cJSON.h"
#include "exceptions.hpp"
#include "replyer.hpp"

class RemoteObjectServer : public Replyer {
public:
  RemoteObjectServer(const std::string &name);
  
  std::string getServerIp();
  void setServerIp(const std::string &newip);

  void parseJString(const std::string &jstring);
  void setInterface(cJSON *in);
  cJSON *getInterface();
  unsigned int getInterfaceReturnCnt();
  cJSON *echoInterface();

protected:
  std::string mIp;
  unsigned int mPort;
  cJSON *mInterface;
  unsigned int mInReturnCount;

  virtual cJSON *echoIndexHook();
};

#endif
