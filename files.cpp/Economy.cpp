#include "Stronghold.h"

Economy::Economy() {
    treasury = 1000;
    taxRate = 10;
    inflation = 0;
}

Economy::~Economy() {
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
    // Simple inflation adjustment
    if (inflation > 0) {
        if (treasury > 0) {
            treasury -= (treasury * inflation) / 100;
        }
    }
}

void Economy::handleTrade() {
    // Simple trade balance calculation
    if (treasury > 1000) {
        treasury += treasury / 20;  // 5% profit from trade if treasury is healthy
    }
}