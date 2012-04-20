#include "replyer.hpp"

Replyer::Replyer(const std::string &name) : mName(name) {
  time_t rawtime;

  time (&rawtime);
  mCreationTime = localtime(&rawtime);
}

cJSON *Replyer::echoIndexHook() {
  return nullptr;
}

cJSON *Replyer::echoIndex() {
  std::string formattedCreationTime = asctime(mCreationTime);
  cJSON *root = cJSON_CreateObject();
  cJSON *childSpecRoot = nullptr;

  cJSON_AddItemToObject(root, "objectname", cJSON_CreateString(mName.c_str()));
  cJSON_AddItemToObject(root, "createdon", cJSON_CreateString(formattedCreationTime.substr(0, formattedCreationTime.length()-1).c_str()));

  if((childSpecRoot = echoIndexHook()) != nullptr)
    cJSON_AddItemReferenceToObject(root, "hookreport", childSpecRoot);

  return root;
}
