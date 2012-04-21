#include "remoteobjectserver.hpp"

RemoteObjectServer::RemoteObjectServer()
    : Replyer() {
}

cJSON *RemoteObjectServer::parseJString(const std::string &jstring) {
  cJSON *root = cJSON_Parse(jstring.c_str());

  if(nullptr != root) {
    cJSON_Delete(root);
  }
  else
    throw BadJSONFormatException();

  return root;
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
