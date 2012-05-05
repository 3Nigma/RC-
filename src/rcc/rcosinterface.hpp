#ifndef _RCOSINTERFACE_HPP_
#define _RCOSINTERFACE_HPP_

#include <iostream>
#include <fstream>

class RCOSInterface {
public :
  RCOSInterface(const std::string &codeFile);

  virtual bool parseCode() = 0;
  virtual void composeSource() = 0;
  virtual void saveRCOS(std::string dir);

  std::string getFileSaveName();
protected :
  std::string mSContent;
  std::string mServerClassName;
  std::string mHRawContent;
  std::string mHProcessedContent;
private:
};

#endif
