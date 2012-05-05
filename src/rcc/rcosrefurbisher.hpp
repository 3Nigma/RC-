#ifndef _RCOSREFURBISHER_HPP_
#define _RCOSREFURBISHER_HPP_

#include "rcosinterface.hpp"
#include <boost/regex.hpp>
#include <cJSON/cJSON.h>
#include <curl/curl.h> 

class RCOSRefurbisher : public RCOSInterface {
public:
  RCOSRefurbisher(const std::string &codeFile);

  virtual bool parseCode();
  virtual void composeSource();

  void setRemoteHeaderContent(const std::string &data);
protected:
  std::string mNamingServiceAddress;
  std::string mRawRemoteHeaderContent;
};

#endif
