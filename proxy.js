#!/bin/node

const fs = require('fs');
const https = require('https');
const net = require('net');

var clientList = [];
var interval;
var igniteInterval;

var timeLeft = 0;
var ctrlState = 0;
var bugAFree = false;
var bugAEnabled = false;

var sendToBaggerPortal = true;
var apiKey = "";

var server = net.createServer({keepAlive: true}, function(socket) {
    clientList = clientList.concat([socket]);
    socket.on('data', handleData);
    socket.on('end', () => { clientList = clientList.filter((value, index, array) => { return value !== socket; }); });
    socket.on('error', (e) => { console.log(e.message); });
});

/**
 * @param {Buffer} data data from client
 */
function handleData(data) {
    var state = data.at(0);
    var joint_a = data.at(1) << 8 | data.at(2);
    var joint_b = data.at(3) << 8 | data.at(4);
    var joint_c = data.at(5) << 8 | data.at(6);
    var vertical_axis = data.at(7) << 8 | data.at(8);
    if(joint_a & 0x8000) joint_a = joint_a - 0x10000;
    if(joint_b & 0x8000) joint_b = joint_b - 0x10000;
    if(joint_c & 0x8000) joint_c = joint_c - 0x10000;
    if(vertical_axis & 0x8000) vertical_axis = vertical_axis - 0x10000;

    var _ctrlState = (state & 0b11100000) >> 5;

    if(_ctrlState == 1 || (_ctrlState == 2 && ctrlState != 2)) takeControll();
    else if(_ctrlState == 4 || (_ctrlState == 5 && ctrlState != 5)) removeControll();
    sendJoints(joint_a, joint_b, joint_c, vertical_axis);
}

async function writeClient() {
    var uint8 = new Uint8Array(5);
    uint8[0] = (ctrlState & 0x07) << 5 | (bugAFree ? 0b00010000 : 0) | (bugAEnabled ? 0b00001000 : 0);
    uint8[1] = (timeLeft >> 24) & 0xFF;
    uint8[2] = (timeLeft >> 16) & 0xFF;
    uint8[3] = (timeLeft >> 8) & 0xFF;
    uint8[4] = (timeLeft >> 0) & 0xFF;
    clientList.forEach((value, index, array) => { value.write(uint8); });
}

async function takeControll() {
    ctrlState = 2;
    var res = await sendIgnite(true);
    if(res.ok) {
        ctrlState = 3;
        igniteInterval = setInterval(() => {
            sendIgnite(true).then(res => {
                if (!res.ok) {
                    ctrlState = 0;
                }
            });
        }, 5000);
    }
}

async function removeControll() {
    ctrlState = 5;
    var res = await sendIgnite(true);
    if(res.ok) {
        ctrlState = 0;
        clearInterval(igniteInterval);
    } else {
        ctrlState = 3;
    }
}

async function sendJoints(joint_a, joint_b, joint_c, vertical_axis) {
    if(!sendToBaggerPortal) console.log('ja: %d, jb: %d, jc: %d, va: %d', joint_a, joint_b, joint_c, vertical_axis);
    if(ctrlState != 3) return;
    if(!sendToBaggerPortal) return;
    var out = {};
    var relavantToSend = false;
    if(joint_a != 0) { out["joint_a"] = joint_a; relavantToSend = true; }
    if(joint_b != 0) { out["joint_b"] = joint_b; relavantToSend = true; }
    if(joint_c != 0) { out["joint_c"] = joint_c; relavantToSend = true; }
    if(vertical_axis != 0) { out["vertical_axis"] = vertical_axis; relavantToSend = true; }

    if(relavantToSend) await fetch("https://bagger.projektion.tv/api/bugacontrol/move", {
        method: "POST",
        body: JSON.stringify(out),
        headers: {
            "api-key": apiKey,
            "content-type": "application/json",
        },
    });
}

async function sendIgnite(state) {
    if(!sendToBaggerPortal) return {ok: true};
    return await fetch("https://bagger.projektion.tv/api/bugacontrol/ignite", {
        method: "POST",
        body: JSON.stringify({
            on: state,
        }),
        headers: {
            "api-key": apiKey,
            "content-type": "application/json",
        },
    });
}

async function handleServerState(stateData) {
    if(stateData["type"] !== undefined) {
        if (stateData.type === "state") {
            handleServerStateAll(stateData);
        } else if (stateData.type === "charged") {
            timeLeft = stateData.total;
        }
    } else {
        await handleServerStateAll(stateData);
    }
}

async function handleServerStateAll(stateData) {
    timeLeft = stateData.total;
    bugAEnabled = stateData.enabled;
    bugAFree = !stateData.inUse;
    ctrlState = stateData.inUseByCurrentUser ? 3 : 0;
}

async function doSSEListen() {
    if(!sendToBaggerPortal) return;
    https.get(`https://bagger.projektion.tv/api/bugacontrol/status/subscribe?apikey=${apiKey}`, (res) => {
        if(res.statusCode !== 200) return;
        if(!/^text\/event-stream/.test(res.headers['content-type'])) return;
        res.setEncoding('utf8');
        var leftData = [''];
        var currBuild = {};
        res.on('data', (chunk) => {
            var newV = (leftData[leftData.length - 1] + chunk).split("\n");
            leftData = leftData.slice(0, -1).concat(newV);
            while(leftData.length > 1) {
                if(leftData[0].length == 0) {
                    handleServerState(JSON.parse(currBuild["data"]));
                    currBuild = {};
                } else {
                    var sp = leftData[0].split(": ", 2);
                    currBuild[sp[0]] = sp[1];
                }
                leftData = leftData.slice(1);
            }
        });
        res.on('end', () => {
            doSSEListen();
        });
    }).on('error', (e) => {
        doSSEListen();
    });
}

var apiFileData = JSON.parse(fs.readFileSync("apiKey.json", { encoding: "utf-8" }));
if(apiFileData["apiKey"] !== undefined) apiKey = apiFileData["apiKey"];
if(apiFileData["sendToBaggerPortal"] !== undefined) sendToBaggerPortal = apiFileData["sendToBaggerPortal"];

if(sendToBaggerPortal)
    fetch('https://bagger.projektion.tv/api/bugacontrol/status', {
            headers: {
                "api-key": apiKey,
            },
            method: "get",
        })
        .then((res) => res.json())
        .then((stateData) => {
            handleServerState(stateData);
        }).catch((e) => {
            console.log('faild to fetch bagger status: %s', e);
        });

doSSEListen();

interval = setInterval(writeClient, 100);

server.listen(8095, '0.0.0.0');
