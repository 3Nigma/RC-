#include "rcosmethod.hpp"

std::string rcosMethodTemplate = "if(methodName == \"<^<LocalMethodName>^>\") {\n\
  <^<OtherOps>^>\
  cJSON_Add<^<cJSON_LocalMethodReturnType>^>ToObject(replyroot, \"returnvalue\", <^<LocalFunctionCallResult>^>); \n\
}";

RCOSMethod::RCOSMethod(const std::string &retType, const std::string &name, const std::string &function)
  : mName(name), mCompleteFunctionDeclaration(function) {
  mReturnType = parseTypeFromString(retType);
}

std::string RCOSMethod::getJSONIntegrationInstructions(const std::string &jRoot, const std::string &jMethodHolder, const std::string &jArgListHolder) {
  std::string comSequence;
  int id;

  comSequence = jMethodHolder + " = cJSON_CreateObject();\n";
  comSequence += jArgListHolder + " = cJSON_CreateArray();\n";
  comSequence += "cJSON_AddStringToObject(" + jMethodHolder + ", \"returntype\", \"" + parseStringFromType(mReturnType) + "\");\n";
  for(id = 0; id < mArgTypes.size(); ++ id) {
    comSequence += "cJSON_AddItemToArray(" + jArgListHolder + ", cJSON_CreateString(\"" + parseStringFromType(mArgTypes.at(id)) + "\"));\n";
  }
  comSequence += "cJSON_AddItemToObject(" + jMethodHolder + ", \"arguments\", " + jArgListHolder + ");\n"; 
  comSequence += "cJSON_AddItemToObject(" + jRoot + ", \"" + mName + "\", " + jMethodHolder + ");\n"; 

  return comSequence;
}

std::string RCOSMethod::getSimpleDeclaration() {
  return mCompleteFunctionDeclaration + std::string(";");
}

std::string RCOSMethod::getPureVirtualDeclaration() {
  return std::string("virtual ") + mCompleteFunctionDeclaration + std::string(" = 0;");
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

void RCOSMethod::pushArgument(const std::string &argType) {
  mArgTypes.push_back(parseTypeFromString(argType));
}

std::string RCOSMethod::parseStringFromType(ImplementedType type) {
  std::string dataType = "";

  if(type == INTEGER) dataType = "integer";
  else if(type == STRING) dataType = "string";
  else if(type == DOUBLE) dataType = "double";
  else if(type == BOOLEAN) dataType = "bool";
  else if(type == VOID) dataType = "void";

  return dataType;
}

ImplementedType RCOSMethod::parseTypeFromString(const std::string &type) {
  ImplementedType recognizedType = UNKNOWN;

  if(type.find("int") != std::string::npos) recognizedType = INTEGER;
  else if(type.find("string") != std::string::npos) recognizedType = STRING;
  else if(type.find("double") != std::string::npos) recognizedType = DOUBLE;
  else if(type.find("bool") != std::string::npos) recognizedType = BOOLEAN;
  else if(type.find("void") != std::string::npos) recognizedType = VOID;

  return recognizedType;
}
