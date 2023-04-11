#ifndef FONT_UTIL_HPP_
#define FONT_UTIL_HPP_

#include "SDL/SDL.h"

#include "files/font.hpp"
#include "vfs/vfsUtil.hpp"

constexpr unsigned charW = 8;
constexpr unsigned charH = 8;
constexpr unsigned charsX = 16;
constexpr unsigned charsY = 16;
constexpr unsigned charsS = 0;
constexpr unsigned charsC = charsX * charsY;
constexpr unsigned charsPW = charW * charsX;
constexpr unsigned charsPH = charH * charsY;
constexpr unsigned charsPC = charsPW * charsPH;
constexpr char charErr = '?';

constexpr uint16_t color = 0xFFFF;
constexpr uint16_t colorNone = 0x0000;

extern uint16_t *fontData;
extern SDL_Surface *fontSurface;

void loadFont();
void deleteFont();
void writeFont(SDL_Surface *to, uint16_t &x, uint16_t &y, char *text);

#endif /* FONT_UTIL_HPP_ */
