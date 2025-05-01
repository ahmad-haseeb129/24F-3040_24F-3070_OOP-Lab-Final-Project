#include "Stronghold.h"

Military::Military() {
    soldiers = new int[5];  // Different types of soldiers
    training = new int[5];
    
    // Initialize all array values to 0
    for(int i = 0; i < 5; i++) {
        soldiers[i] = 0;
        training[i] = 0;
    }
    
    weapons = new int[10];
    for(int i = 0; i < 10; i++) {
        weapons[i] = 0;
    }
    
    morale = 70;
    cost = 0;
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
        cost += soldiers[i] * 2;  // Each soldier costs 2 gold per turn
    }

    // Pay affects morale
    if (pay < cost) {
        morale = max(0, morale - 10);  // Can't go below 0
        corruptionLevel += 5;
    } else {
        // Slight morale boost if paid properly
        morale = min(100, morale + 2);
    }

    // Handle corruption effects on morale
    handleCorruption();
    
    // Natural morale recovery if above minimum threshold
    if (morale > 30 && corruptionLevel < 30) {
        morale = min(100, morale + 1);
    }

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
        morale = max(0, morale - 5);  // Can't go below 0
        weaponCount = max(0, weaponCount - corruptionLevel / 10);
    }
    
    // Corruption can naturally decrease if morale is high
    if (morale > 70) {
        corruptionLevel = max(0, corruptionLevel - 2);
    }
}