#include <cstdio>
#include <cinttypes>
#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(int argc, char const *argv[]) {
    if(argc < 3) {
        std::printf("USAGE: %s <in> <out>\n", argc ? argv[0] : "cmd");
        return 0;
    }

    int x, y, n;
    unsigned char *data = stbi_load(argv[1], &x, &y, &n, 0);
    unsigned length = x * y;
    unsigned dlength = length * n;
    int o = n - 1;

    unsigned outSize = (length + 7) / 8;
    uint8_t *out = (uint8_t *)malloc(outSize);

    uint8_t tmp = 0;
    uint8_t tmpPos = 0;
    unsigned outPos = 0;

    for(unsigned i = 0; i < dlength; i += n) {
        if(data[i + o]) tmp |= 1 << tmpPos;
        tmpPos++;
        if(tmpPos > 7) {
            tmpPos = 0;
            out[outPos++] = tmp;
            tmp = 0;
        }
    }
    if(tmpPos) out[outPos++] = tmp;

    stbi_image_free(data);

    std::FILE *outFile = std::fopen(argv[2], "w");
    std::fwrite(out, sizeof(uint8_t), outSize, outFile);
    free(out);
    std::fclose(outFile);

    return 0;
}
