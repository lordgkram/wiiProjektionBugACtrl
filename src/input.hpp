#ifndef INPUT_HPP_
#define INPUT_HPP_

/**
 * A function that must be implemented by the input driver
 * @returns if the loop/programm should continue
 */
bool handleInput();

enum bugACtrlState {
    NO_CTRL,
    TRY_RECIVE,
    TRY_RECIVE_NETAPP, // net handle send request
    CTRL,
    TRY_REVOKE,
    TRY_REVOKE_NETAPP // net handle send request
};

void printAxis();
void toggleCtrlState();

extern bugACtrlState currBugACtrlState;

extern float axis0; // joint_a
extern float axis1; // joint_b
extern float axis2; // joint_c
extern float axisD; // vertical_axis
extern bool axis0Active;
extern bool axis1Active;
extern bool axis2Active;
extern bool axisDActive;

#endif /* INPUT_HPP_ */
