
#include <cstdio>
#include <string>
#include <vector>

std::vector<long> fileLengths;

int writeCppFile(int argc, char *argv[], std::FILE* codeFile) {
    std::fprintf(codeFile, "%s%s%s", R"(
#include ")", argv[3], R"(.hpp"
)");
    // const unsigned char embededResources::dataArrays::RESOURCE_ELEM_data[] = {};
    std::FILE* inFile;
    unsigned char buff[512];
    size_t read;
    char outline[] = "    0x??, 0x??, 0x??, 0x??, 0x??, 0x??, 0x??, 0x??, 0x??, 0x??, 0x??, 0x??,\n";
    constexpr auto outline_indent = 4;
    constexpr auto outline_elems = (sizeof(outline) - 1 - outline_indent) / 6;
    for(int argi = 4; argi + 2 < argc; argi += 3) {
        std::fprintf(codeFile, "%s%s%s%s%s", R"(
const unsigned char embededResources::dataArrays::)", argv[2], R"(_)", argv[argi + 1], R"(_data[] = {
)");
        // read file
        inFile = std::fopen(argv[argi], "rb");
        unsigned idx = 0;
        do {
            // TODO: fix potential idx accounting error
            constexpr auto hexify = [](char *out, const unsigned char *in, size_t count){
                for (size_t i = 0; i < count; ++i) {
                    auto byte = in[i];
                    auto low = byte & 0xf;
                    out[2 + i * 6] = '0' + (byte >> 4) + ((byte >= 0xa0) ? 'a' - '0' - 10 : 0);
                    out[3 + i * 6] = '0' + low + ((low >= 0xa) ? 'a' - '0' - 10 : 0);
                }
            };
            read = std::fread(buff, 1, sizeof(buff), inFile);
            size_t printed = 0;
            if (idx != 0) {
                auto initial_pos = outline_indent + 6 * idx;
                auto count = std::min(read, outline_elems - idx);
                hexify(outline + initial_pos, buff + printed, count);
                std::fwrite(outline + initial_pos - 1, 1, (int)(count * 6 + (count != read)), codeFile);
                printed = count;
                idx += count;
            }
            if (read > printed) {
                auto count = read - printed;
                while (count > outline_elems) {
                    hexify(outline + outline_indent, buff + printed, outline_elems);
                    std::fwrite(outline, 1, (int)(sizeof(outline) - 1), codeFile);
                    printed += outline_elems;
                    count -= outline_elems;
                }
                hexify(outline + outline_indent, buff + printed, count);
                std::fwrite(outline, 1, (int)(outline_indent - 1 + 6 * count), codeFile);
                idx = count;
            }
        } while(read > 0);
        fileLengths.emplace_back(std::ftell(inFile));
        std::fclose(inFile);

        std::fprintf(codeFile, "\n};");
    }
    std::fprintf(codeFile, "\n");
    return 0;
}

int writeHppFile(int argc, char *argv[], std::FILE* codeFile) {
    std::fprintf(codeFile, "%s%s%s%s%s", R"(
#ifndef )", argv[2], R"(_HPP_
#define )", argv[2], R"(_HPP_

#include "vfs/vfs.hpp"

namespace embededResources {
    namespace dataArrays {)");
    // extern const unsigned char RESOURCE_ELEM_data[];
    for(int argi = 4; argi + 2 < argc; argi += 3) {
        std::fprintf(codeFile, "%s%s%s%s%s", R"(
        extern const unsigned char )", argv[2], R"(_)", argv[argi + 1], R"(_data[];)");
    }
    std::fprintf(codeFile, "%s%s%s", R"(
    }
    constexpr file_object )", argv[2], R"(_objects[] {)");
    // {.data = dataArrays::RESOURCE_ELEM_data, .length = sizeof(dataArrays::RESOURCE_ELEM_data), .name = "ELEM"}
    for(int argi = 4, i = 0; argi + 2 < argc; argi += 3, i++) {
        std::fprintf(codeFile, "%s%s%s%s%s%ld%s%s%s", R"(
        {.data = dataArrays::)", argv[2], R"(_)", argv[argi + 1], R"(_data, .length = )", fileLengths[i], R"(, .name = ")", argv[argi + 2], R"("},)");
    }
    std::fprintf(codeFile, "%s%s%s%s%s%s%s", R"(
    };

    constexpr auto )", argv[2], R"(_objects_length = sizeof()", argv[2], R"(_objects) / sizeof(file_object);
}

#endif /* )", argv[2], R"(_HPP_ */
)");
    return 0;
}

int main(int argc, char *argv[]) {
    if(argc < 7) {
        std::printf("USAGE: %s <basepath> <header_guard_name> <filebasename> <file0> <codename0> <internalPath0> [<file1> <codename1> <internalPath1>...]\n", argc ? argv[0] : "cmd");
        return 1;
    }
    std::string tmp;
    auto len = std::char_traits<char>::length(argv[1]);
    tmp.reserve(len + 4);
    tmp += std::string_view{argv[1], len};
    tmp += ".cpp";

    fileLengths.reserve((argc - 4) / 3);

    std::FILE* codeFile = std::fopen(tmp.c_str(), "w");
    int rtn = writeCppFile(argc, argv, codeFile);
    if(rtn) return rtn;
    std::fclose(codeFile);

    tmp[len + 1] = 'h';
    codeFile = std::fopen(tmp.c_str(), "w");
    rtn = writeHppFile(argc, argv, codeFile);
    if(rtn) return rtn;
    std::fclose(codeFile);
}
