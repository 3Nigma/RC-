#include "rcosrefurbisher.hpp"

RCOSRefurbisher::RCOSRefurbisher(const std::string &codeFile) 
  : RCOSInterface(codeFile) {

}

void RCOSRefurbisher::setRemoteHeaderContent(const std::string &data) {
  mRawRemoteHeaderContent = data;
  std::cout << data << std::endl;
}

static size_t repoReply(void *ptr, size_t elSize, size_t elCount, void *userdata) {
  std::string reply = (char *)ptr;
  RCOSRefurbisher *parent = (RCOSRefurbisher *)userdata;
  cJSON *root = nullptr;
  cJSON *hElement = nullptr;

  if(nullptr != parent) {
    root = cJSON_Parse(reply.c_str());
    hElement = cJSON_GetObjectItem(root, "interface");
    if(nullptr != hElement) {
      parent->setRemoteHeaderContent(hElement->valuestring);
    }
    cJSON_Delete(root);

    return elCount;
  }

  return 0;
}


bool RCOSRefurbisher::parseCode() {
  CURL *repoLink = nullptr;
  char errBuff[4096];

  if((repoLink = curl_easy_init()) != nullptr) {
    // read the className
    boost::regex clasExpr("#include +\"rcos_(\\w+)\\.cpp\"");
    boost::match_results<std::string::const_iterator> classCaptures;

    if(boost::regex_search(mRawFileContent, classCaptures, clasExpr, boost::match_default)){
      // (1) = ClassName
      mServerClassName = classCaptures[1];
      std::cout << "RCC : Class name aquired '" << mServerClassName << "'" << std::endl;
    } else {
      std::cerr << "Class name could not be extracted!" << std::endl;
      return false;
    }

    // get naming service address
    boost::regex nsExpr(mServerClassName + " *\\. *getInstance\\( *\"([\\w:\\.]+)\" *\\)");
    boost::match_results<std::string::const_iterator> nsCaptures;
    
    if(boost::regex_search(mRawFileContent, nsCaptures, nsExpr, boost::match_default)){
      // (1) = ClassName
      mNamingServiceAddress = nsCaptures[1];
    } else {
      std::cerr << "'" << mServerClassName << "' : Naming service could not be extracted!" << std::endl;
      return false;
    }
    
    // aquire header info
    curl_easy_setopt(repoLink, CURLOPT_ERRORBUFFER, errBuff);
    curl_easy_setopt(repoLink, CURLOPT_URL, (mNamingServiceAddress + "/" + mServerClassName + "/interface").c_str());
    curl_easy_setopt(repoLink, CURLOPT_WRITEFUNCTION, repoReply);
    curl_easy_setopt(repoLink, CURLOPT_WRITEDATA, this);
    
    if(curl_easy_perform(repoLink) != 0) {
      std::cerr << "Unable to aquire header! : " << errBuff << std::endl;
      return false;
    }

    // clean it up!
    curl_easy_cleanup(repoLink);
    
    return true;
  }

  return false;
}

void RCOSRefurbisher::composeSource() {

}
