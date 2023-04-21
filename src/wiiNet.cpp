#include "net.hpp"

#include "config.h"

#include <errno.h>
#include <network.h>
#include <fcntl.h>

#include <SDL/SDL.h>

s32 socket;

uint8_t inD[5];
inPackData in;
uint8_t outD[9];
uint8_t inRead = 0;

bool inRecon;

bool initOK;
uint8_t retCout;

sockaddr_in *address;
sockaddr *addressFC;
pollsd pollData;

constexpr uint32_t conTimeout = 10000;

s32 tcp_socket() {
    s32 s = net_socket(PF_INET, SOCK_STREAM, 0);
    if (s < 0) return s;

    u32 val = 1;
    net_setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));

    s32 res = net_fcntl(s, F_GETFL, 0);
    if (res < 0) {
        net_close(s);
        return res;
    }

    res = net_fcntl(s, F_SETFL, res | 4);
    if (res < 0) {
        net_close(s);
        return res;
    }

    return s;
}

bool sendState(sendPackData *data) {
    if(!netConnected) return false;
    sendPackDataToByteArray(data, outD);
    uint8_t send = 0;
    while(true) {
        s32 result = net_send(socket, outD, 9 - send, 0);
        if(result < 0) { netConnected = false; return false; } // error
        else if(result > 0) {
            send += result;
        }
        if(send >= 9) return true;
    }
}

void initNetwork() {
    netConnected = false;
    initOK = false;
    if(net_init() < 0) return;
    hostent *resolveData = net_gethostbyname(PROXYHOST);
	if ((!resolveData) || (resolveData->h_addrtype != PF_INET) || (resolveData->h_length != 4)) {
        net_deinit();
		return;
	}
    socket = tcp_socket();
    if(socket < 0) {
        net_deinit();
		return;
    }
    address = new sockaddr_in();
    memset(address, 0, sizeof(sockaddr_in));
    address->sin_family = PF_INET;
    address->sin_len = sizeof(sockaddr_in);
    address->sin_port = SDL_SwapBE16(PROXYPORT);
    auto aux = reinterpret_cast<u32 **>(resolveData->h_addr_list);
    address->sin_addr.s_addr = *aux[0];

    pollData.socket = socket;
    pollData.events |= POLLIN;

    addressFC = (sockaddr *)address;

    uint32_t start = SDL_GetTicks();
    while(true) {
        if(SDL_GetTicks() - start > conTimeout) {
            net_close(socket);
            net_deinit();
            return;
        }
        s32 result = net_connect(socket, addressFC, address->sin_len);
        if(result < 0) {
            if(result == -EISCONN) break; // connected
            if(result == -EINPROGRESS || result == -EALREADY) {
                // in progress
                SDL_Delay(20);
                continue;
            }
            // unhandleable
            net_close(socket);
            net_deinit();
            return;
        }
        break;
    }

    netConnected = true;
    initOK = true;
    retCout = 0;
}

void endNetwork() {
    if(!initOK) return;
    netConnected = false;
    net_close(socket);
    net_deinit();
    initOK = false;
}

void loopNetwork() {
    if(!initOK) return;
    if(!netConnected) {
        if(retCout > 3) return;
        if(!inRecon) {
            retCout++;
            if(socket >= 0) net_close(socket);
            socket = tcp_socket();
            pollData.socket = socket;
            if(socket >= 0) inRecon = true;
        } else {
            s32 result = net_connect(socket, addressFC, address->sin_len);
            if(result < 0) {
                if(result == -EINPROGRESS || result == -EALREADY) {
                    // in progress
                    return;
                }
                // done or unhandleable
                inRecon = false;
                if(result == -EISCONN) {
                    netConnected = true;
                } else {
                    // unhandleable
                    return;
                }
            }
        }
    }
    if(netConnected) {
        s32 pres = net_poll(&pollData, 1, 0);
        if(pres > 0) {
            s32 result = net_recv(socket, inD, 5 - inRead, 0);
            if(result < 0) { netConnected = false; return; } // error
            else if(result > 0) {
                inRead += result;
                if(inRead == 5) {
                    inRead = 0;
                    byteArrayToinPackData(inD, &in);
                    handleInData(&in);
                }
            }
            // no data
            return;
        }
    }
}
