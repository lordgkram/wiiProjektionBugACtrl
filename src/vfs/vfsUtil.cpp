#include "vfsUtil.hpp"

#include <algorithm>
#include <string.h>

std::vector<std::string_view> embededResources::util::lsdir(const file_object* list, int length, const char* path) {
    std::vector<std::string_view> out;
    std::size_t pathlen = strlen(path);
    for(int i = 0; i < length; i++) {
        if(strlen(list[i].name) < pathlen) continue;
        if(memcmp(list[i].name, path, pathlen) != 0) continue;
        const char* firstS = strchr(list[i].name + pathlen, '/');
        if(firstS == nullptr) {
            // file
            out.emplace_back(list[i].name);
        } else {
            // dir
            std::ptrdiff_t rellen = (firstS + 1) - list[i].name;
            out.emplace_back(list[i].name, rellen);
        }
    }
    // sort out by length and then lexicographically
    std::sort(out.begin(), out.end(), [](const std::string_view &a, const std::string_view &b){
        if (a.length() != b.length())
            return a.length() < b.length();
        else
            return a < b;
    });
    // remove duplicates
    if (!out.empty()) {
        std::string_view last = *out.begin();
        out.erase(std::remove_if(++out.begin(), out.end(), [&](const std::string_view &e){
            auto tmp = last;
            last = e;
            return tmp == e;
        }), out.end());
    }
    return out;
}

const embededResources::file_object* embededResources::util::getFile(const file_object* list, int length, const char* path) {
    for(int i = 0; i < length; i++) if(strcmp(list[i].name, path) == 0) return list + i;
    return nullptr;
}
