#include "remoteobjectserver.hpp"

RemoteObjectServer::RemoteObjectServer(const std::string &name, const std::string &ip, const std::string &port)
    : Replyer(name), mIp(ip), mPort(port), mInReturnCount(0) {
}

void RemoteObjectServer::setInterface(const std::string &in) {
  mInterface = in;
}

const std::string RemoteObjectServer::getInterface() { 
  mInReturnCount++;
  return mInterface;
}

unsigned int RemoteObjectServer::getInterfaceReturnCnt() {
  return mInReturnCount;
}

cJSON *RemoteObjectServer::echoInterface() {
  cJSON *root = nullptr;
  cJSON *in = nullptr;

  cJSON_AddItemToObject(in, "objectname", cJSON_CreateString(mName.c_str()));
  cJSON_AddItemToObject(root, "interface", in = cJSON_CreateObject());
  cJSON_AddItemToObject(in, "c++", cJSON_CreateString(mInterface.c_str()));

  return root;
}

bool RemoteObjectServer::handleGetRequest(struct mg_connection *client, const std::string &uri) {
  return true;
}
