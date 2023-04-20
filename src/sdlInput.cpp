#include "input.hpp"

#include <SDL/SDL.h>

#include "net.hpp"

SDL_Event event;

bool handleInput() {
    // SDL input
    bool continueProgram = true;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_QUIT:
            continueProgram = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            // exit with ESC
            case SDLK_ESCAPE:
                continueProgram = false;
                break;

            // axes
            case SDLK_u:
                axis2 = -1;
                break;
            case SDLK_j:
                axis2 = 1;
                break;

            case SDLK_i:
                axis1 = 1;
                break;
            case SDLK_k:
                axis1 = -1;
                break;

            case SDLK_o:
                axis0 = -1;
                break;
            case SDLK_l:
                axis0 = 1;
                break;

            // rotation
            case SDLK_e:
                axisD = -1;
                break;
            case SDLK_r:
                axisD = 1;
                break;

            // get accept
            case SDLK_z:
                toggleCtrlState();
                break;

            // net delays
            case SDLK_h:
                framesPerMove++;
                if(framesPerMove >= 60) framesPerMove = 60;
                break;
            case SDLK_n:
                framesPerMove--;
                if(framesPerMove <= 0) framesPerMove = 1;
                break;
            case SDLK_g:
                percentMsPerMove += 0.02f;
                if(percentMsPerMove > 1) percentMsPerMove = 1;
                break;
            case SDLK_b:
                percentMsPerMove -= 0.02f;
                if(percentMsPerMove <= 0) percentMsPerMove = 0.02f;
                break;
            
            default:
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
            // axes
            case SDLK_u:
            case SDLK_j:
                axis2 = 0;
                break;
            case SDLK_i:
            case SDLK_k:
                axis1 = 0;
                break;
            case SDLK_o:
            case SDLK_l:
                axis0 = 0;
                break;
            // rotation
            case SDLK_e:
            case SDLK_r:
                axisD = 0;
                break;
            
            default:
                break;
            }
            break;
        }
    }
    return continueProgram;
}
