#pragma once
#include <stdexcept>
#include <string>
class TaucsException : public std::runtime_error{
public:
    TaucsException(std::string str) : std::runtime_error(str){}
};
