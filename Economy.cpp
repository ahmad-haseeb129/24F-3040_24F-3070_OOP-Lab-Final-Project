#include "Stronghold.h"

Economy::Economy() {
    treasury = 1000;
    taxRate = 10;
    inflation = 0;
    marketSize = 10;
    marketPrices = new int[marketSize];
    tradingPartners = new int[5];

    for (int i = 0; i < marketSize; i++) {
        marketPrices[i] = 100;
    }

    for (int i = 0; i < 5; i++) {
        tradingPartners[i] = 1;
    }
}

Economy::~Economy() {
    delete[] marketPrices;
    delete[] tradingPartners;
}

void Economy::collectTaxes(int population) {
    int collected = (population * taxRate) / 100;
    treasury += collected;

    if (taxRate > 20) {
        inflation += 2;
    }

    adjustInflation();
}

void Economy::adjustInflation() {
    if (inflation > 0) {
        for (int i = 0; i < marketSize; i++) {
            marketPrices[i] += (marketPrices[i] * inflation) / 100;
        }

        if (treasury > 0) {
            treasury -= (treasury * inflation) / 100;
        }
    }
}

void Economy::handleTrade() {
    int tradeBalance = 0;
    for (int i = 0; i < 5; i++) {
        if (tradingPartners[i] > 0) {
            tradeBalance += tradingPartners[i] * 100;
        }
    }
    treasury += tradeBalance;
}