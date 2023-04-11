#ifndef VFS_VFSUTIL_HPP_
#define VFS_VFSUTIL_HPP_

#include <string_view>
#include <vector>

#include "vfs.hpp"

namespace embededResources {
    namespace util {
        std::vector<std::string_view> lsdir(const file_object* list, int length, const char* path);
        const file_object* getFile(const file_object* list, int length, const char* path);
    }
}

#endif /* VFS_VFSUTIL_HPP_ */
