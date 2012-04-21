#include <iostream>
#include <fstream>
#include <list>
#include <boost/regex.hpp>

std::string rcosTemplate = "// RemoteC++Compiler generated file. Please be carefull what you'r touching here! \
#include <iostream> \
#include <cJSON/cJSON.h> \
#include \"mongoose/mongoose.h\" \
#include \"curl/curl.h\" \
 \
class RCObjectServer { \
public: \
  RCObjectServer() = default; \
 \
  void RegisterObject(const std::string &serverAddress, unsigned int localPort, const std::string &localServer = \"\") { \
    CURL *repoLink = nullptr; \
    cJSON *root = cJSON_CreateObject(); \
 \
    std::string serverUrl; \
    if(serverAddress.find(\"http://\") != 0) \
      serverUrl = \"http://\"; \
    serverUrl += serverAddress + \"/self/register?<^<ObjectRegisterName>^>\"; \
 \
    if((repoLink = curl_easy_init()) != nullptr) { \
      // create register body \
      cJSON_AddStringToObject(root, \"serverip\", localServer.c_str()); \
      cJSON_AddNumberToObject(root, \"serverport\", localPort); \
      cJSON_AddStringToObject(root, \"interface\", \"<^<ClassInterface>^>\"); \
 \
      // setup CURL \
      curl_easy_setopt(repoLink, CURLOPT_URL, serverUrl.c_str()); \
      curl_easy_setopt(repoLink, CURLOPT_POSTFIELDS, cJSON_Print(root)); \
      curl_easy_setopt(repoLink, CURLOPT_WRITEFUNCTION, repoReply); \
      curl_easy_setopt(repoLink, CURLOPT_WRITEDATA, NULL); \
 \
      // register it (object)! \
      curl_easy_perform(repoLink); \
 \
      // clean it up! \
      curl_easy_cleanup(repoLink); \
    } \
  } \
protected: \
  static void *ClientWebCallback(enum mg_event event, struct mg_connection *client, const struct mg_request_info *request_info) { \
    <^<ClientWebCallback>^> \
  } \
 \
  static size_t repoReply(void *ptr, size_t elSize, size_t elCount, void *destStream) { \
    std::string reply = (char *)ptr; \
    cJSON *root = cJSON_Parse(reply.c_str()); \
 \
    std::cout << \"CRep replied : \" << cJSON_GetObjectItem(root, \"statusmsg\")->valuestring << std::cout; \
    cJSON_Delete(root); \
 \
    return elCount; \
  } \
};";

class RCOSMethod {
public:
  RCOSMethod(const std::string &retType, const std::string &name)
    : mReturnType(retType), mName(name) { }
  void pushArgument(const std::string &argType) {
    mArgList.push_back(argType);
  }
private:
  std::string mReturnType;
  std::string mName;
  std::list<std::string> mArgList;
};

class RCOSCreator {
public:
  RCOSCreator(const std::string &hLoc) {
    std::ifstream fHeader(hLoc.c_str());
    mHRawContent.assign(std::istreambuf_iterator<char>(fHeader), std::istreambuf_iterator<char>());
    fHeader.close();
  }

  bool parseHeader() {
    // read the className
    boost::regex c("class +(.*) +: +public +RCObjectServer");
    boost::smatch captures;

    if(boost::regex_match(mHRawContent, captures, c, boost::match_extra)){
      // (1) = ClassName
      mServerClassName = captures[1];
    } else {
      std::cerr << "Class name could not be extracted!";
      return false;
    }

    // get methods
    boost::regex methExpr("USERCC +(.*) +(.*)\\((.*)\\) *;");
    boost::sregex_iterator m1(mHRawContent.begin(), mHRawContent.end(), methExpr);
    boost::sregex_iterator m2;
    std::for_each(m1, m2, [&](const boost::smatch &m) -> bool{
      // (1) = return value, (2) = method name, (3) = method argument comma separated list
      RCOSMethod newMet(m[1], m[2]);
      std::string argList = m[3];

      // get method arguments
      boost::regex argExpr("(int|string|double)");
      boost::sregex_iterator m3(argList.begin(), argList.end(), argExpr);
      boost::sregex_iterator m4;
      std::for_each(m3, m4, [&](const boost::smatch &ma) -> bool{
        newMet.pushArgument(ma[1]);
      });

      mServerMethods.push_back(newMet);
    });

    //clean header
    mHProcessedContent = regex_replace(mHRawContent, as_xpr("USERCC "), "");

  }
private:
  std::string mServerClassName;
  std::string mHRawContent;
  std::string mHProcessedContent;
  std::list<RCOSMethod> mServerMethods;
};

int main(int argc, char *argv[]) {
  //RCObjectServer os;

  //os.RegisterObject("127.0.0.1:8081", 93);

  return 0;
}
