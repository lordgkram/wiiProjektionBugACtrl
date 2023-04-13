#ifndef FONT_UTIL_HPP_
#define FONT_UTIL_HPP_

#include "SDL/SDL.h"

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

extern uint16_t colors[8];
extern SDL_Surface *fontSurfaces[8];

void loadFont();
void deleteFont();
void writeFont(SDL_Surface *to, uint16_t &x, uint16_t &y, uint16_t sx, uint8_t &scolor, char *text);

#endif /* FONT_UTIL_HPP_ */
