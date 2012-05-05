#include "methodinterface.hpp"

MethodInterface::MethodInterface(const std::string &completeFct) 
  : mCompleteFunctionDeclaration(completeFct) {
  
  boost::regex methExpr("(([\\w:]+) +(\\w+) *\\(([\\w, :]+)\\))");
  boost::match_results<std::string::const_iterator> captures;
  if(boost::regex_search(completeFct, captures, methExpr, boost::match_default)) {
    // (1) complete function declaration, (2) = return value, (3) = method name, (4) = method argument comma separated list
    mReturnType = parseTypeFromString(captures[2]);
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
  comSequence += "cJSON_AddStringToObject(" + jMethodHolder + ", \"returntype\", \"" + parseStringFromType(mReturnType) + "\");\n";
  for(id = 0; id < mArgTypes.size(); ++ id) {
    comSequence += "cJSON_AddItemToArray(" + jArgListHolder + ", cJSON_CreateString(\"" + parseStringFromType(mArgTypes.at(id)) + "\"));\n";
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
  mArgTypes.push_back(parseTypeFromString(argType));
}

std::string MethodInterface::parseStringFromType(ImplementedType type) {
  std::string dataType = "";

  if(type == INTEGER) dataType = "integer";
  else if(type == STRING) dataType = "string";
  else if(type == DOUBLE) dataType = "double";
  else if(type == BOOLEAN) dataType = "bool";
  else if(type == VOID) dataType = "void";

  return dataType;
}

ImplementedType MethodInterface::parseTypeFromString(const std::string &type) {
  ImplementedType recognizedType = UNKNOWN;

  if(type.find("int") != std::string::npos) recognizedType = INTEGER;
  else if(type.find("string") != std::string::npos) recognizedType = STRING;
  else if(type.find("double") != std::string::npos) recognizedType = DOUBLE;
  else if(type.find("bool") != std::string::npos) recognizedType = BOOLEAN;
  else if(type.find("void") != std::string::npos) recognizedType = VOID;

  return recognizedType;
}
