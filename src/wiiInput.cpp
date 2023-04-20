#include "input.hpp"

#include <cmath>
#include <numbers>

#include <wiiuse/wpad.h>

#include "fontUtil.hpp"
#include "main.hpp"
#include "net.hpp"

constexpr float deg2ang = std::numbers::pi / 180;

float map(float v, float mini, float maxi, float mino, float maxo) {
    return (v - mini) / (maxi - mini) * (maxo - mino) + mino;
}

bool handleInput() {
    // Wii input
    bool continueProgram = true;
    // wii input
    // get WiiFB events
    WPAD_ScanPads();
    // WiiFB1
    WPADData *fb1 = WPAD_Data(0);
    uint32_t pressed = fb1->btns_d;
    uint32_t released = fb1->btns_u;

    if(fb1->exp.type == EXP_NUNCHUK) {
        // nunchuk is allways active
        axis2Active = true;

        // wfb-orient-roll > 15 | < -15 -> axisD
        float wfbr = fb1->orient.roll;
        if(wfbr > 15) axisD = map(wfbr, 15, 75, 0, -1); // hold right
        else if(wfbr < -15) axisD = map(wfbr, -15, -75, 0, 1); // hold left
        else axisD = 0;

        // wfb-orient-pitch > 10 | < -10 -> axis0
        float wfbp = fb1->orient.pitch;
        if(wfbp > 10) axis0 = map(wfbp, 10, 60, 0, -1); // held down
        else if(wfbp < -10) axis0 = map(wfbp, -10, -60, 0, 1); // held up
        else axis0 = 0;
        
        // nun-orient-pitch > 10 | < -10 -> axis1
        float nunp = fb1->exp.nunchuk.orient.pitch;
        if(nunp > 10) axis1 = map(nunp, 10, 60, 0, -1); // held down
        else if(nunp < -10) axis1 = map(nunp, -10, -60, 0, 1); // held up
        else axis1 = 0;

        // nun-js -> axis2
        float jsy = std::cos(fb1->exp.nunchuk.js.ang * deg2ang) * fb1->exp.nunchuk.js.mag * 1.02f;
        if(jsy > 0.02f) axis2 = jsy - 0.02f;
        else if(jsy < -0.02f) axis2 = jsy + 0.02f;
        else axis2 = 0;

        // controle state
        if(pressed & WPAD_BUTTON_PLUS) toggleCtrlState();

        // toggle axes
        // rotation
        if(pressed & WPAD_BUTTON_A) axisDActive = true;
        if(released & WPAD_BUTTON_A) axisDActive = false;
        // pitch wfb
        if(pressed & WPAD_BUTTON_B) axis0Active = true;
        if(released & WPAD_BUTTON_B) axis0Active = false;
        // pitch nunchuk
        if(pressed & WPAD_NUNCHUK_BUTTON_Z) axis1Active = true;
        if(released & WPAD_NUNCHUK_BUTTON_Z) axis1Active = false;
        // toggel all exies with nunchuk c button
        if(pressed & WPAD_NUNCHUK_BUTTON_C) {
            axisDActive = !axisDActive;
            axis0Active = axisDActive;
            axis1Active = axisDActive;
        }

        // net speed
        if(pressed & WPAD_BUTTON_RIGHT) {
            framesPerMove++;
            if(framesPerMove >= 60) framesPerMove = 60;
        }
        if(pressed & WPAD_BUTTON_LEFT) {
            framesPerMove--;
            if(framesPerMove <= 0) framesPerMove = 1;
        }
        if(pressed & WPAD_BUTTON_UP) {
            percentMsPerMove += 0.02f;
            if(percentMsPerMove > 1) percentMsPerMove = 1;
        }
        if(pressed & WPAD_BUTTON_DOWN) {
            percentMsPerMove -= 0.02f;
            if(percentMsPerMove <= 0) percentMsPerMove = 0.02f;
        }
    } else {
        // no nunchuck
        axis0 = 0;
        axis1 = 0;
        axis2 = 0;
        axisD = 0;
        axis0Active = false;
        axis1Active = false;
        axis2Active = false;
        axisDActive = false;

        writeFont(screen, fontX, fontY, fontLineX, fontColor, "\xA7Please \xA6""connect\xA7 a \xA3Nunchuk\xA7 to the \xA5WFB1\xA7\n");
        if(currBugACtrlState == TRY_RECEIVE) currBugACtrlState = NO_CTRL;
        else if(currBugACtrlState == CTRL) currBugACtrlState = TRY_REVOKE;
    }

    // exit program
    if(pressed & WPAD_BUTTON_HOME) continueProgram = false;

    // exit input handler
    return continueProgram;
}
