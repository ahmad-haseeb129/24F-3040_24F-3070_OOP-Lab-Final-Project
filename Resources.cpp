#include "Stronghold.h"

Resources::Resources() {
    resourceCount = 4;  // food, wood, stone, iron
    resourceTypes = new int[resourceCount];
    productionRates = new int[resourceCount];
    consumptionRates = new int[resourceCount];
    storage = new int[resourceCount];

    for (int i = 0; i < resourceCount; i++) {
        resourceTypes[i] = 1000;
        productionRates[i] = 100;
        consumptionRates[i] = 50;
        storage[i] = 5000;
    }
}

Resources::~Resources() {
    delete[] resourceTypes;
    delete[] productionRates;
    delete[] consumptionRates;
    delete[] storage;
}

void Resources::gather() {
    for (int i = 0; i < resourceCount; i++) {
        int produced = productionRates[i];
        if (resourceTypes[i] + produced <= storage[i]) {
            resourceTypes[i] += produced;
        }
    }
}

void Resources::consume() {
    for (int i = 0; i < resourceCount; i++) {
        resourceTypes[i] -= consumptionRates[i];
        if (resourceTypes[i] < 0) resourceTypes[i] = 0;
    }
}

void Resources::trade() {
    for (int i = 0; i < resourceCount; i++) {
        if (resourceTypes[i] > storage[i] / 2) {
            int excess = resourceTypes[i] - storage[i] / 2;
            resourceTypes[i] -= excess;
            resourceTypes[(i + 1) % resourceCount] += excess / 2;
        }
    }
}