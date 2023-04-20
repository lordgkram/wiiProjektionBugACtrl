#ifndef NET_HPP_
#define NET_HPP_

#include <cinttypes>

#define NET_STATE_CTRL_STATE 0b11100000
#define NET_STATE_FREE 0b00010000
#define NET_STATE_ENABLED 0b00001000

struct [[gnu::packed]] sendPackData {
    /**
     * state:
     * 0b01234567
     * 
     * 012: 3 bit number of instate: currBugACtrlState
     */
    uint8_t state;
    int16_t joint_a;
    int16_t joint_b;
    int16_t joint_c;
    int16_t vertical_axis;
};

struct [[gnu::packed]] inPackData {
    /**
     * state:
     * 0b01234567
     * 
     * 012: 3 bit number of new instate: currBugACtrlState
     * 3  : is the bugA free
     * 4  : is the bugA enabled
     */
    uint8_t state;
    uint32_t timeLeft;
};

/**
 * A function that must be implemented by the network driver
 */
bool sendState(sendPackData *data);
/**
 * A function that must be implemented by the network driver
 */
void initNetwork();
/**
 * A function that must be implemented by the network driver
 */
void endNetwork();
/**
 * A function that must be implemented by the network driver
 */
void loopNetwork();

/**
 * @param out a pointer to a minimum 9 byte free space
 */
void sendPackDataToByteArray(sendPackData *data, uint8_t *out);

/**
 * @param data a pointer to a 5 byte input
 */
void byteArrayToinPackData(uint8_t *data, inPackData *out);

extern uint32_t timeLeft;
extern bool bugAFree;
extern bool bugAEnabled;
extern bool netConnected;

extern uint16_t framesPerMove;
extern float percentMsPerMove;

void handleInData(inPackData *out);
void handleNetwork();
void printNetwork();

#endif /* NET_HPP_ */
