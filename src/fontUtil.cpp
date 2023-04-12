#include "fontUtil.hpp"

uint16_t *fontData;
SDL_Surface *fontSurface;

void loadFont() {
    const embededResources::file_object *fontFile = embededResources::util::getFile(embededResources::FILES_FONT_objects, embededResources::FILES_FONT_objects_length, "/font.bin");
    fontData = new uint16_t[charsPC];

    uint32_t o = 0;
    for(uint32_t i = 0; i < fontFile->length; i++) {
        for(uint8_t e = 0; e < 8; e++, o++) {
            if(o < charsPC)
                fontData[o] = ((fontFile->data[i] >> e) & 1) ? color : colorNone;
        }
    }

    SDL_Surface *_fontSurface = SDL_CreateRGBSurfaceFrom(fontData, charsPW, charsPH, 16, charsPW * 2, 0xF000, 0x0F00, 0x00F0, 0x000F);
    fontSurface = SDL_DisplayFormat(_fontSurface);
    SDL_FreeSurface(_fontSurface);
    delete[] fontData;
}

void deleteFont() {
    SDL_FreeSurface(fontSurface);
}

void writeFont(SDL_Surface *to, uint16_t &x, uint16_t &y, char *text) {
    SDL_Rect *charRect = new SDL_Rect();
    charRect->w = charW;
    charRect->h = charH;
    SDL_Rect *toRect = new SDL_Rect();
    toRect->w = charW;
    toRect->h = charH;
    toRect->x = x;
    toRect->y = y;

    uint16_t i = 0;
    while(text[i] != '\0') {
        if(text[i] == '\n') {
            toRect->x = x;
            y += charH;
            toRect->y = y;
        } else {
            char tdp = text[i];
            if(tdp < charsS) tdp = charErr;
            if(tdp > charsC + charsS) tdp = charErr;
            uint8_t id = tdp - charsS;
            uint8_t chX = id % charsX;
            uint8_t chY = id / charsX;
            charRect->x = chX * charW;
            charRect->y = chY * charH;
            if(SDL_BlitSurface(fontSurface, charRect, to, toRect) < 0) printf("BlitSurface error: %s\n", SDL_GetError());
            toRect->x += charW;
        }
        i++;
    }
    delete charRect;
    delete toRect;
}
