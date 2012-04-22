#include <iostream>
#include <fstream>
#include <list>
#include <boost/regex.hpp>

std::string rcosMethodTemplate = "if(methodName == \"<^<LocalMethodName>^>\") {\n\
  <^<OtherOps>^>\
  cJSON_Add<^<cJSON_LocalMethodReturnType>^>ToObject(replyroot, \"returnvalue\", <^<LocalFunctionCallResult>^>); \n\
}";

std::string rcosClassTemplate = "// RemoteC++Compiler generated file. Please be carefull what you'r touching here! \n\
#include <iostream> \n\
#include <string> \n\
#include <cJSON/cJSON.h> \n\
#include \"mongoose/mongoose.h\" \n\
#include \"curl/curl.h\" \n\
 \n\
class RCObjectServer { \n\
public: \n\
  RCObjectServer() = default; \n\
 \n\
 \\\\ region dedicated to hosting virtual function client declarations \n\
<^<VirtualHostedFunctionDeclarations>^>\n\
 \n\
  void RegisterObject(const std::string &serverAddress, unsigned int localPort, const std::string &serverObjName = \"\", const std::string &localServer = \"\") { \n\
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
      cJSON_AddStringToObject(root, \"serverip\", localServer.c_str()); \n\
      cJSON_AddNumberToObject(root, \"serverport\", localPort); \n\
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
    } \n\
  } \n\
