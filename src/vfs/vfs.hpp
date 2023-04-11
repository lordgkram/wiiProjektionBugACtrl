#ifndef VFS_VFS_HPP_
#define VFS_VFS_HPP_

namespace embededResources {
    struct file_object {
        const unsigned char* data;
        const unsigned int length;
        const char* name;
    };
}

#endif /* VFS_VFS_HPP_ */
