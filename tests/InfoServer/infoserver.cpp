#include "infoserver.hpp"

std::string InfoServer::getRoadInfo(int roadID) {
  std::string roadInfo;

  switch(roadID) {
  case 0:
    roadInfo = "liber";
    break;
  case 1:
    roadInfo = "aglomerat";
    break;
  default:
    roadInfo = "necunoscut";
    break; 
  }

  return roadInfo;
}

double InfoServer::getTemp(std::string cityName) {
  double temp = 0.0;

  if(cityName == "Timisoara")
    temp = 27.0;

  return temp;
}
