#include "rcoscreator.hpp"

std::string rcosClassTemplate = "// RemoteC++Compiler generated file. Please be carefull what you'r touching here! \n\
#include <iostream> \n\
#include <string> \n\
#include <cstring> \n\
#include <cJSON/cJSON.h> \n\
#include \"mongoose/mongoose.h\" \n\
#include \"curl/curl.h\" \n\
 \n\
#define USERCC virtual \n\
 \n\
class RCObjectServer { \n\
public: \n\
  RCObjectServer() = default; \n\
 \n\
// ### Virtual exported object methods Region ### \n\
<^<VirtualHostedFunctionDeclarations>^>\n\
// ### END OF REGION ### \n\
 \n\
  void RegisterObject(const std::string &serverAddress, unsigned int localObjectPort, const std::string &serverObjName = \"\", const std::string &localServer = \"\") { \n\
    if(!sendRegistrationRequest(serverAddress, localObjectPort, serverObjName, localServer)) { \n\
      return; \n\
    } \n\
 \n\
    // start answering to clients \n\
    StartServingRequests(); \n\
  } \n\
  bool StartServingRequests() { \n\
    std::string listenPort = std::to_string(mListenPort).c_str(); \n\
    const char *options[] = {\"listening_ports\", listenPort.c_str(), NULL}; \n\
 \n\
    if((mCtx = mg_start(&ClientWebCallback, this, options)) == nullptr) {\n\
      std::cerr << \"Unable to initialize object server\" << std::endl; \n\
      return false; \n\
    } \n\
 \n\
    return true; \n\
  } \n\
  void StopServingRequests() { \n\
    mg_stop(mCtx); \n\
  } \n\
protected: \n\
  unsigned int mListenPort; \n\
  struct mg_context *mCtx; \n\
  \n\
  static void *ClientWebCallback(enum mg_event event, struct mg_connection *client, const struct mg_request_info *request_info) { \n\
    char reqBody[32768]; \n\
    cJSON *root = nullptr; \n\
    cJSON *replyroot = cJSON_CreateObject(); \n\
    cJSON *jsObj = nullptr; \n\
    std::string methodName; \n\
    RCObjectServer *rcos = (RCObjectServer *)request_info->user_data; \n\
 \n\
    switch(event) { \n\
    case MG_NEW_REQUEST:  // New HTTP request has arrived from the client \n\
      mg_read(client, reqBody, sizeof(reqBody)); \n\
      if(strlen(reqBody) == 0 || (root = cJSON_Parse(reqBody)) == nullptr) { \n\
        cJSON_AddStringToObject(replyroot, \"statusmsg\", \"No or invalid JSON message body provided!\"); \n\
      } else { \n\
        // aquire method name \n\
        if((jsObj = cJSON_GetObjectItem(root, \"methodname\")) != nullptr) \n\
          methodName = jsObj->valuestring; \n\
 \n\
        // handle method calls \n\
        try { \n\
<^<ClientWebCallback>^> \n\
        } catch(...) { \n\
          std::cerr << \"Something horrible happened to the actual method calls!\" << std::endl; \n\
        }\n\
 \n\
        cJSON_Delete(root); \n\
      } \n\
 \n\
      // reply return value \n\
      mg_printf(client, \"HTTP/1.1 200 Ok\\r\\n\" \n\
                        \"Content-Type: text/plain\\r\\n\\r\\n\" \n\
                        \"%s\", cJSON_Print(replyroot)); \n\
      cJSON_Delete(replyroot); \n\
 \n\
      return (void *)\"\";\n\
      break; \n\
    default: \n\
      break; \n\
    } \n\
 \n\
    return NULL; \n\
  } \n\
 \n\
  static size_t repoReply(void *ptr, size_t elSize, size_t elCount, void *destStream) { \n\
    std::string reply = (char *)ptr; \n\
    cJSON *root = cJSON_Parse(reply.c_str()); \n\
 \n\
    std::cout << \"CRep replied : \" << cJSON_GetObjectItem(root, \"statusmsg\")->valuestring << std::endl; \n\
    cJSON_Delete(root); \n\
 \n\
    return elCount; \n\
  } \n\
 \n\
  bool sendRegistrationRequest(const std::string &serverAddress, unsigned int localObjectPort, const std::string &serverObjName, const std::string &localServer) { \n\
    char errBuff[CURL_ERROR_SIZE]; \n\
    CURL *repoLink = nullptr; \n\
    cJSON *root = cJSON_CreateObject(); \n\
    cJSON *methodsRoot = cJSON_CreateObject(); \n\
    cJSON *currentMethod = nullptr; \n\
    cJSON *currentArgList = nullptr; \n\
 \n\
    std::string serverUrl; \n\
    if(serverAddress.find(\"http://\") != 0) \n\
      serverUrl = \"http://\"; \n\
    if(serverObjName.empty()) \n\
      serverUrl += serverAddress + \"/self/register?<^<ObjectRegisterName>^>\"; \n\
    else \n\
      serverUrl += serverAddress + \"/self/register?\" + serverObjName; \n\
 \n\
    if((repoLink = curl_easy_init()) != nullptr) { \n\
      // create register body \n\
      mListenPort = localObjectPort; \n\
      cJSON_AddStringToObject(root, \"serverip\", localServer.c_str()); \n\
      cJSON_AddNumberToObject(root, \"serverport\", localObjectPort); \n\
 \n\
// ### JSON Method Export Region ### \n\
<^<JSONExportedMethods>^> \n\
// ### END OF REGION ### \n\
\n\
      cJSON_AddItemToObject(root, \"methods\", methodsRoot); \n\
 \n\
      // setup CURL \n\
      curl_easy_setopt(repoLink, CURLOPT_ERRORBUFFER, errBuff); \n\
      curl_easy_setopt(repoLink, CURLOPT_URL, serverUrl.c_str()); \n\
      curl_easy_setopt(repoLink, CURLOPT_POSTFIELDS, cJSON_Print(root)); \n\
      curl_easy_setopt(repoLink, CURLOPT_WRITEFUNCTION, repoReply); \n\
      curl_easy_setopt(repoLink, CURLOPT_WRITEDATA, NULL); \n\
 \n\
      // register it (object)! \n\
      if(curl_easy_perform(repoLink) != 0) { \n\
        std::cerr << \"Unable to process registration! : \" << errBuff << std::endl; \n\
        return false; \n\
      } \n\
 \n\
      // clean it up! \n\
      curl_easy_cleanup(repoLink); \n\
    } else { \n\
      return false; \n\
    } \n\
   \n\
    return true; \n\
  } \n\
};";

