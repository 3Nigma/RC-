#ifndef _MATHSERVER_HPP_
#define _MATHSERVER_HPP_

#include <iostream>
#include <cmath>
#include "rcos_MathServer.cpp"

class MathServer : public RCObjectServer {
public:
  USERCC double do_add(double a, double b);
  USERCC double do_sqr(double a); 
};

#endif
