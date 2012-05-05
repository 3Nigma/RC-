#ifndef _RCOSREFURBISHER_HPP_
#define _RCOSREFURBISHER_HPP_

#include <boost/regex.hpp>
#include <cJSON/cJSON.h>
#include <curl/curl.h> 

#include "rcosinterface.hpp"
#include "rcocmethod.hpp"

class RCOSRefurbisher : public RCOSInterface {
public:
  RCOSRefurbisher(const std::string &codeFile);

  virtual bool parseCode();
  virtual void composeSource();

  void storeJSONMethodNode(cJSON *methRoot);
protected:
  std::string mNamingServiceAddress;
  std::list<RCOCMethod> mServerMethods;

  std::string getServerMethodsCode();
};

#endif
