#include "rcosmethod.hpp"

std::string rcosMethodTemplate = "if(methodName == \"<^<LocalMethodName>^>\") {\n\
  <^<OtherOps>^>\
  cJSON_Add<^<cJSON_LocalMethodReturnType>^>ToObject(replyroot, \"returnvalue\", <^<LocalFunctionCallResult>^>); \n\
}";

RCOSMethod::RCOSMethod(const std::string &completeFct)
  : MethodInterface(completeFct) {
  
}

std::string RCOSMethod::getFormattedCallCode() {
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
  methodCall += std::string("rcos->") + mName + std::string("(");
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
  if(mReturnType == STRING) methodCall += std::string(".c_str()");

  if(mReturnType == VOID) {
    callCode = boost::regex_replace(callCode, boost::regex("<\\^<OtherOps>\\^>"), methodCall);
    callCode = boost::regex_replace(callCode, boost::regex("<\\^<LocalFunctionCallResult>\\^>"), "(void) No return");
  } else {
    callCode = boost::regex_replace(callCode, boost::regex("<\\^<OtherOps>\\^>"), "");
    callCode = boost::regex_replace(callCode, boost::regex("<\\^<LocalFunctionCallResult>\\^>"), methodCall);
  }

  return callCode;
}
