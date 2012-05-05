#include "rcosrefurbisher.hpp"

const std::string rcocClassTemplate = "// RemoteC++Compiler generated file. Please be carefull what you'r touching here! \n\
#include <iostream> \n\
#include <string> \n\
#include <cstring> \n\
#include <cJSON/cJSON.h> \n\
#include \"curl/curl.h\" \n\
 \n\
class <^<ServerObjectName>^> { \n\
public: \n\
 // ### Region Hosting Invocation of Server-Side methods ### \n\
<^<RemoteObjectInvocationCode>^> \n\
 // ### END OF REGION ###\n\
  static <^<ServerObjectName>^> *getInstance(const std::string &serverAddress) { \n\
    char errBuff[4096]; \n\
    char completeObjAddress[1024]; \n\
    CURL *repoLink = nullptr; \n\
    cJSON *root = nullptr; \n\
 \n\
    if((repoLink = curl_easy_init()) != nullptr) { \n\
      // setup CURL \n\
      curl_easy_setopt(repoLink, CURLOPT_ERRORBUFFER, errBuff); \n\
      curl_easy_setopt(repoLink, CURLOPT_URL, (serverAddress + \"/<^<ServerObjectName>^>\").c_str()); \n \
      curl_easy_setopt(repoLink, CURLOPT_WRITEFUNCTION, repoReply); \n\
      curl_easy_setopt(repoLink, CURLOPT_WRITEDATA, completeObjAddress); \n\
 \n\
      if(curl_easy_perform(repoLink) != 0) { \n\
        std::cerr << \"Unable to aquire object address! : \" << errBuff << std::endl; \n\
        return false; \n\
      } else \n\
        std::cout << \"'<^<ServerObjectName>^>' found at \" << completeObjAddress << std::endl; \n\
 \n\
      // clean it up! \n\
      curl_easy_cleanup(repoLink); \n\
    } else { \n\
      return nullptr; \n\
    } \n\
 \n\
    return new <^<ServerObjectName>^>(completeObjAddress); \n\\
  } \n\
protected: \n\
  std::string mObjAddr; \n\
  \n\
  <^<ServerObjectName>^>(const std::string &mAddr) : mObjAddr(mAddr) { } \n\
 \n\
  static size_t repoReply(void *ptr, size_t elSize, size_t elCount, void *userdata) { \n\
    char buff[256]; \n\
    std::string reply = (char *)ptr; \n\
    char *formattedObjAddress = (char *)userdata; \n\
    cJSON *root = cJSON_Parse(reply.c_str()); \n\
    cJSON *objHookReport = cJSON_GetObjectItem(root, \"hookreport\"); \n\
    cJSON *objPort = nullptr; \n\
    cJSON *objIp = nullptr; \n\
    cJSON *objStatus = cJSON_GetObjectItem(root, \"statusmsg\"); \n\
 \n\
    if(nullptr != objHookReport) { \n\
      objIp = cJSON_GetObjectItem(objHookReport, \"ip\"); \n\
      objPort = cJSON_GetObjectItem(objHookReport, \"port\"); \n\
      if(nullptr != objIp && nullptr != objPort) { \n\
        sprintf(buff, \"%s:%d\", objIp->valuestring, objPort->valueint); \n\
        strcpy(formattedObjAddress, buff); \n\
      } \n\
    } \n\
    cJSON_Delete(root); \n\
 \n\
    return elCount; \n\
  } \n\
 \n\
};";


RCOSRefurbisher::RCOSRefurbisher(const std::string &codeFile) 
  : RCOSInterface(codeFile) {

}

void RCOSRefurbisher::storeJSONMethodNode(cJSON *methRoot) {
  RCOCMethod newMethod(methRoot);

  mServerMethods.push_back(newMethod);
}

static size_t repoReply(void *ptr, size_t elSize, size_t elCount, void *userdata) {
  std::string reply = (char *)ptr;
  RCOSRefurbisher *parent = (RCOSRefurbisher *)userdata;
  cJSON *root = nullptr;
  cJSON *mElement = nullptr;
  cJSON *methodNode = nullptr;
  unsigned int methCount = 0;
  unsigned int methId = 0;

  if(nullptr != parent) {
    root = cJSON_Parse(reply.c_str());
    mElement = cJSON_GetObjectItem(root, "methods");
    if(nullptr != mElement) {
      methCount = cJSON_GetArraySize(mElement);
      for(methId = 0; methId < methCount; ++methId) {
	methodNode = cJSON_GetArrayItem(mElement, methId);
	parent->storeJSONMethodNode(methodNode);
      }
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
    boost::regex nsExpr(mServerClassName + " *:: *getInstance\\( *\"([\\w:\\.]+)\" *\\)");
    boost::match_results<std::string::const_iterator> nsCaptures;
    
    if(boost::regex_search(mRawFileContent, nsCaptures, nsExpr, boost::match_default)){
      // (1) = ClassName
      mNamingServiceAddress = nsCaptures[1];
    } else {
      std::cerr << "'" << mServerClassName << "' : Naming service could not be extracted!" << std::endl;
      return false;
    }
    
    // aquire object info
    curl_easy_setopt(repoLink, CURLOPT_ERRORBUFFER, errBuff);
    curl_easy_setopt(repoLink, CURLOPT_URL, (mNamingServiceAddress + "/" + mServerClassName + "/interface").c_str());
    curl_easy_setopt(repoLink, CURLOPT_WRITEFUNCTION, repoReply);
    curl_easy_setopt(repoLink, CURLOPT_WRITEDATA, this);
    
    if(curl_easy_perform(repoLink) != 0) {
      std::cerr << "Unable to aquire header! : " << errBuff << std::endl;
      return false;
    }

    //

    // clean up naming service link
    curl_easy_cleanup(repoLink);
    
    return true;
  }

  return false;
}

std::string RCOSRefurbisher::getServerMethodsCode() {
  std::string methodsCode;

  for(RCOCMethod meth : mServerMethods)
    methodsCode += meth.getInvocationCode() + std::string("\\n");

  return methodsCode;
}

void RCOSRefurbisher::composeSource() {
  mSContent = rcocClassTemplate;

  // fill in the data on the template
  mSContent = boost::regex_replace(mSContent, boost::regex("<\\^<ServerObjectName>\\^>"), mServerClassName);
  mSContent = boost::regex_replace(mSContent, boost::regex("<\\^<RemoteObjectInvocationCode>\\^>"), getServerMethodsCode());
}
