#include "hash.h"
#include <functional>
#ifdef HASH_IS_NUMERIC
std::hash<std::string> hash_str_fn;
hash_t utils::Hash(const char* s) {
    std::string str(s);
    return utils::Hash(str);
}
hash_t utils::Hash(const std::string& s)
{
    return hash_str_fn(s);
}
#else
hash_t utils::Hash(const char* s) {
    std::string str(s);
    return utils::Hash(str);
}
hash_t utils::Hash(const std::string& s)
{
    return s;
}
#endif