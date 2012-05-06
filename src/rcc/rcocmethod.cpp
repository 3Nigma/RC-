#include "rcocmethod.hpp"

const std::string rcocMethodTemplate = "virtual <^<MethodRetType>^> <^<MethodName>^>(<^<ArgList>^>) { \n"
  "  char errBuff[4096]; \n"
  "  char remoteResponse[4096]; \n"
  "  cJSON *rootReq = nullptr; \n"
  "  cJSON *rootRes = nullptr; \n"
  "  cJSON *retNode = nullptr; \n"
  "  CURL *objLink = curl_easy_init(); \n"
  "\n"
  "  if(objLink != nullptr) { \n"
  "    rootReq = cJSON_CreateObject(); \n"
  "    cJSON_AddStringToObject(rootReq, \"methodname\", \"<^<MethodName>^>\"); \n"
  "<^<JSONMethArgsInsertion>^> \n"
  "\n"
  "    curl_easy_setopt(objLink, CURLOPT_ERRORBUFFER, errBuff); \n"
  "    curl_easy_setopt(objLink, CURLOPT_URL, <^<ObjectServerAddress>^>.c_str()); \n"
  "    curl_easy_setopt(objLink, CURLOPT_POSTFIELDS, cJSON_Print(rootReq)); \n" 
  "    curl_easy_setopt(objLink, CURLOPT_WRITEFUNCTION, <^<LocalObjCallback>^>); \n"
  "    curl_easy_setopt(objLink, CURLOPT_WRITEDATA, remoteResponse); \n"
  "\n"
  "    if(curl_easy_perform(objLink) != 0) { \n"
  "       std::cerr << \"Unable to invoke remote object method! : \" << errBuff << std::endl; \n"
  "     } else {\n"
  "       // method request got through succesfully, process the response \n"
  "       if((rootRes = cJSON_Parse(remoteResponse)) != 0 ) { \n"
  "         retNode = cJSON_GetObjectItem(rootRes, \"returnvalue\"); \n"
  "         if(nullptr != retNode) { \n"
  "           return retNode-><^<cJSONRetType>^>;\n"
  "         } \n"
  "         cJSON_Delete(rootRes); \n"
  "       } \n"
  "     } \n"
  "\n"
  "    curl_easy_cleanup(objLink); \n"
  "  }\n"
  "  <^<DefaultReturn>^>"
  "}"; 

RCOCMethod::RCOCMethod(cJSON *methNode)
  : MethodInterface(methNode) {

}

std::string RCOCMethod::getInvocationCode(const std::string &objServerAddress, const std::string &localObjCallback) {
  std::string methCode = rcocMethodTemplate;
  std::string cJSONPendingArgs = "";
  std::string completeArgName;
  std::string argList = "";
  int argIndex = 0;
  
  methCode = boost::regex_replace(methCode, boost::regex("<\\^<MethodRetType>\\^>"), mReturnType.st);
  methCode = boost::regex_replace(methCode, boost::regex("<\\^<MethodName>\\^>"), mName);
  methCode = boost::regex_replace(methCode, boost::regex("<\\^<ObjectServerAddress>\\^>"), objServerAddress);
  methCode = boost::regex_replace(methCode, boost::regex("<\\^<LocalObjCallback>\\^>"), localObjCallback);
  
  // build argument list and add JSON method arguments to remote call root
  for(argIndex = 0; argIndex < mArgTypes.size(); ++argIndex) {
    completeArgName = "arg" + std::to_string(argIndex);
    argList += mArgTypes[argIndex].st + " " + completeArgName;
    if(argIndex != mArgTypes.size() - 1) {
      argList += ", ";
    }

    switch(mArgTypes[argIndex].en) {
    case STRING:
      cJSONPendingArgs += "cJSON_AddStringToObject(rootReq, \"" + completeArgName + "\", " + completeArgName + ".c_str()); \n";
      break;
    case INTEGER:
    case DOUBLE:
      cJSONPendingArgs += "cJSON_AddNumberToObject(rootReq, \"" + completeArgName + "\", " + completeArgName + "); \n";
      break;
    case BOOLEAN:
      //TODO : Implement this
      cJSONPendingArgs += "/* Boolean arguments not yet implemented! */ \n";
      break;
    default: // unknown type
      break;  
    }
  }
  methCode = boost::regex_replace(methCode, boost::regex("<\\^<ArgList>\\^>"), argList);
  methCode = boost::regex_replace(methCode, boost::regex("<\\^<JSONMethArgsInsertion>\\^>"), cJSONPendingArgs);

  // process the return value
  // <^<cJSONRetType>^>

  // add default return value (if it the case)
  switch(mReturnType.en) {
  case STRING:
    methCode = boost::regex_replace(methCode, boost::regex("<\\^<cJSONRetType>\\^>"), "valuestring");
    methCode = boost::regex_replace(methCode, boost::regex("<\\^<DefaultReturn>\\^>"), "return \"Something went wrong durring the method call!\"; \n");
    break;
  case INTEGER:
    methCode = boost::regex_replace(methCode, boost::regex("<\\^<cJSONRetType>\\^>"), "valueint");
    methCode = boost::regex_replace(methCode, boost::regex("<\\^<DefaultReturn>\\^>"), "return 0; \n");
    break;
  case DOUBLE:
    methCode = boost::regex_replace(methCode, boost::regex("<\\^<cJSONRetType>\\^>"), "valuedouble");
    methCode = boost::regex_replace(methCode, boost::regex("<\\^<DefaultReturn>\\^>"), "return 0.0; \n");
    break;
  case BOOLEAN:
    methCode = boost::regex_replace(methCode, boost::regex("<\\^<cJSONRetType>\\^>"), "false");  // TODO: there is no means of getting the boolean value from a cJSON data structure. Search for alternatives!
    methCode = boost::regex_replace(methCode, boost::regex("<\\^<DefaultReturn>\\^>"), "return false; \n");
    break;
  case VOID:
    // this won't be the case : a void parameter isn't allowed in C++
    break;
  default: // unknown type
    break;  
  }

  return methCode;
}
