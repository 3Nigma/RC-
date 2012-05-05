#ifndef _RCOSMETHOD_HPP_
#define _RCOSMETHOD_HPP_

#include <iostream>
#include <list>
#include <boost/regex.hpp>

enum ImplementedType {UNKNOWN, STRING, INTEGER, DOUBLE, BOOLEAN, VOID};

class RCOSMethod {
public:
  RCOSMethod(const std::string &retType, const std::string &name, const std::string &function);

  std::string getSimpleDeclaration();
  std::string getCompleteDeclaration();
  std::string getFormattedCallCode();

  void pushArgument(const std::string &argType);
private:
  ImplementedType mReturnType;
  std::string mName;
  std::string mCompleteFunctionDeclaration;
  std::list<ImplementedType> mArgTypes;

  ImplementedType parseTypeFromString(const std::string &type);
};

#endif
