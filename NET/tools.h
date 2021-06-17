#ifndef TOOLS_DGDB
#define TOOLS_DGDB

#include <string>

inline std::string trim(std::string& str)
{
    str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
    str.erase(str.find_last_not_of(' ')+1);         //surfixing spaces
    return str;
}

const std::string fixToBytes(const std::string &input, const std::size_t& n_bytes) {
  if(input.length() > n_bytes) {
    exit(EXIT_FAILURE);
  }
  return std::string().assign(n_bytes - input.length(), '0') + input;
}

#endif