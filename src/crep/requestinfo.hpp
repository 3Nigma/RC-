#ifndef _REQUESTINFO_HPP_
#define _REQUESTINFO_HPP_

#include <iostream>
#include <sstream>
#include <vector>
#include "mongoose/mongoose.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "exceptions.hpp"

class RequestInfo {
public:
  RequestInfo(struct mg_connection *cl, const struct mg_request_info *ri);

  void setClientStructure(struct mg_connection *newcl);
  struct mg_connection *getClientStructure();
  void setRequestInfoStructure(struct mg_request_info *newri);
  char *getCompleteUri() const;
  char *getCompleteQuery() const;

  std::string getRequestObjectName();
  std::string getRequestMethodName();
  std::string getRequestMethodArgument(unsigned int index);
  std::string getClientFormattedIp();
  int getClientPort();
private:
  void parseCompleteUri(const std::string &uri, const std::string &query);
  std::vector<std::string> splitString(const std::string &s, char delim); // http://stackoverflow.com/a/236803
protected:
  std::string mObjectReqName;
  std::string mMethodReqName;
  std::vector<std::string> mArguments;
  struct mg_connection *mClient;
  const struct mg_request_info *mRequestInfo;
};

#endif
