#ifndef _INFOSERVER_HPP_
#define _INFOSERVER_HPP_

#include <iostream>
#include <string>
#include "rcos_InfoServer.cpp"

class InfoServer : public RCObjectServer {
public:
  USERCC std::string getRoadInfo(int roadID);
  USERCC double getTemp(std::string cityName);
};

#endif
