#define _CRT_SECURE_NO_WARNINGS
#include "Stronghold.h"
#include <string>

Leadership::Leadership(const char* leader) {
    currentLeader = new char[strlen(leader) + 1];
    strcpy(currentLeader, leader);
    popularity = 50;
    term = 1;
    isCoup = false;
    supporterCount = 10;
    supporters = new int[supporterCount];
    for (int i = 0; i < supporterCount; i++) {
        supporters[i] = rand() % 100;  // Random initial support levels
    }
}

Leadership::~Leadership() {
    delete[] currentLeader;
    delete[] supporters;
}

void Leadership::checkStability() {
    // Count strong supporters (>70% support)
    int strongSupporters = 0;
    for (int i = 0; i < supporterCount; i++) {
        if (supporters[i] > 70) strongSupporters++;
    }
    
    // Calculate risk factors
    int riskFactors = 0;
    
    if (popularity < 30) riskFactors++;  // Low popularity
    if (strongSupporters < supporterCount / 3) riskFactors++;  // Low strong support
    if (term > 10) riskFactors++;  // Long term in power
    
    // Only start coup if multiple risk factors present
    if (!isCoup && riskFactors >= 2) {
        isCoup = true;
    }
    
    // Natural support changes
    for (int i = 0; i < supporterCount; i++) {
        // Supporters are influenced by popularity
        if (popularity > 70) {
            supporters[i] = min(100, supporters[i] + 5);
        } else if (popularity < 30) {
            supporters[i] = max(0, supporters[i] - 5);
        }
    }
    
    // Popularity naturally declines over time unless very high support
    if (strongSupporters < supporterCount / 2) {
        popularity = max(0, popularity - 2);
    }
}

void Leadership::holdElection() {
    // Reset term and adjust popularity based on election results
    term = 1;
    
    // Count supporters
    int totalSupport = 0;
    for (int i = 0; i < supporterCount; i++) {
        totalSupport += supporters[i];
    }
    
    // Average support determines election outcome
    int averageSupport = totalSupport / supporterCount;
    if (averageSupport > 60) {
        popularity += 20;
        isCoup = false;  // Successfully prevented coup through election
    } else {
        popularity -= 20;
    }
    
    // Ensure popularity stays within bounds
    popularity = max(0, min(100, popularity));
}

void Leadership::implementPolicy(int policyType) {
    switch(policyType) {
        case 1: // Economic Focus
            // Increase support from merchants and wealthy
            for (int i = 0; i < supporterCount; i++) {
                if (supporters[i] > 50) { // Wealthy supporters
                    supporters[i] = min(100, supporters[i] + 15);
                } else {
                    supporters[i] = max(0, supporters[i] - 5);
                }
            }
            popularity += 5;
            break;
            
        case 2: // Military Focus
            // Increase support from military-aligned supporters
            for (int i = 0; i < supporterCount/2; i++) { // First half represents military
                supporters[i] = min(100, supporters[i] + 20);
            }
            popularity += 5;
            break;
            
        case 3: // Population Focus
            // Broad popularity increase but less supporter loyalty
            popularity += 15;
            for (int i = 0; i < supporterCount; i++) {
                supporters[i] = max(30, min(70, supporters[i])); // Normalize support levels
            }
            break;
    }
    
    // Ensure popularity stays within bounds
    popularity = max(0, min(100, popularity));
    
    // Increment term counter for each policy implemented
    term++;
}