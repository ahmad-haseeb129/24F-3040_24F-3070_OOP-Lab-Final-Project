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
    if (happiness < 30) wealth -= 5;
    if (influence > 70) wealth += 5;
    if (wealth < 20) population -= 10;
}