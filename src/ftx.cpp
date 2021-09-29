/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "stdafx.h"
#include "ftx.h"
#include <wtypes.h>

#define PLUGIN_VERSION    2

DLLFUNC_C int BrokerOpen(char *Name, FARPROC fpError, FARPROC fpProgress) {
    strcpy_s(Name, 32, "FTX");
    (FARPROC &) BrokerError = fpError;
    (FARPROC &) BrokerProgress = fpProgress;
    return PLUGIN_VERSION;
}

DLLFUNC_C void BrokerHTTP(FARPROC fpSend, FARPROC fpStatus, FARPROC fpResult, FARPROC fpFree) {
    (FARPROC &) http_send = fpSend;
    (FARPROC &) http_status = fpStatus;
    (FARPROC &) http_result = fpResult;
    (FARPROC &) http_free = fpFree;
}

DLLFUNC_C int BrokerLogin(char *User, char *Pwd, char *Type, char *Account) {
    return 0;
}

DLLFUNC_C int BrokerTime(DATE *pTimeGMT) {
    return 0;
}

DLLFUNC_C int BrokerAsset(char *Asset, double *pPrice, double *pSpread, double *pVolume, double *pPip, double *pPipCost,
                          double *pLotAmount, double *pMarginCost, double *pRollLong, double *pRollShort) {
    return 0;
}

DLLFUNC_C int BrokerHistory2(char *Asset, DATE tStart, DATE tEnd, int nTickMinutes, int nTicks, T6 *ticks) {
    return 0;
}

DLLFUNC_C int BrokerBuy(char *Asset, int nAmount, double dStopDist, double *pPrice) {
    return 0;
}
DLLFUNC_C double BrokerCommand(int Command, DWORD dwParameter) {
    return 0;
}