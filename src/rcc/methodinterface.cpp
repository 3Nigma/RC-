#include "methodinterface.hpp"

MethodInterface::MethodInterface(cJSON *methNode) {
  cJSON *methRetType = nullptr;
  cJSON *methArguments = nullptr;
  cJSON *argType = nullptr;
  int argCnt = 0;
  int argIndex = 0;

  if(nullptr != methNode) {
    mName = methNode->string;
    methRetType = cJSON_GetObjectItem(methNode, "returntype");
    if(nullptr != methRetType) {
      mReturnType = getTypeFromString(methRetType->valuestring);
    }
    methArguments = cJSON_GetObjectItem(methNode, "arguments");
    if(nullptr != methArguments) {
      argCnt = cJSON_GetArraySize(methArguments);
      for(argIndex = 0; argIndex < argCnt; ++ argIndex) {
	argType = cJSON_GetArrayItem(methArguments, argIndex);
	pushArgument(argType->valuestring);
      }
    }
  }
}

MethodInterface::MethodInterface(const std::string &completeFct) 
  : mCompleteFunctionDeclaration(completeFct) {
  
  boost::regex methExpr("(([\\w:]+) +(\\w+) *\\(([\\w, :]+)\\))");
  boost::match_results<std::string::const_iterator> captures;
  if(boost::regex_search(completeFct, captures, methExpr, boost::match_default)) {
    // (1) complete function declaration, (2) = return value, (3) = method name, (4) = method argument comma separated list
    mReturnType = getTypeFromString(captures[2]);
    mName = captures[3];
    std::string argList = captures[4];
    
    // get method arguments
    boost::regex argExpr("((?:unsigned +)?int|(?:std::)?string|double|bool|void)");
    boost::sregex_iterator m3(argList.begin(), argList.end(), argExpr);
    boost::sregex_iterator m4;
    std::for_each(m3, m4, [&](const boost::smatch &ma)->bool {
	pushArgument(ma[1]);
      });
  }
}

std::string MethodInterface::getJSONIntegrationInstructions(const std::string &jRoot, const std::string &jMethodHolder, const std::string &jArgListHolder) {
  std::string comSequence;
  int id;

  comSequence = jMethodHolder + " = cJSON_CreateObject();\n";
  comSequence += jArgListHolder + " = cJSON_CreateArray();\n";
  comSequence += "cJSON_AddStringToObject(" + jMethodHolder + ", \"returntype\", \"" + getStringFromType(mReturnType) + "\");\n";
  for(id = 0; id < mArgTypes.size(); ++ id) {
    comSequence += "cJSON_AddItemToArray(" + jArgListHolder + ", cJSON_CreateString(\"" + getStringFromType(mArgTypes.at(id)) + "\"));\n";
  }
  comSequence += "cJSON_AddItemToObject(" + jMethodHolder + ", \"arguments\", " + jArgListHolder + ");\n"; 
  comSequence += "cJSON_AddItemToObject(" + jRoot + ", \"" + mName + "\", " + jMethodHolder + ");"; 

  return comSequence;
}

std::string MethodInterface::getSimpleDeclaration() {
  return mCompleteFunctionDeclaration + std::string(";");
}

std::string MethodInterface::getPureVirtualDeclaration() {
  return std::string("virtual ") + mCompleteFunctionDeclaration + std::string(" = 0;");
}

void MethodInterface::pushArgument(const std::string &argType) {
  mArgTypes.push_back(getTypeFromString(argType));
}

std::string MethodInterface::getStringFromType(VarType typ) {
  return typ.st;
}

VarType MethodInterface::getTypeFromString(const std::string &origType) {
  VarType recognizedType;

  recognizedType.st = origType;
  if(origType.find("int") != std::string::npos) recognizedType.en = INTEGER;
  else if(origType.find("string") != std::string::npos) recognizedType.en = STRING;
  else if(origType.find("double") != std::string::npos) recognizedType.en = DOUBLE;
  else if(origType.find("bool") != std::string::npos) recognizedType.en = BOOLEAN;
  else if(origType.find("void") != std::string::npos) recognizedType.en = VOID;

  return recognizedType;
}
