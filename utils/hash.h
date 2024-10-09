#pragma once
#include <string>
#ifdef HASH_IS_NUMERIC
typedef size_t hash_t;
namespace utils {
    hash_t Hash(const std::string& s);
    hash_t Hash(const char* s);
}
#else
typedef std::string hash_t;
namespace utils {
    hash_t Hash(const std::string& s);
    hash_t Hash(const char* s);
}
#endif