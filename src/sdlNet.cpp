#include "net.hpp"

#include "config.h"

#include <SDL/SDL_net.h>

IPaddress ip;
TCPsocket sock;
SDLNet_SocketSet socketSet;

uint8_t inD[5];
inPackData in;
uint8_t outD[9];

bool initOK;
uint8_t retCout;

bool sendState(sendPackData *data) {
    if(!netConnected) return false;
    sendPackDataToByteArray(data, outD);
    if(SDLNet_TCP_Send(sock, outD, 9) < 9) netConnected = false;
    return netConnected;
}

void initNetwork() {
    printf("net: init\n");
    initOK = false;
    printf("00\n");
    if(SDLNet_Init() == -1) return;
    printf("01\n");
    if(SDLNet_ResolveHost(&ip, PROXYHOST, PROXYPORT) == -1) return;
    printf("02\n");
    sock = SDLNet_TCP_Open(&ip);
    printf("03\n");
    if(!sock) return;
    printf("04\n");
    socketSet = SDLNet_AllocSocketSet(1);
    printf("05\n");
    if(!socketSet)  {
        SDLNet_TCP_Close(sock);
        return;
    }
    printf("06\n");
    if(SDLNet_TCP_AddSocket(socketSet, sock) == -1) {
        SDLNet_TCP_Close(sock);
        SDLNet_FreeSocketSet(socketSet);
        return;
    }
    printf("07\n");
    netConnected = true;
    initOK = true;
    retCout = 0;
}

void endNetwork() {
    printf("net: end\n");
    if(!initOK) return;
    netConnected = false;
    SDLNet_TCP_DelSocket(socketSet, sock);
    SDLNet_TCP_Close(sock);
    SDLNet_FreeSocketSet(socketSet);
    SDLNet_Quit();
    initOK = false;
}

void loopNetwork() {
    if(!initOK) return;
    if(!netConnected) {
        if(retCout > 3) return;
        printf("net: recon\n");
        retCout++;
        if(!sock) {
            SDLNet_TCP_DelSocket(socketSet, sock);
            SDLNet_TCP_Close(sock);
        }
        sock = SDLNet_TCP_Open(&ip);
        if(!sock) return;
        if(SDLNet_TCP_AddSocket(socketSet, sock) == -1) {
            SDLNet_TCP_Close(sock);
            sock = nullptr;
            return;
        }
        retCout = 0;
    }
    int numRedy = SDLNet_CheckSockets(socketSet, 0);
    if(numRedy != -1) {
        if(numRedy && SDLNet_SocketReady(sock)) {
            int result = SDLNet_TCP_Recv(sock, inD, 5);
            if(result <= 0) {
                netConnected = false;
            } else {
                byteArrayToinPackData(inD, &in);
                handleInData(&in);
            }
        }
    } else netConnected = false;
}

