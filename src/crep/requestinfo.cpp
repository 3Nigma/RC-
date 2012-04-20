#include "requestinfo.hpp"

RequestInfo::RequestInfo(struct mg_connection *cl, const struct mg_request_info *ri) : mClient(cl), mRequestInfo(ri) { }

void RequestInfo::setClientStructure(struct mg_connection *newcl) {
  mClient = newcl;
}

struct mg_connection *RequestInfo::getClientStructure() {
  return mClient;
}

void RequestInfo::setRequestInfoStructure(struct mg_request_info *newri) {
  mRequestInfo = newri;
}

char *RequestInfo::RequestInfo::getCompleteUri() const {
  return mRequestInfo->uri;
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
