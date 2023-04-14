#include "input.hpp"

#include "fontUtil.hpp"
#include "main.hpp"

bugACtrlState currBugACtrlState = NO_CTRL;
float axis0 = 0;
float axis1 = 0;
float axis2 = 0;
float axisD = 0;
bool axis0Active = true;
bool axis1Active = true;
bool axis2Active = true;
bool axisDActive = true;

void toggleCtrlState() {
    if(currBugACtrlState == NO_CTRL) currBugACtrlState = TRY_RECIVE;
    else if(currBugACtrlState == TRY_RECIVE) currBugACtrlState = NO_CTRL;
    else if(currBugACtrlState == CTRL) currBugACtrlState = TRY_REVOKE;
    else if(currBugACtrlState == TRY_REVOKE) currBugACtrlState = CTRL;
}

void printAxis() {
    snprintf(fontWriteBuff, 512, "\xA3""Axis-0\xA7: %c%f\xA7\n", axis0Active ? '\xA6' : '\xA5', axis0);
    writeFont(screen, fontX, fontY, fontLineX, fontColor, fontWriteBuff);
    snprintf(fontWriteBuff, 512, "\xA3""Axis-1\xA7: %c%f\xA7\n", axis1Active ? '\xA6' : '\xA5', axis1);
    writeFont(screen, fontX, fontY, fontLineX, fontColor, fontWriteBuff);
    snprintf(fontWriteBuff, 512, "\xA3""Axis-2\xA7: %c%f\xA7\n", axis2Active ? '\xA6' : '\xA5', axis2);
    writeFont(screen, fontX, fontY, fontLineX, fontColor, fontWriteBuff);
    snprintf(fontWriteBuff, 512, "\xA3""Axis-D\xA7: %c%f\xA7\n", axisDActive ? '\xA6' : '\xA5', axisD);
    writeFont(screen, fontX, fontY, fontLineX, fontColor, fontWriteBuff);

    writeFont(screen, fontX, fontY, fontLineX, fontColor, "\xA7""In Conntroll state: ");
    switch (currBugACtrlState) {
    case NO_CTRL: writeFont(screen, fontX, fontY, fontLineX, fontColor, "\xA6No Ctrl\xA7\n"); break;
    case TRY_RECIVE: writeFont(screen, fontX, fontY, fontLineX, fontColor, "\xA2Try Recive\xA7\n"); break;
    case TRY_RECIVE_NETAPP: writeFont(screen, fontX, fontY, fontLineX, fontColor, "\xA4Try Recive\xA7\n"); break;
    case CTRL: writeFont(screen, fontX, fontY, fontLineX, fontColor, "\xA6""Ctrl\xA7\n"); break;
    case TRY_REVOKE: writeFont(screen, fontX, fontY, fontLineX, fontColor, "\xA2Try Revoke\xA7\n"); break;
    case TRY_REVOKE_NETAPP: writeFont(screen, fontX, fontY, fontLineX, fontColor, "\xA4Try Revoke\xA7\n"); break;
    }
}
