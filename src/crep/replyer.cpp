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

std::string Replyer::getName() {
  return mName;
}

bool Replyer::handleGetRequest(RequestInfo &ri) {
  return this->handleRequest(ri, mGETFilters);
}

bool Replyer::handlePostRequest(RequestInfo &ri) {
  return this->handleRequest(ri, mPOSTFilters);
}

void Replyer::addPostFilter(const std::tuple<std::string, std::function<void(RequestInfo &, const std::string &)> > &newpfil) {
  mPOSTFilters.push_back(newpfil);
}

void Replyer::addGetFilter(const std::tuple<std::string, std::function<void(RequestInfo &, const std::string &)> > &newgfil) {
  mGETFilters.push_back(newgfil);
}

bool Replyer::handleRequest(RequestInfo &ri, std::list<std::tuple<std::string, std::function<void(RequestInfo &, const std::string &)>>> &fFilters) {
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
   
    for(std::tuple<std::string, std::function<void(RequestInfo &, const std::string &)>> fCandidate : fFilters)
      if(std::get<0>(fCandidate) == methodName) {
        (std::get<1>(fCandidate))(ri, processedUri);
        return true;
      }
  }
  
  return false;
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
