#include "main.hpp"

#include <SDL/SDL_timer.h>

#include "fontUtil.hpp"
#include "input.hpp"
#include "net.hpp"

#ifdef __wii__
#include "gccore.h"
#include "wiiuse/wpad.h"
#else /* __wii__ */
#endif /* __wii__ */

bool running = true;

constexpr int32_t mspf = 1000 / 60;

uint16_t fontX = 16, fontY = 16, fontLineX = 16;
uint8_t fontColor = 7;
char *fontWriteBuff;

SDL_Surface *screen;

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
    if(SDL_Init(SDL_INIT_VIDEO) == -1) {
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

    screen = SDL_SetVideoMode(640, 480, 16, SDL_HWSURFACE);
    if(screen == nullptr) {
        SDL_Quit();
        return 2;
    }
    SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));

    loadFont();
    fontWriteBuff = new char[512];
    initNetwork();

    uint32_t startMs = SDL_GetTicks();
    int e = 0;
    while(running) {
        SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));
        fontX = 16;
        fontY = 26;
        writeFont(screen, fontX, fontY, fontLineX, fontColor, "\xA7Wii \xA6ProjektionTV\xA7-\xA4""BugACtrl\xA7 by \xA1gkram\xA7\n");

        running = handleInput();
        printAxis();

        handleNetwork();
        printNetwork();

        snprintf(fontWriteBuff, 512, "\xA7""Frame: \xA5%d\xA7\n", e);
        writeFont(screen, fontX, fontY, fontLineX, fontColor, fontWriteBuff);

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
    endNetwork();
    delete[] fontWriteBuff;
    deleteFont();
    SDL_Quit();
#ifdef __wii__
    if(wiiExitState != -1) SYS_ResetSystem(wiiExitState, 0, 0); // do what the button toldus
    else if(argc > 0) exit(0); //return to homebrew channel
    else SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0); // return to main menu
#endif /* __wii__ */
    return 0;
}
