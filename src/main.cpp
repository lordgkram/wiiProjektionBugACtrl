
#include <SDL/SDL.h>
#include <SDL/SDL_joystick.h>
#include <SDL/SDL_timer.h>

#include "fontUtil.hpp"

#ifdef __wii__
#include "gccore.h"
#include "wiiuse/wpad.h"
#else /* __wii__ */
#endif /* __wii__ */

bool running = true;

constexpr int32_t mspf = 1000 / 60;

#ifdef __wii__
// newimpl for wii (without couldn't link)
void *operator new[](size_t v) { return malloc(v); }
void operator delete[](void *v) { free(v); }
void *operator new(size_t v) { return malloc(v); }
void operator delete(void *v) { free(v); }

// wii specific variables
int8_t wiiExitState = -1;
#endif /* __wii__ */

#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char *argv[]) {
#ifdef __wii__
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == -1)
#else /* __wii__ */
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == -1)
#endif /* __wii__ */
    {
        return 1;
    }

#ifdef __wii__
    // init WPAD
    WPAD_Init();
    WPAD_SetPowerButtonCallback([](s32 chan) -> void {
        running = false;
        wiiExitState = SYS_POWEROFF_STANDBY;
    });
    SYS_SetResetCallback([](u32 irq, void* ctx) -> void {
        running = false;
        wiiExitState = SYS_RETURNTOMENU;
    });
    SYS_SetPowerCallback([]() -> void {
        running = false;
        wiiExitState = SYS_POWEROFF_STANDBY;
    });
    SDL_ShowCursor(SDL_DISABLE);
#endif /* __wii__ */

    SDL_Surface *screen = SDL_SetVideoMode(640, 480, 16, SDL_HWSURFACE);
    if(screen == nullptr) {
        SDL_Quit();
        return 2;
    }
    SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));

    loadFont();
    uint16_t wx = 16, wy = 16, sx = 16;
    uint8_t fontColor = 7;
    writeFont(screen, wx, wy, sx, fontColor, "SDL JOYSTICK OUT \xA4V1\xA7\n");

    char *writeBuff = new char[512];
    snprintf(writeBuff, 512, "\xA6%i\xA7 joysticks:\n" , SDL_NumJoysticks());
    writeFont(screen, wx, wy, sx, fontColor, writeBuff);
    for(int i = 0; i < SDL_NumJoysticks(); i++) {
        snprintf(writeBuff, 512, "  \xA3%s\xA7\n", SDL_JoystickName(i));
        writeFont(screen, wx, wy, sx, fontColor, writeBuff);
        SDL_Joystick *joystick = SDL_JoystickOpen(i);
        snprintf(writeBuff, 512, "    \xA2%i\xA7 axes\n", SDL_JoystickNumAxes(joystick));
        writeFont(screen, wx, wy, sx, fontColor, writeBuff);
        snprintf(writeBuff, 512, "    \xA2%i\xA7 buttons\n", SDL_JoystickNumButtons(joystick));
        writeFont(screen, wx, wy, sx, fontColor, writeBuff);
        snprintf(writeBuff, 512, "    \xA2%i\xA7 balls\n", SDL_JoystickNumBalls(joystick));
        writeFont(screen, wx, wy, sx, fontColor, writeBuff);
        snprintf(writeBuff, 512, "    \xA2%i\xA7 hats\n", SDL_JoystickNumHats(joystick));
        writeFont(screen, wx, wy, sx, fontColor, writeBuff);
        SDL_JoystickClose(joystick);
    }
    snprintf(writeBuff, 512, "\xA5%d\xA7 BPX\n", screen->format->BytesPerPixel);
    writeFont(screen, wx, wy, sx, fontColor, writeBuff);
    snprintf(writeBuff, 512, "%s\xA7\n", (screen->flags & SDL_HWSURFACE) ? "\xA2is HW" : "\xA4is not HW");
    writeFont(screen, wx, wy, sx, fontColor, writeBuff);
    snprintf(writeBuff, 512, "%s\xA7\n", (screen->flags & SDL_DOUBLEBUF) ? "\xA2is DB" : "\xA4is not DB");
    writeFont(screen, wx, wy, sx, fontColor, writeBuff);
    snprintf(writeBuff, 512, "V DRIVER: \xA3");
    writeFont(screen, wx, wy, sx, fontColor, writeBuff);
    SDL_VideoDriverName(writeBuff, 256);
    writeFont(screen, wx, wy, sx, fontColor, writeBuff);
    snprintf(writeBuff, 512, "\xA7\n");
    writeFont(screen, wx, wy, sx, fontColor, writeBuff);
    
    SDL_Event event;
    uint32_t startMs = SDL_GetTicks();
    int e = 0;
    while(running) {
#ifndef __wii__
        // SDL input
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_QUIT:
                running = false;
                break;
            }
        }
#else /* ! __wii__ */
        // wii input
        // get WiiFB events
        WPAD_ScanPads();
        // exit when homebutton on WiiFB1 is pressed
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) running = false;
#endif /* ! __wii__ */
        wx = 16;
        snprintf(writeBuff, 512, "\xA5%d\xA7", e);
        writeFont(screen, wx, wy, sx, fontColor, writeBuff);

#ifdef __wii__
        // dumb hack witch somehow updates the bufferdata in a way that dolphin updates the immage
        ((uint8_t *)screen->pixels)[1] = e & 3;
#endif /* __wii__ */
        SDL_Flip(screen);
        e++;

#ifndef __wii__
        // frame limit
        uint32_t endMs = SDL_GetTicks();
        int32_t delay = mspf - (endMs - startMs);
        if(delay > 0) SDL_Delay(delay);
        startMs = endMs;
#endif /* ! __wii__ */
    }

    // shutdown
    delete[] writeBuff;
    deleteFont();
    SDL_Quit();
#ifdef __wii__
    if(wiiExitState != -1) SYS_ResetSystem(wiiExitState, 0, 0); // do what the button toldus
    else if(argc > 0) exit(0); //return to homebrew channel
    else SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0); // return to main menu
#endif /* __wii__ */
    return 0;
}
