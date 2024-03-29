#include "remoteobjectserver.hpp"

RemoteObjectServer::RemoteObjectServer(const std::string &name)
    : Replyer(name) {

  // add interfce filter
  addGetFilter(std::tuple<std::string, std::function<cJSON *(RequestInfo &)>>
                {"interface", [&](RequestInfo &ri) -> cJSON* {
    cJSON *root = echoInterface();

    return root;
  }});
}

std::string RemoteObjectServer::getServerIp() {
  return mIp;
}

void RemoteObjectServer::setServerIp(const std::string &newip) {
  mIp = newip;
}

void RemoteObjectServer::parseJString(const std::string &jstring) {
  cJSON *reqRoot = cJSON_Parse(jstring.c_str());

  if(nullptr != reqRoot) {
    cJSON *obj = nullptr;
     
    if((obj = cJSON_GetObjectItem(reqRoot, "serverip")) == nullptr) {
      cJSON_Delete(reqRoot);
      throw BadJSONFormatException();
    } else
      setServerIp(obj->valuestring);

    if((obj = cJSON_GetObjectItem(reqRoot, "serverport")) == nullptr) {
      cJSON_Delete(reqRoot);
      throw BadJSONFormatException();
    } else
      mPort = obj->valueint;

    if((obj = cJSON_GetObjectItem(reqRoot, "methods")) == nullptr) {
      cJSON_Delete(reqRoot);
      throw BadJSONFormatException();
    } else
      setInterface(obj);

  }
  else
    throw BadJSONFormatException();
}

void RemoteObjectServer::setInterface(cJSON *in) {
  mInterface = in;
}

cJSON *RemoteObjectServer::getInterface() { 
  mInReturnCount++;
  return mInterface;
}

unsigned int RemoteObjectServer::getInterfaceReturnCnt() {
  return mInReturnCount;
}

cJSON *RemoteObjectServer::echoIndexHook() {
  cJSON *root = cJSON_CreateObject();

  cJSON_AddStringToObject(root, "ip", getServerIp().c_str());
  cJSON_AddNumberToObject(root, "port", mPort);

  return root;
}

cJSON *RemoteObjectServer::echoInterface() {
  cJSON *root = cJSON_CreateObject();
  //cJSON *in = nullptr;

  cJSON_AddStringToObject(root, "objectname", mName.c_str());
  cJSON_AddItemToObject(root, "methods", mInterface);//in = cJSON_CreateObject());
  //cJSON_AddItemToObject(in, "c++", cJSON_CreateString(mInterface.c_str()));

  return root;
}
