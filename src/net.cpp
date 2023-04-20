#include "net.hpp"

#include <cstdlib>
#include <SDL/SDL_timer.h>

#include "fontUtil.hpp"
#include "input.hpp"
#include "main.hpp"

uint32_t timeLeft = 0;
bool bugAFree = false;
bool bugAEnabled = false;
bool netConnected = false;

uint16_t framesPerMove = 6;
float percentMsPerMove = 1;
uint16_t framesSinceLastMove = 65535;
uint32_t msLastMove = 0;
uint32_t msPerMove = 0;
sendPackData moveData;

uint32_t lastTimeUpdate = 0;

float clamp(float v, float min, float max) {
    if(v < min) return min;
    if(v > max) return max;
    return v;
}

void sendPackDataToByteArray(sendPackData *data, uint8_t *out) {
    auto aux = reinterpret_cast<sendPackData *>(out);
    *aux = *data;
    aux->joint_a = SDL_SwapBE16(aux->joint_a);
    aux->joint_b = SDL_SwapBE16(aux->joint_b);
    aux->joint_c = SDL_SwapBE16(aux->joint_c);
    aux->vertical_axis = SDL_SwapBE16(aux->vertical_axis);
}

void byteArrayToinPackData(uint8_t *data, inPackData *out) {
    *reinterpret_cast<uint8_t *>(out) = *data;
    out->timeLeft = SDL_SwapBE32(out->timeLeft);
}

void handleInData(inPackData *out) {
    timeLeft = out->timeLeft;
    lastTimeUpdate = SDL_GetTicks();
    bugAEnabled = out->state & NET_STATE_ENABLED;
    bugAFree = out->state & NET_STATE_FREE;
    switch ((out->state & NET_STATE_CTRL_STATE) >> 5) {
    case 0: currBugACtrlState = NO_CTRL; break;
    case 1: currBugACtrlState = TRY_RECEIVE; break;
    case 2: currBugACtrlState = TRY_RECEIVE_NETAPP; break;
    case 3: currBugACtrlState = CTRL; break;
    case 4: currBugACtrlState = TRY_REVOKE; break;
    case 5: currBugACtrlState = TRY_REVOKE_NETAPP; break;
    }
}

void handleNetwork() {
    loopNetwork();
    if(framesSinceLastMove >= framesPerMove) {
        bool init = framesSinceLastMove == 65535; 
        framesSinceLastMove = 0;
        uint32_t currMove = SDL_GetTicks();
        uint32_t msSinceLastMove = currMove - msLastMove;
        msPerMove = msSinceLastMove;
        if(!init) {
            moveData.joint_a = axis0Active ? clamp(axis0, -1, 1) * percentMsPerMove * msSinceLastMove : 0;
            moveData.joint_b = axis1Active ? clamp(axis1, -1, 1) * percentMsPerMove * msSinceLastMove : 0;
            moveData.joint_c = axis2Active ? clamp(axis2, -1, 1) * percentMsPerMove * msSinceLastMove : 0;
            moveData.vertical_axis = axisDActive ? clamp(axisD, -1, 1) * percentMsPerMove * msSinceLastMove : 0;
            moveData.state = 0;
            uint8_t stateCtrlState = 0;
            switch (currBugACtrlState) {
            case NO_CTRL: stateCtrlState = 0; break;
            case TRY_RECEIVE: stateCtrlState = 1; break;
            case TRY_RECEIVE_NETAPP: stateCtrlState = 2; break;
            case CTRL: stateCtrlState = 3; break;
            case TRY_REVOKE: stateCtrlState = 4; break;
            case TRY_REVOKE_NETAPP: stateCtrlState = 5; break;
            }
            if(currBugACtrlState == TRY_RECEIVE) currBugACtrlState = TRY_RECEIVE_NETAPP;
            if(currBugACtrlState == TRY_REVOKE) currBugACtrlState = TRY_REVOKE_NETAPP;
            moveData.state |= stateCtrlState << 5;
            if(sendState(&moveData)) msLastMove = currMove;
            else framesSinceLastMove = framesPerMove;
        } else msLastMove = currMove;
    } else framesSinceLastMove++;
}

void printTime(int32_t secs, char digetColor, char whiteColor) {
    if(secs < 0) secs = 0;
    uint8_t secDisplay = secs % 60;
    uint32_t mins = secs / 60;
    uint8_t minDisplay = mins % 60;
    uint32_t hours = mins / 60;

    snprintf(fontWriteBuff, 512, "%c%02d%c:%c%02d%c:%c%02d", digetColor, hours, whiteColor, digetColor, mins, whiteColor, digetColor, secs);
    writeFont(screen, fontX, fontY, fontLineX, fontColor, fontWriteBuff);
}

void printNetwork() {
    snprintf(fontWriteBuff, 512, "\xA3Netzwerk verbunden\xA7: %s\xA7\n", netConnected ? "\xA2Ja" : "\xA4Nein");
    writeFont(screen, fontX, fontY, fontLineX, fontColor, fontWriteBuff);
    snprintf(fontWriteBuff, 512, "\xA3""BugA-Frei\xA7: %s\xA7\n", bugAFree ? "\xA2Ja" : "\xA4Nein");
    writeFont(screen, fontX, fontY, fontLineX, fontColor, fontWriteBuff);
    snprintf(fontWriteBuff, 512, "\xA3""BugA-Aktiviert\xA7: %s\xA7\n", bugAEnabled ? "\xA2Ja" : "\xA4Nein");
    writeFont(screen, fontX, fontY, fontLineX, fontColor, fontWriteBuff);
    snprintf(fontWriteBuff, 512, "\xA3""Frames per move\xA7: \xA6%d\xA7\n", framesPerMove);
    writeFont(screen, fontX, fontY, fontLineX, fontColor, fontWriteBuff);
    snprintf(fontWriteBuff, 512, "\xA3""Prozent der Zeit\xA7: \xA6%.2f%%\xA7\n", percentMsPerMove * 100);
    writeFont(screen, fontX, fontY, fontLineX, fontColor, fontWriteBuff);
    snprintf(fontWriteBuff, 512, "\xA3""Ms per move\xA7: \xA6%d\xA7\n", msPerMove);
    writeFont(screen, fontX, fontY, fontLineX, fontColor, fontWriteBuff);
    writeFont(screen, fontX, fontY, fontLineX, fontColor, "\xA3Zeit\xA7: ");
    printTime(timeLeft, '\xA6', '\xA7');
    writeFont(screen, fontX, fontY, fontLineX, fontColor, " ");
    printTime(timeLeft - (currBugACtrlState == CTRL ? (SDL_GetTicks() - lastTimeUpdate) / 1000 : 0), '\xA5', '\xA7');
    writeFont(screen, fontX, fontY, fontLineX, fontColor, "\xA7\n");
}
