#ifndef _RCOSMETHOD_HPP_
#define _RCOSMETHOD_HPP_

#include <iostream>
#include <vector>
#include <boost/regex.hpp>

enum ImplementedType {UNKNOWN, STRING, INTEGER, DOUBLE, BOOLEAN, VOID};

class RCOSMethod {
public:
  RCOSMethod(const std::string &retType, const std::string &name, const std::string &function);

  std::string getJSONIntegrationInstructions(const std::string &jRoot, const std::string &jMethodHolder, const std::string &jArgListHolder);
  std::string getSimpleDeclaration();
  std::string getPureVirtualDeclaration();
  std::string getFormattedCallCode();

  void pushArgument(const std::string &argType);
private:
  ImplementedType mReturnType;
  std::string mName;
  std::string mCompleteFunctionDeclaration;
  std::vector<ImplementedType> mArgTypes;

  ImplementedType parseTypeFromString(const std::string &type);
  std::string parseStringFromType(ImplementedType type);
};

#endif
