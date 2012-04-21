#ifndef _EXCEPTIONS_HPP_
#define _EXCEPTIONS_HPP_

#include <exception>

class BadJSONFormatException : public std::exception { };
class ArgumentIndexOutOfBounds : public std::exception { };

#endif
