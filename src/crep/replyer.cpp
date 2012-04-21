#include "replyer.hpp"

Replyer::Replyer(const std::string &name) : mName(name) {
  time_t rawtime;

  time(&rawtime);
  mCreationTime = localtime(&rawtime);
}

Replyer::~Replyer() {
  mPOSTFilters.clear();
  mGETFilters.clear();
}

void Replyer::setName(const std::string &name) {
  mName = name;
}

std::string Replyer::getName() {
  return mName;
}

void Replyer::addStatusMsg(cJSON **root, const std::string &content) {
  if((*root) == nullptr) (*root) = cJSON_CreateObject();

  cJSON_AddStringToObject((*root), "statusmsg", content.c_str());
}

cJSON *Replyer::handleGetRequest(RequestInfo &ri) {
  return this->handleRequest(ri, mGETFilters);
}

cJSON *Replyer::handlePostRequest(RequestInfo &ri) {
  return this->handleRequest(ri, mPOSTFilters);
}

void Replyer::addPostFilter(const std::tuple<std::string, std::function<cJSON *(RequestInfo &, const std::string &)> > &newpfil) {
  mPOSTFilters.push_back(newpfil);
}

void Replyer::addGetFilter(const std::tuple<std::string, std::function<cJSON *(RequestInfo &, const std::string &)> > &newgfil) {
  mGETFilters.push_back(newgfil);
}

cJSON *Replyer::handleRequest(RequestInfo &ri, std::list<std::tuple<std::string, std::function<cJSON *(RequestInfo &, const std::string &)>>> &fFilters) {
  cJSON *root = nullptr;
  std::string processedUri = ri.getCompleteUri();

  if(processedUri == "/") processedUri = "/self";
  if(processedUri[0] == '/') processedUri = processedUri.substr(1);  // go pass the '/' character

  if(processedUri.find(this->mName) == 0) {
    // the request is intended for current Replyer instance
    processedUri = processedUri.substr(this->mName.length());
    if(processedUri[0] == '/') processedUri = processedUri.substr(1);  // go pass the '/' character

    // aquire method name
    size_t delimPos = processedUri.find("/");
    std::string methodName;
    if(delimPos != std::string::npos) {
      methodName = processedUri.substr(0, delimPos);
      processedUri = processedUri.substr(delimPos);
    } else {
      methodName = processedUri;
      processedUri = "";
    }
   
    for(std::tuple<std::string, std::function<cJSON *(RequestInfo &, const std::string &)>> fCandidate : fFilters)
      if(std::get<0>(fCandidate) == methodName) {
        root = (std::get<1>(fCandidate))(ri, processedUri);

        if(cJSON_GetObjectItem(root, "statusmsg") == nullptr)
          addStatusMsg(&root, "All OK");
      }

    if(root == nullptr)
      addStatusMsg(&root, "Unable to process request : unrecognized METHOD");
  }
  else
    addStatusMsg(&root, "Unable to process request : unrecognized OBJECT");  

  return root;
}

cJSON *Replyer::echoIndexHook() {
  return nullptr;
}

cJSON *Replyer::echoIndex() {
  std::string formattedCreationTime = asctime(mCreationTime);
  cJSON *root = cJSON_CreateObject();
  cJSON *childSpecRoot = nullptr;

  cJSON_AddStringToObject(root, "objectname", mName.c_str());
  cJSON_AddStringToObject(root, "createdon", formattedCreationTime.substr(0, formattedCreationTime.length()-1).c_str());

  if((childSpecRoot = echoIndexHook()) != nullptr)
    cJSON_AddItemReferenceToObject(root, "hookreport", childSpecRoot);

  return root;
}
