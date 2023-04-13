#include "fontUtil.hpp"

uint16_t colors[8];
SDL_Surface *fontSurfaces[8];

void writeFontData(uint16_t *out, uint16_t color, uint16_t colorBg, uint32_t length, const unsigned char *inData, uint32_t pixelCount) {
    uint32_t o = 0;
    for(uint32_t i = 0; i < length; i++) {
        for(uint8_t e = 0; e < 8; e++, o++) {
            if(o < pixelCount)
                out[o] = ((inData[i] >> e) & 1) ? color : colorBg;
        }
    }
}

void loadFont() {
    colors[0] = 0x000F;
    colors[1] = 0x00FF;
    colors[2] = 0x0F0F;
    colors[3] = 0x0FFF;
    colors[4] = 0xF00F;
    colors[5] = 0xF0FF;
    colors[6] = 0xFF0F;
    colors[7] = 0xFFFF;
    const embededResources::file_object *fontFile = embededResources::util::getFile(embededResources::FILES_FONT_objects, embededResources::FILES_FONT_objects_length, "/font.bin");
    uint16_t *fontData = new uint16_t[charsPC];
    SDL_Surface *_fontSurface = SDL_CreateRGBSurfaceFrom(fontData, charsPW, charsPH, 16, charsPW * 2, 0xF000, 0x0F00, 0x00F0, 0x000F);

    for(uint8_t i = 0; i < 8; i++) {
        writeFontData(fontData, colors[i], 0x0000, fontFile->length, fontFile->data, charsPC);
        fontSurfaces[i] = SDL_DisplayFormat(_fontSurface);
    }

    SDL_FreeSurface(_fontSurface);
    delete[] fontData;
}

void deleteFont() {
    for(uint8_t i = 0; i < 8; i++)
        SDL_FreeSurface(fontSurfaces[i]);
}

void writeFont(SDL_Surface *to, uint16_t &x, uint16_t &y, uint16_t sx, uint8_t &scolor, char *text) {
    SDL_Rect *charRect = new SDL_Rect();
    charRect->w = charW;
    charRect->h = charH;
    SDL_Rect *toRect = new SDL_Rect();
    toRect->w = charW;
    toRect->h = charH;
    toRect->x = x;
    toRect->y = y;

    SDL_Surface *fontSurfaceCurr = fontSurfaces[scolor];

    uint16_t i = 0;
    while(text[i] != '\0') {
        if(text[i] == '\n') {
            x = sx;
            toRect->x = x;
            y += charH;
            toRect->y = y;
        } else if(text[i] >= '\xA0' && text[i] <= '\xA7') {
            scolor = text[i] - '\xA0';
            fontSurfaceCurr = fontSurfaces[scolor];
        } else {
            char tdp = text[i];
            if(tdp < charsS) tdp = charErr;
            if(tdp > charsC + charsS) tdp = charErr;
            uint8_t id = tdp - charsS;
            uint8_t chX = id % charsX;
            uint8_t chY = id / charsX;
            charRect->x = chX * charW;
            charRect->y = chY * charH;
            if(SDL_BlitSurface(fontSurfaceCurr, charRect, to, toRect) < 0) printf("BlitSurface error: %s\n", SDL_GetError());
            x += charW;
            toRect->x = x;
        }
        i++;
    }
    delete charRect;
    delete toRect;
}