RCOSCreator::RCOSCreator(const std::string &cLoc) 
  : RCOSInterface(cLoc){
  
}

bool RCOSCreator::parseCode() {
  // read the className
  boost::regex clasExpr("class +(\\w+) *: *public +RCObjectServer");
  boost::match_results<std::string::const_iterator> captures;

  if(boost::regex_search(mRawFileContent, captures, clasExpr, boost::match_default)){
    // (1) = ClassName
    mServerClassName = captures[1];
    std::cout << "RCC : Class name aquired '" << mServerClassName << "'" << std::endl;
  } else {
    std::cerr << "Class name could not be extracted!" << std::endl;
    return false;
  }

  // get methods
  boost::regex methExpr("USERCC *([\\w, :\\)\\(]+) *;");
  boost::sregex_iterator m1(mRawFileContent.begin(), mRawFileContent.end(), methExpr);
  boost::sregex_iterator m2;
  std::for_each(m1, m2, [&](const boost::smatch &m) -> bool{
    // (1) complete function declaration
    RCOSMethod newMet(m[1]);
    mServerMethods.push_back(newMet);
  });

  // clean header
   mHProcessedContent = boost::regex_replace(mRawFileContent, boost::regex("USERCC "), "");
   mHProcessedContent = boost::regex_replace(mHProcessedContent, boost::regex(std::string("#include +\"")+ getFileSaveName() + std::string("\"")), "");
   mHProcessedContent = boost::regex_replace(mHProcessedContent, boost::regex("[:]? *public +RCObjectServer *"), "");
   mHProcessedContent = boost::regex_replace(mHProcessedContent, boost::regex("\\n\\n"), "\\n");
  // format header
   mHProcessedContent = boost::regex_replace(mHProcessedContent, boost::regex("\""), "\\\\\\\\\"");
   mHProcessedContent = boost::regex_replace(mHProcessedContent, boost::regex("\\n"), "\\\\\\\\n\\\\\\\\\\n");
  // mHProcessedContent = getClientWebCallCode();
  std::cout << "Cleaned and formatted header :" << std::endl
            << mHProcessedContent << std::endl;

  return true;
}

void RCOSCreator::composeSource() {
  mSContent = rcosClassTemplate;

  // fill in ClassName, ClassInterface and virtual function declarations
  mSContent = boost::regex_replace(mSContent, boost::regex("<\\^<ObjectRegisterName>\\^>"), mServerClassName);
  mSContent = boost::regex_replace(mSContent, boost::regex("<\\^<JSONExportedMethods>\\^>"), getJPostedInterface("methodsRoot", "currentMethod", "currentArgList"));

  // add methods and method calls
  mSContent = boost::regex_replace(mSContent, boost::regex("<\\^<VirtualHostedFunctionDeclarations>\\^>"), getVirtualClientFctDeclarations());
  mSContent = boost::regex_replace(mSContent, boost::regex("<\\^<ClientWebCallback>\\^>"), getClientWebCallCode());

  std::cout << "Composed source code :" << std::endl
            << mSContent;
}

std::string RCOSCreator::getClientWebCallCode() {
  std::string callCode = "";

  for(RCOSMethod met : mServerMethods) {
    callCode += met.getFormattedCallCode() + std::string("\\n");
  }

  return callCode;
}

std::string RCOSCreator::getVirtualClientFctDeclarations() {
  std::string formattedClientFcts = "";

  for(RCOSMethod met : mServerMethods)
    formattedClientFcts += met.getPureVirtualDeclaration() + std::string("\\n");

  return formattedClientFcts;
}

std::string RCOSCreator::getJPostedInterface(const std::string &jRoot, const std::string &jMethodHolder, const std::string &jArgListHolder) {
  std::string clientInterface = "";

  for(RCOSMethod met : mServerMethods)
    clientInterface += met.getJSONIntegrationInstructions(jRoot, jMethodHolder, jArgListHolder) + std::string("\\n");

  return clientInterface;
}
