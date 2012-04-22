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
 // region dedicated to hosting virtual function client declarations \n\
<^<VirtualHostedFunctionDeclarations>^>\n\
 \n\
  void RegisterObject(const std::string &serverAddress, unsigned int localObjectPort, const std::string &serverObjName = \"\", const std::string &localServer = \"\") { \n\
    CURL *repoLink = nullptr; \n\
    cJSON *root = cJSON_CreateObject(); \n\
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
      cJSON_AddStringToObject(root, \"interface\", \"<^<ClassInterface>^>\"); \n\
 \n\
      // setup CURL \n\
      curl_easy_setopt(repoLink, CURLOPT_URL, serverUrl.c_str()); \n\
      curl_easy_setopt(repoLink, CURLOPT_POSTFIELDS, cJSON_Print(root)); \n\
      curl_easy_setopt(repoLink, CURLOPT_WRITEFUNCTION, repoReply); \n\
      curl_easy_setopt(repoLink, CURLOPT_WRITEDATA, NULL); \n\
 \n\
      // register it (object)! \n\
      curl_easy_perform(repoLink); \n\
 \n\
      // clean it up! \n\
      curl_easy_cleanup(repoLink); \n\
 \n\
      // start answering to clients \n\
      StartServingRequests(); \n\
    } \n\
  } \n\
  void StartServingRequests() { \n\
    const char *options[] = {\"listening_ports\", std::to_string(mListenPort).c_str(), NULL}; \n\
 \n\
    if((mCtx = mg_start(&ClientWebCallback, this, options)) == nullptr) \n\
      std::cerr << \"Unable to initialize object server\" << std::endl; \n\
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
      root = cJSON_Parse(reqBody); \n\
 \n\
      // aquire method name \n\
      if((jsObj = cJSON_GetObjectItem(root, \"methodname\")) != nullptr) \n\
        methodName = jsObj->valuestring; \n\
 \n\
      // handle method calls \n\
<^<ClientWebCallback>^> \n\
 \n\
      // reply return value \n\
      mg_printf(client, \"HTTP/1.1 200 Ok\\r\\n\" \n\
                         \"Content-Type: text/plain\\r\\n\\r\\n\" \n\
                         \"%s\", cJSON_Print(replyroot)); \n\
 \n\
      cJSON_Delete(root); \n\
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
};";

RCOSCreator::RCOSCreator(const std::string &hLoc) {
  std::ifstream fHeader(hLoc.c_str());

  if(fHeader.good()) {
    mHRawContent.assign(std::istreambuf_iterator<char>(fHeader), std::istreambuf_iterator<char>());
    std::cout << "Raw header content : " << std::endl
              << mHRawContent << std::endl;
  } else
    std::cerr << "Unable to open header file '" << hLoc << "'!" << std::endl;

  fHeader.close();
}

bool RCOSCreator::parseHeader() {
  // read the className
  boost::regex clasExpr("class +(\\w+) *: *public +RCObjectServer");
  boost::match_results<std::string::const_iterator> captures;

  if(boost::regex_search(mHRawContent, captures, clasExpr, boost::match_default)){
    // (1) = ClassName
    mServerClassName = captures[1];
    std::cout << "RCC : Class name aquired '" << mServerClassName << "'" << std::endl;
  } else {
    std::cerr << "Class name could not be extracted!" << std::endl;
    return false;
  }

  // get methods
  boost::regex methExpr("USERCC +(([\\w:]+) +(\\w+) *\\(([\\w, :]+)\\)) *;");
  boost::sregex_iterator m1(mHRawContent.begin(), mHRawContent.end(), methExpr);
  boost::sregex_iterator m2;
  std::for_each(m1, m2, [&](const boost::smatch &m) -> bool{
    // (1) complete function declaration, (2) = return value, (3) = method name, (4) = method argument comma separated list
    std::cout << "RCC : New method : return type '" << m[2] << "', name '" << m[3] << "' and arguments : "; 
    RCOSMethod newMet(m[2], m[3], m[1]);
    std::string argList = m[4];

    // get method arguments
    boost::regex argExpr("((?:unsigned +)?int|(?:std::)?string|double|bool|void)");
    boost::sregex_iterator m3(argList.begin(), argList.end(), argExpr);
    boost::sregex_iterator m4;
    std::for_each(m3, m4, [&](const boost::smatch &ma) -> bool{
      std::cout << ma[1] << " ";
      newMet.pushArgument(ma[1]);
    });
    std::cout << std::endl;

    mServerMethods.push_back(newMet);
  });

  // clean header
  mHProcessedContent = boost::regex_replace(mHRawContent, boost::regex("USERCC "), "");
  mHProcessedContent = boost::regex_replace(mHProcessedContent, boost::regex(std::string("#include +\"")+ getFileSaveName() + std::string("\"")), "");
  mHProcessedContent = boost::regex_replace(mHProcessedContent, boost::regex("[:]? *public +RCObjectServer *"), "");
  mHProcessedContent = boost::regex_replace(mHProcessedContent, boost::regex("\\n\\n"), "\\n");
  // format header
  mHProcessedContent = boost::regex_replace(mHProcessedContent, boost::regex("\""), "\\\\\\\\\"");
  mHProcessedContent = boost::regex_replace(mHProcessedContent, boost::regex("\\n"), "\\\\\\\\n\\\\\\\\\\n");

  std::cout << "Cleaned and formatted header :" << std::endl
            << mHProcessedContent << std::endl;

  return true;
}

void RCOSCreator::composeSource() {
  mSContent = rcosClassTemplate;

  // fill in ClassName, ClassInterface and virtual function declarations
  mSContent = boost::regex_replace(mSContent, boost::regex("<\\^<ObjectRegisterName>\\^>"), mServerClassName);
  mSContent = boost::regex_replace(mSContent, boost::regex("<\\^<ClassInterface>\\^>"), mHProcessedContent);

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
    formattedClientFcts += met.getCompleteDeclaration() + std::string("\\n");

  return formattedClientFcts;
}

void RCOSCreator::saveRCOS(std::string dir) {
  if(dir[dir.length() - 1] != '/') dir += "/";
  std::ofstream rcosResult(dir + getFileSaveName());

  rcosResult << mSContent;
  rcosResult.close();
}

std::string RCOSCreator::getFileSaveName() {
  std::string fileNameChunk = mServerClassName;
  std::transform(fileNameChunk.begin(), fileNameChunk.end(), fileNameChunk.begin(), ::tolower);

  return std::string("rcos_") + fileNameChunk + std::string(".cpp");
}