protected: \n\
  static void *ClientWebCallback(enum mg_event event, struct mg_connection *client, const struct mg_request_info *request_info) { \n\
    char reqBody[32768]; \n\
    cJSON *root = nullptr; \n\
    cJSON *replyroot = cJSON_CreateObject(); \n\
    cJSON *jsObj = nullptr; \n\
    std::string methodName; \n\
 \n\
    mg_read(client, reqBody, sizeof(reqBody)); \n\
    root = cJSON_Parse(reqBody); \n\
 \n\
    // aquire method name \n\
    if((jsObj = cJSON_GetObjectItem(root, \"methodname\") != nullptr) \n\
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
  } \n\
 \n\
  static size_t repoReply(void *ptr, size_t elSize, size_t elCount, void *destStream) { \n\
    std::string reply = (char *)ptr; \n\
    cJSON *root = cJSON_Parse(reply.c_str()); \n\
 \n\
    std::cout << \"CRep replied : \" << cJSON_GetObjectItem(root, \"statusmsg\")->valuestring << std::cout; \n\
    cJSON_Delete(root); \n\
 \n\
    return elCount; \n\
  } \n\
};";

class RCOSMethod {
public:
  enum ImplementedType {UNKNOWN, STRING, INTEGER, DOUBLE, BOOLEAN, VOID};

  RCOSMethod(const std::string &retType, const std::string &name, const std::string &function)
    : mName(name), mCompleteFunctionDeclaration(function) {
    mReturnType = parseTypeFromString(retType);
  }
  std::string getCompleteDeclaration() {
    return std::string("virtual ") + mCompleteFunctionDeclaration;
  }
  std::string getFormattedCallCode() {
    std::string callCode = rcosMethodTemplate;
    std::string methodCall = "";

    callCode = boost::regex_replace(rcosMethodTemplate, boost::regex("<\\^<LocalMethodName>\\^>"), mName);
    switch(mReturnType) {
    case VOID:
    case STRING:
      callCode = boost::regex_replace(callCode, boost::regex("<\\^<cJSON_LocalMethodReturnType>\\^>"), "String");
      break;
    case INTEGER:
    case DOUBLE:
      callCode = boost::regex_replace(callCode, boost::regex("<\\^<cJSON_LocalMethodReturnType>\\^>"), "Number");
      break;
    case BOOLEAN:
      callCode = boost::regex_replace(callCode, boost::regex("<\\^<cJSON_LocalMethodReturnType>\\^>"), "Bool");
      break;
    default: // unknown type
      break;
    }
   
    // fill in the method and result
    int argIndex = 0;
    methodCall += mName + std::string("(");
    for(ImplementedType argType : mArgTypes) {
      switch(argType) {
      case STRING:
        methodCall += "std::string(cJSON_GetObjectItem(root, \"argument" + std::to_string(argIndex) + "\")->valuestring)";
        break;
      case INTEGER:
        methodCall += "cJSON_GetObjectItem(root, \"argument" + std::to_string(argIndex) + "\")->valueint";
        break;
      case DOUBLE:
        methodCall += "cJSON_GetObjectItem(root, \"argument" + std::to_string(argIndex) + "\")->valuedouble";
        break;
      case BOOLEAN:
        //TODO : Implement this
        methodCall += "/* Boolean arguments not yet implemented! */";
        break;
      default: // unknown type
        break;  
      }
      if(argIndex != mArgTypes.size() - 1) methodCall += ", ";
      argIndex ++;
    }
    methodCall += std::string(")");

    if(mReturnType == VOID) {
      callCode = boost::regex_replace(callCode, boost::regex("<\\^<OtherOps>\\^>"), methodCall);
      callCode = boost::regex_replace(callCode, boost::regex("<\\^<LocalFunctionCallResult>\\^>"), "(void) No return");
    } else {
      callCode = boost::regex_replace(callCode, boost::regex("<\\^<OtherOps>\\^>"), "");
      callCode = boost::regex_replace(callCode, boost::regex("<\\^<LocalFunctionCallResult>\\^>"), methodCall);
    }

    return callCode;
  }
  void pushArgument(const std::string &argType) {
    mArgTypes.push_back(parseTypeFromString(argType));
  }
private:
  ImplementedType mReturnType;
  std::string mName;
  std::string mCompleteFunctionDeclaration;
  std::list<ImplementedType> mArgTypes;

  ImplementedType parseTypeFromString(const std::string &type) {
    ImplementedType recognizedType = UNKNOWN;

    if(type.find("int") != std::string::npos) recognizedType = INTEGER;
    else if(type.find("string") != std::string::npos) recognizedType = STRING;
    else if(type.find("double") != std::string::npos) recognizedType = DOUBLE;
    else if(type.find("bool") != std::string::npos) recognizedType = BOOLEAN;
    else if(type.find("void") != std::string::npos) recognizedType = VOID;

    return recognizedType;
  }
};

class RCOSCreator {
public:
  RCOSCreator(const std::string &hLoc) {
    std::ifstream fHeader(hLoc.c_str());

    if(fHeader.good()) {
      mHRawContent.assign(std::istreambuf_iterator<char>(fHeader), std::istreambuf_iterator<char>());
      std::cout << "Raw header content : " << std::endl
                << mHRawContent << std::endl;
    } else
      std::cerr << "Unable to open header file '" << hLoc << "'!" << std::endl;

    fHeader.close();
  }

  bool parseHeader() {
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
    boost::regex methExpr("USERCC +(([\\w:]+) +(\\w+) *\\(([\\w, :]+)\\) *;)");
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
    mHProcessedContent = boost::regex_replace(mHProcessedContent, boost::regex("#include +\"rcobjectserver.hpp\""), "");
    mHProcessedContent = boost::regex_replace(mHProcessedContent, boost::regex("[:]? *public +RCObjectServer *"), "");
    mHProcessedContent = boost::regex_replace(mHProcessedContent, boost::regex("\\n\\n"), "\\n");
    // format header
    mHProcessedContent = boost::regex_replace(mHProcessedContent, boost::regex("\\n"), "\\\\\\\\n\\\\\\\\ \\n");

    std::cout << "Cleaned and formatted header :" << std::endl
              << mHProcessedContent << std::endl;

    return true;
  }

  void composeSource() {
    mSContent = rcosClassTemplate;
    
    // fill in ClassName, ClassInterface and virtual function declarations
    mSContent = boost::regex_replace(mSContent, boost::regex("<\\^<ObjectRegisterName>\\^>"), mServerClassName);
    mSContent = boost::regex_replace(mSContent, boost::regex("<\\^<ClassInterface>\\^>"), mHProcessedContent);
    
    // add methods and method calls
    mSContent = boost::regex_replace(mSContent, boost::regex("<\\^<VirtualHostedFunctionDeclarations>\\^>"), getVirtualClientFctDeclarations());
    mSContent = boost::regex_replace(mSContent, boost::regex("<\\^<ClientWebCallback>\\^>"), getClientWebCallCode());

    std::cout << "Composed source file :" << std::endl
              << mSContent;
  }
private:
  std::string mSContent;
  std::string mServerClassName;
  std::string mHRawContent;
  std::string mHProcessedContent;
  std::list<RCOSMethod> mServerMethods;

  std::string getClientWebCallCode() {
    std::string callCode = "";

    for(RCOSMethod met : mServerMethods) {
      callCode += met.getFormattedCallCode() + std::string("\\n");
    }

    return callCode;
  }

  std::string getVirtualClientFctDeclarations() {
    std::string formattedClientFcts = "";

    for(RCOSMethod met : mServerMethods)
      formattedClientFcts += met.getCompleteDeclaration() + std::string("\\n");

    return formattedClientFcts;
  }
};

int main(int argc, char *argv[]) {
  RCOSCreator rcosc(argv[1]);

  rcosc.parseHeader();
  rcosc.composeSource();
  //RCObjectServer os;

  //os.RegisterObject("127.0.0.1:8081", 93);

  return 0;
}
