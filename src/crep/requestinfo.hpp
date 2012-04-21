#ifndef _REQUESTINFO_HPP_
#define _REQUESTINFO_HPP_

#include <iostream>
#include "mongoose/mongoose.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class RequestInfo {
public:
  RequestInfo(struct mg_connection *cl, const struct mg_request_info *ri);

  void setClientStructure(struct mg_connection *newcl);
  struct mg_connection *getClientStructure();
  void setRequestInfoStructure(struct mg_request_info *newri);
  char *getCompleteUri() const;

  std::string getRequestObjectName();
  std::string getRequestMethodName();
  std::string getClientFormattedIp();
  int getClientPort();
private:
  void parseUri(const std::string &origUri);
protected:
  std::string mObjectReqName;
  std::string mMethodReqName;
  struct mg_connection *mClient;
  const struct mg_request_info *mRequestInfo;
};

#endif
