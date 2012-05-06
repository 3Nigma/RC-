#include "rcocmethod.hpp"

const std::string rcocMethodTemplate = "virtual <^<MethodRetType>^> <^<MethodName>^>(<^<ArgList>^>) {\n"
  
  "}"; 

RCOCMethod::RCOCMethod(cJSON *methNode)
  : MethodInterface(methNode) {

}

std::string RCOCMethod::getInvocationCode() {
  std::string methCode = rcocMethodTemplate;
  std::string argList = "";
  int argIndex = 0;
  
  methCode = boost::regex_replace(methCode, boost::regex("<\\^<MethodRetType>\\^>"), mReturnType.st);
  methCode = boost::regex_replace(methCode, boost::regex("<\\^<MethodName>\\^>"), mName);
  // build argument list
  for(argIndex = 0; argIndex < mArgTypes.size(); ++argIndex) {
    argList += mArgTypes[argIndex].st + " arg" + std::to_string(argIndex);
    if(argIndex != mArgTypes.size() - 1) {
      argList += ", ";
    }
  }
  methCode = boost::regex_replace(methCode, boost::regex("<\\^<ArgList>\\^>"), argList);

  return methCode;
}
