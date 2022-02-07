/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#ifndef FTX_ZORRO_PLUGIN_FTX_H
#define FTX_ZORRO_PLUGIN_FTX_H

#include <wtypes.h>
#include "stdafx.h"
#include <zorro/trading.h>

#ifdef FTX_DLL_EXPORTS
#define DLLFUNC extern __declspec(dllexport)
#define DLLFUNC_C extern "C" __declspec(dllexport)
#else
#define DLLFUNC extern __declspec(dllimport)
#define DLLFUNC_C extern "C" __declspec(dllimport)
#endif

int (__cdecl *BrokerError)(const char *txt) = nullptr;

int (__cdecl *BrokerProgress)(const int percent) = nullptr;

// zorro functions
DLLFUNC_C int BrokerOpen(char *Name, FARPROC fpError, FARPROC fpProgress);
DLLFUNC_C int BrokerLogin(char *User, char *Pwd, char *Type, char *Account);
DLLFUNC_C int BrokerTime(DATE *pTimeGMT);
DLLFUNC_C int BrokerAsset(char *Asset, double *pPrice, double *pSpread, double *pVolume, double *pPip, double *pPipCost,
                          double *pLotAmount, double *pMarginCost, double *pRollLong, double *pRollShort);
DLLFUNC_C int BrokerHistory2(char *Asset, DATE tStart, DATE tEnd, int nTickMinutes, int nTicks,
                             T6 *ticks);  // only supports stocks, no option history available.
DLLFUNC_C int BrokerBuy2(char* Asset,int Amount,double dStopDist,double Limit,double *pPrice,int *pFill);
DLLFUNC_C double BrokerCommand(int Command, DWORD dwParameter);
DLLFUNC_C int BrokerAccount(char* Account,double *pdBalance,double *pdTradeVal,double *pdMarginVal);

#endif //FTX_ZORRO_PLUGIN_FTX_H