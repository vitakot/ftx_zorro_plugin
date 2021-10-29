/*
FTX Zorro Plugin
https://github.com/vitakot/ftx_zorro_plugin

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2021 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "ftx.h"

int main() {

    double price;
    int fill;

    CreateDummyClient();
    BrokerBuy2((char*)"", 10, 0.0, 0.0, &price, &fill);

    return 0;
}
