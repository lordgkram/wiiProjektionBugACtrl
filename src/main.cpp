
#include <SDL/SDL.h>
#include <SDL/SDL_joystick.h>
#include <SDL/SDL_timer.h>

#include "fontUtil.hpp"

#ifdef __wii__ // wii
#else /* __wii__ */
#endif /* __wii__ */

bool running = true;

constexpr int32_t mspf = 1000 / 20;
SDL_Color colors[256];

void *operator new[](size_t v) { return malloc(v); }
void operator delete[](void *v) { free(v); }
void *operator new(size_t v) { return malloc(v); }
void operator delete(void *v) { free(v); }

#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char *argv[]) {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == -1) {
        return 1;
    }

    SDL_Surface *screen = SDL_SetVideoMode(640, 480, 16, SDL_HWSURFACE);
    if(screen == nullptr) {
        SDL_Quit();
        return 2;
    }
    for(uint16_t i = 0; i < 256; i++){
        colors[i].r = i;
        colors[i].g = i;
        colors[i].b = i;
    }
    SDL_SetColors(screen, colors, 0, 256);

    loadFont();
    uint16_t wx = 0, wy = 0;
    wx = 0;
    wy = 0;
    writeFont(screen, wx, wy, "SDL JOYSTICK OUT V1\n");

    char *writeBuff = new char[512];
    snprintf(writeBuff, 512, "%i joysticks:\n" , SDL_NumJoysticks());
    writeFont(screen, wx, wy, writeBuff);
    for(int i = 0; i < SDL_NumJoysticks(); i++) {
        snprintf(writeBuff, 512, "  %s\n", SDL_JoystickName(i));
        writeFont(screen, wx, wy, writeBuff);
        SDL_Joystick *joystick = SDL_JoystickOpen(i);
        snprintf(writeBuff, 512, "    %i axes\n", SDL_JoystickNumAxes(joystick));
        writeFont(screen, wx, wy, writeBuff);
        snprintf(writeBuff, 512, "    %i buttons\n", SDL_JoystickNumButtons(joystick));
        writeFont(screen, wx, wy, writeBuff);
        snprintf(writeBuff, 512, "    %i balls\n", SDL_JoystickNumBalls(joystick));
        writeFont(screen, wx, wy, writeBuff);
        snprintf(writeBuff, 512, "    %i hats\n", SDL_JoystickNumHats(joystick));
        writeFont(screen, wx, wy, writeBuff);
        SDL_JoystickClose(joystick);
    }
    delete[] writeBuff;
    
    SDL_Event event;
    uint32_t startMs = SDL_GetTicks();
    while(running) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_QUIT:
                running = false;
                break;
            }
        }

        SDL_Flip(screen);

        // frame limit
        uint32_t endMs = SDL_GetTicks();
        int32_t delay = mspf - (endMs - startMs);
        if(delay > 0) SDL_Delay(delay);
        startMs = endMs;
    }

    // shutdown
    deleteFont();
    SDL_Quit();
    return 0;
}
