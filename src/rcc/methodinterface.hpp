#ifndef _METHODINTERFACE_HPP_
#define _METHODINTERFACE_HPP_

#include <iostream>
#include <boost/regex.hpp>

#include "cJSON/cJSON.h"

enum ImplementedType {UNKNOWN, STRING, INTEGER, DOUBLE, BOOLEAN, VOID};

typedef struct {
  ImplementedType en;
  std::string st;
} VarType;

class MethodInterface {
public:
  MethodInterface(cJSON *methNode);
  MethodInterface(const std::string &completeFct);

  std::string getJSONIntegrationInstructions(const std::string &jRoot, const std::string &jMethodHolder, const std::string &jArgListHolder);
  std::string getSimpleDeclaration();
  std::string getPureVirtualDeclaration();

  void pushArgument(const std::string &argType);
protected:
  VarType mReturnType;
  std::string mName;
  std::string mCompleteFunctionDeclaration;
  std::vector<VarType> mArgTypes;

  VarType getTypeFromString(const std::string &typ);
  std::string getStringFromType(VarType typ);
};

#endif
