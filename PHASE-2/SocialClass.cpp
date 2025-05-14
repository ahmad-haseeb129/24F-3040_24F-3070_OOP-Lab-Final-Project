#define _CRT_SECURE_NO_WARNINGS
#include "Stronghold.h"
#include <string>

SocialClass::SocialClass(const char* name) {
    className = new char[strlen(name) + 1];
    strcpy(className, name);
    happiness = 50;
    wealth = 50;
    influence = 50;
    population = 100;
    birthRate = 5;      // Base birth rate per 1000 people
    deathRate = 3;      // Base death rate per 1000 people
    foodConsumption = population * 2;  // Each person consumes 2 units
    isRevolting = false;
}

SocialClass::~SocialClass() {
    delete[] className;
}

void SocialClass::interact(SocialClass* other) {
    if (wealth > other->wealth) {
        influence += 5;
        other->influence -= 5;
    }
    if (population > other->population) {
        happiness -= 2;
        other->happiness += 2;
    }
}

void SocialClass::updateStatus() {
    // Update base attributes
    if (happiness < 30) wealth -= 2;  // Reduced from 5
    if (influence > 70) wealth += 2;  // Reduced from 5
    
    // Calculate population changes
    int births = (birthRate * population) / 1000;
    int deaths = (deathRate * population) / 1000;
    int growth = births - deaths;
    
    // Update population with bounds checking
    if (population + growth < 0) {
        population = 0;
    } else {
        population += growth;
    }
    
    // Update food consumption
    foodConsumption = population * 2;
    
    // Check revolt conditions
    if (happiness < 15 || foodConsumption > population * 4) {  // Made revolt conditions more lenient
        isRevolting = true;
    } else {
        isRevolting = false;
    }
    
    // Keep values in valid ranges
    if (wealth < 0) wealth = 0;
    if (wealth > 100) wealth = 100;
    if (influence < 0) influence = 0;
    if (influence > 100) influence = 100;
    if (happiness < 0) happiness = 0;
    if (happiness > 100) happiness = 100;
}