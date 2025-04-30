#include "Stronghold.h"

Population::Population() {
    totalPopulation = 1000;
    birthRate = 5;
    deathRate = 3;
    foodConsumption = totalPopulation * 2;
    isRevolting = false;
    happiness = 50;
    ageGroups = new int[5];
    diseaseRate = 0;
}

Population::~Population() {
    delete[] ageGroups;
}

void Population::updatePopulation(int food) {
    if (food < foodConsumption) {
        deathRate += 2;
        happiness -= 10;
    }
    calculateGrowth();
    checkRevoltConditions();
}

void Population::calculateGrowth() {
    int growth = (birthRate - deathRate - diseaseRate) * totalPopulation / 100;
    totalPopulation += growth;
}

void Population::checkRevoltConditions() {
    if (happiness < 20 || foodConsumption > totalPopulation * 3) {
        isRevolting = true;
    }
}