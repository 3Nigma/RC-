#include "requestinfo.hpp"

RequestInfo::RequestInfo(struct mg_connection *cl, const struct mg_request_info *ri) : mClient(cl), mRequestInfo(ri) {
  parseCompleteUri(ri->uri, ri->query_string == nullptr ? "" : ri->query_string);
}

void RequestInfo::setClientStructure(struct mg_connection *newcl) {
  mClient = newcl;
}

struct mg_connection *RequestInfo::getClientStructure() {
  return mClient;
}

void RequestInfo::setRequestInfoStructure(struct mg_request_info *newri) {
  mRequestInfo = newri;
  parseCompleteUri(newri->uri, newri->query_string == nullptr ? "" : newri->query_string);
}

char *RequestInfo::getCompleteUri() const {
  return mRequestInfo->uri;
}

char *RequestInfo::getCompleteQuery() const {
  return mRequestInfo->query_string;
}

std::string RequestInfo::getRequestObjectName() {
  return mObjectReqName;
}

std::string RequestInfo::getRequestMethodName() {
  return mMethodReqName;
}

std::string RequestInfo::getRequestMethodArgument(unsigned int index) {
  if(index >= mArguments.size()) throw ArgumentIndexOutOfBounds();

  return mArguments[index];
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

std::vector<std::string> RequestInfo::splitString(const std::string &s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;

    while(!s.empty() && std::getline(ss, item, delim)) {
        elems.push_back(item);
    }

    return elems;
}

void RequestInfo::parseCompleteUri(const std::string &uri, const std::string &query) {
  std::string processedUri = uri;
  std::string processedQuery = query;
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

  // get present arguments
  mArguments = splitString(processedQuery, ';');
}
