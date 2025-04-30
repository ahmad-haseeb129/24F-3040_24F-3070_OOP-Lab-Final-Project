#include "Stronghold.h"

Military::Military() {
    soldiers = new int[5];  // Different types of soldiers
    training = new int[5];
    morale = 70;
    cost = 0;
    weapons = new int[10];
    weaponCount = 100;
    corruptionLevel = 0;
    isLoyal = true;
}

Military::~Military() {
    delete[] soldiers;
    delete[] training;
    delete[] weapons;
}

void Military::updateMilitary(int pay) {
    cost = 0;
    for (int i = 0; i < 5; i++) {
        cost += soldiers[i] * 2;
    }

    if (pay < cost) {
        morale -= 10;
        corruptionLevel += 5;
    }

    handleCorruption();
    assessMorale();
}

void Military::trainTroops() {
    for (int i = 0; i < 5; i++) {
        if (training[i] < 100) {
            training[i] += 5;
        }
    }
}

void Military::assessMorale() {
    if (morale < 30) {
        isLoyal = false;
    }
}

void Military::handleCorruption() {
    if (corruptionLevel > 50) {
        morale -= 5;
        weaponCount -= corruptionLevel / 10;
    }
}