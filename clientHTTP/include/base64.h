#pragma once

#include <string>

/* =================================== Public API's =================================== */
#ifdef __cplusplus    // If used by C++ code,

std::string base64_encode(unsigned char const*, unsigned int len);
std::string base64_decode(std::string const& s);

#endif
