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
#include <string>
#include <ftx_client.h>
#include <fstream>

#define PLUGIN_VERSION    2

bool verbose = true;
std::unique_ptr<FTXClient> ftxClient;

bool httpGetMethod(const std::string &url, const std::string &header, std::string &response) {

    int n;
    int id = http_send(const_cast<char *>(url.data()), nullptr, const_cast<char *>(header.data()));

    if (!id) {
        if (verbose) {
            BrokerError("\nError: Cannot connect to server.");
            // TODO: log
        }
        return true;
    }

    while (!http_status(id)) {
        Sleep(100); // wait for the server to reply
        if (!BrokerProgress(1)) {
            if (verbose) {
                BrokerError("\nBrokerProgress returned zero. Aborting...");
            }
            http_free(id); //always clean up the id!
            return false; //failure
        } // print dots, abort if returns zero.
    }

    n = http_status(id);

    if (n > 0)  //transfer successful?
    {
        char *output;
        output = (char *) malloc(n + 1);
        http_result(id, output, n);   //get the replied IP
        response = output;
        free(output); //free up memory allocation
        http_free(id); //always clean up the id!

        // HACK - trim off odd characters at end of message, up to five attempts
//        for (int ii = 1; ii<= 5; ii++)
//        {
//            if (Output->substr(Output->length()-1, 1) == ">") break;
//
//            //*Output = Output->substr(0, Output->length() - 1);
//            Output->pop_back();
//        }

//        if (IsQuotaExceeded(*Output))
//        {
//            if (!quota_exceeded) {
//                BrokerError("Quota exceeded! Please wait...");
//                quota_exceeded = true;
//            }
//            if (!BrokerProgress(1))
//            {
//                if (diag)BrokerError("\nBrokerprogress returned zero. Aborting...");
//                http_free(id); //always clean up the id!
//                return 0; //failure
//            }
//            *Output = "";
//            //if (diag)BrokerError("Trying again..");
//            Sleep(INTERVAL_QUOTA_EXCEEDED_MS);
//            continue;
//        }

        return true; //success
    } else {
        if (verbose) {
            BrokerError("\nError during transfer from server.");
            // TODO: log
        }
        http_free(id); //always clean up the id!
        return false; //failure
    }
}

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

    if (!User) {
        return 0;
    } else if (((std::string) Type) == "Demo") {
        BrokerError("\nDemo mode not supported by this plugin.");
        return 0;
    } else {
        if (!ftxClient) {
            if (!std::string_view(User).empty() && !std::string_view(Pwd).empty()) {
                ftxClient = std::make_unique<FTXClient>(User, Pwd, Account);
                ftxClient->setHttpGetMethod(httpGetMethod);
            } else {
                BrokerError("\nError: Missing or Incomplete Account credentials.");
                return 0;
            }
        } else {
            ftxClient->setCredentials(User, Pwd, Account);
        }
    }

    try {
        ftxClient->getAccountInfo();
        // TODO: Log
    }
    catch (std::exception &e) {
        return 0;
    }

    return 0;
}

DLLFUNC_C int BrokerTime(DATE *pTimeGMT) {
    return 0;
}

DLLFUNC_C int
BrokerAsset(char *Asset, double *pPrice, double *pSpread, double *pVolume, double *pPip, double *pPipCost,
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

    switch (Command) {
        case SET_DELAY:
            break;
    }

    return 0;
}