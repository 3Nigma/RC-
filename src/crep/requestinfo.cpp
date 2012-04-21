#include "requestinfo.hpp"

RequestInfo::RequestInfo(struct mg_connection *cl, const struct mg_request_info *ri) : mClient(cl), mRequestInfo(ri) {
  parseUri(ri->uri);
}

void RequestInfo::setClientStructure(struct mg_connection *newcl) {
  mClient = newcl;
}

struct mg_connection *RequestInfo::getClientStructure() {
  return mClient;
}

void RequestInfo::setRequestInfoStructure(struct mg_request_info *newri) {
  mRequestInfo = newri;
  parseUri(newri->uri);
}

char *RequestInfo::RequestInfo::getCompleteUri() const {
  return mRequestInfo->uri;
}

std::string RequestInfo::getRequestObjectName() {
  return mObjectReqName;
}

std::string RequestInfo::getRequestMethodName() {
  return mMethodReqName;
}

std::string RequestInfo::getClientFormattedIp() {
  struct in_addr addr;
  addr.s_addr = ntohl(mRequestInfo->remote_ip);
  std::string ip = inet_ntoa(addr);

  return ip;
}

int RequestInfo::getClientPort() {
  return mRequestInfo->remote_port;
}

void RequestInfo::parseUri(const std::string &uri) {
  std::string processedUri = uri;
  size_t delimPos;

  // get object request name
  if(processedUri[0] == '/') processedUri = processedUri.substr(1);  // go pass the '/' character

  delimPos = processedUri.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
  if(delimPos == std::string::npos) {
    mObjectReqName = processedUri;
    processedUri = "";
  } else {
    mObjectReqName = processedUri.substr(0, delimPos);
    processedUri = processedUri.substr(delimPos);
  }

  // get method request name
  if(processedUri.length() != 0 && processedUri[0] == '/') processedUri = processedUri.substr(1);  // go pass the '/' character

  delimPos = processedUri.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
  if(delimPos == std::string::npos) {
    mMethodReqName = processedUri;
    processedUri = "";
  } else {
    mMethodReqName = processedUri.substr(0, delimPos);
    processedUri = processedUri.substr(delimPos);
  }
}
