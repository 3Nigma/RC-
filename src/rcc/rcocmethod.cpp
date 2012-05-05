#include "rcocmethod.hpp"

const std::string rcocMethodTemplate = "virtual <^<MethodRetType>^> <^<MethodName>^>(<^<ArgList>^>) {\n"
  "}"; 

RCOCMethod::RCOCMethod(cJSON *methNode)
  : MethodInterface(methNode) {

}

std::string RCOCMethod::getInvocationCode() {
  std::string methCode = rcocMethodTemplate;

  methCode = boost::regex_replace(methCode, boost::regex("<\\^<MethodRetType>\\^>"), mReturnType.st);
  methCode = boost::regex_replace(methCode, boost::regex("<\\^<MethodName>\\^>"), mName);

  return methCode;
}
