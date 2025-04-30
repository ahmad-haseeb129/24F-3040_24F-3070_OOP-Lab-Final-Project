#define _CRT_SECURE_NO_WARNINGS
#include "Stronghold.h"
#include <string>

Leadership::Leadership(const char* leader) {
    currentLeader = new char[strlen(leader) + 1];
    strcpy(currentLeader, leader);
    popularity = 50;
    term = 0;
    isCoup = false;
    supporterCount = 100;
    supporters = new int[supporterCount];
    for (int i = 0; i < supporterCount; i++) {
        supporters[i] = rand() % 100;
    }
}

Leadership::~Leadership() {
    delete[] currentLeader;
    delete[] supporters;
}

void Leadership::holdElection() {
    int votes = 0;
    for (int i = 0; i < supporterCount; i++) {
        if (supporters[i] > 50) votes++;
    }

    if (votes < supporterCount / 2) {
        popularity -= 10;
        if (popularity < 30) isCoup = true;
    }
    else {
        term++;
        popularity += 5;
    }
}

void Leadership::checkStability() {
    int loyalSupporters = 0;
    for (int i = 0; i < supporterCount; i++) {
        if (supporters[i] > 75) loyalSupporters++;
    }

    if (loyalSupporters < supporterCount / 4) {
        isCoup = true;
    }
}

void Leadership::implementPolicy() {
    if (popularity > 70) {
        for (int i = 0; i < supporterCount; i++) {
            supporters[i] += 5;
            if (supporters[i] > 100) supporters[i] = 100;
        }
    }
    else {
        for (int i = 0; i < supporterCount; i++) {
            supporters[i] -= 5;
            if (supporters[i] < 0) supporters[i] = 0;
        }
    }
}