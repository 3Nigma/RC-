#ifndef _METHODINTERFACE_HPP_
#define _METHODINTERFACE_HPP_

#include <iostream>
#include <boost/regex.hpp>

enum ImplementedType {UNKNOWN, STRING, INTEGER, DOUBLE, BOOLEAN, VOID};

class MethodInterface {
public:
  MethodInterface(const std::string &completeFct);

  std::string getJSONIntegrationInstructions(const std::string &jRoot, const std::string &jMethodHolder, const std::string &jArgListHolder);
  std::string getSimpleDeclaration();
  std::string getPureVirtualDeclaration();

  void pushArgument(const std::string &argType);
protected:
  ImplementedType mReturnType;
  std::string mName;
  std::string mCompleteFunctionDeclaration;
  std::vector<ImplementedType> mArgTypes;

  ImplementedType parseTypeFromString(const std::string &type);
  std::string parseStringFromType(ImplementedType type);
};

#endif
