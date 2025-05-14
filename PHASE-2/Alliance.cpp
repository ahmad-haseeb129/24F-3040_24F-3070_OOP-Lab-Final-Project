#define _CRT_SECURE_NO_WARNINGS
#include "Stronghold.h"
#include <iostream>
#include <string> 

Alliance::Alliance() {
    treatyCount = 0;
    reputation = 50;
    for (int i = 0; i < MAX_TREATIES; i++) {
        treaties[i].kingdom1[0] = '\0';
        treaties[i].kingdom2[0] = '\0';
        treaties[i].type[0] = '\0';
        treaties[i].duration = 0;
    }
}

void Alliance::proposeTreaty(const char* kingdom1, const char* kingdom2, const char* type, int duration) {
    if (treatyCount >= MAX_TREATIES) {
        displayEvent("ERROR", "Treaty storage full!");
        return;
    }
    if (hasTreaty(kingdom1, kingdom2, type)) {
        char message[100];
        strcpy(message, "Treaty already exists between ");
        strcat(message, kingdom1);
        strcat(message, " and ");
        strcat(message, kingdom2);
        displayEvent("ERROR", message);
        return;
    }
    strncpy(treaties[treatyCount].kingdom1, kingdom1, MAX_NAME_LEN - 1);
    treaties[treatyCount].kingdom1[MAX_NAME_LEN - 1] = '\0';
    strncpy(treaties[treatyCount].kingdom2, kingdom2, MAX_NAME_LEN - 1);
    treaties[treatyCount].kingdom2[MAX_NAME_LEN - 1] = '\0';
    strncpy(treaties[treatyCount].type, type, 19);
    treaties[treatyCount].type[19] = '\0';
    treaties[treatyCount].duration = duration;
    treatyCount++;
    char message[200];
    strcpy(message, "Treaty proposed: ");
    strcat(message, type);
    strcat(message, " between ");
    strcat(message, kingdom1);
    strcat(message, " and ");
    strcat(message, kingdom2);
    strcat(message, " for ");
    std::string durationStr = std::to_string(duration); // Replaced itoa
    strcat(message, durationStr.c_str());
    strcat(message, " turns");
    displayEvent("TREATY PROPOSED", message);
}

void Alliance::breakTreaty(const char* kingdom1, const char* kingdom2) {
    for (int i = 0; i < treatyCount; i++) {
        if ((strcmp(treaties[i].kingdom1, kingdom1) == 0 && strcmp(treaties[i].kingdom2, kingdom2) == 0) ||
            (strcmp(treaties[i].kingdom1, kingdom2) == 0 && strcmp(treaties[i].kingdom2, kingdom1) == 0)) {
            char message[200];
            strcpy(message, "Treaty broken between ");
            strcat(message, kingdom1);
            strcat(message, " and ");
            strcat(message, kingdom2);
            strcat(message, ": ");
            strcat(message, treaties[i].type);
            displayEvent("TREATY BROKEN", message);
            reputation = reputation > 10 ? reputation - 10 : 0; // Penalty for breaking treaty
            treaties[i] = treaties[treatyCount - 1]; // Remove treaty
            treatyCount--;
            return;
        }
    }
    char message[100];
    strcpy(message, "No treaty found between ");
    strcat(message, kingdom1);
    strcat(message, " and ");
    strcat(message, kingdom2);
    displayEvent("ERROR", message);
}

bool Alliance::isAllied(const char* kingdom1, const char* kingdom2) {
    for (int i = 0; i < treatyCount; i++) {
        if (((strcmp(treaties[i].kingdom1, kingdom1) == 0 && strcmp(treaties[i].kingdom2, kingdom2) == 0) ||
            (strcmp(treaties[i].kingdom1, kingdom2) == 0 && strcmp(treaties[i].kingdom2, kingdom1) == 0)) &&
            strcmp(treaties[i].type, "Alliance") == 0) {
            return true;
        }
    }
    return false;
}

bool Alliance::hasTreaty(const char* kingdom1, const char* kingdom2, const char* type) {
    for (int i = 0; i < treatyCount; i++) {
        if (((strcmp(treaties[i].kingdom1, kingdom1) == 0 && strcmp(treaties[i].kingdom2, kingdom2) == 0) ||
            (strcmp(treaties[i].kingdom1, kingdom2) == 0 && strcmp(treaties[i].kingdom2, kingdom1) == 0)) &&
            strcmp(treaties[i].type, type) == 0) {
            return true;
        }
    }
    return false;
}

void Alliance::updateTreaties() {
    // Check for valid treatyCount first
    if (treatyCount <= 0) {
        return; // No treaties to update
    }
    
    // Ensure treatyCount isn't beyond bounds
    if (treatyCount > MAX_TREATIES) {
        treatyCount = MAX_TREATIES;
    }

    for (int i = 0; i < treatyCount; i++) {
        // Ensure strings are properly null-terminated
        treaties[i].kingdom1[MAX_NAME_LEN - 1] = '\0';
        treaties[i].kingdom2[MAX_NAME_LEN - 1] = '\0';
        treaties[i].type[19] = '\0';
        
        // Check for valid treaty data
        if (treaties[i].kingdom1[0] == '\0' || treaties[i].kingdom2[0] == '\0' || treaties[i].type[0] == '\0') {
            // Skip invalid treaty data
            treaties[i] = treaties[treatyCount - 1];
            treatyCount--;
            i--;
            continue;
        }
        
        if (treaties[i].duration > 0) {
            treaties[i].duration--;
            if (treaties[i].duration == 0) {
                char message[200];
                strcpy(message, "Treaty expired: ");
                strcat(message, treaties[i].type);
                strcat(message, " between ");
                strcat(message, treaties[i].kingdom1);
                strcat(message, " and ");
                strcat(message, treaties[i].kingdom2);
                displayEvent("TREATY EXPIRED", message);
                
                // Make sure we don't try to access beyond the array
                if (i < treatyCount - 1 && treatyCount > 0) {
                    treaties[i] = treaties[treatyCount - 1];
                }
                
                if (treatyCount > 0) {
                    treatyCount--;
                }
                
                // Make sure i doesn't go negative
                if (i > 0) {
                    i--;
                } else {
                    i = 0;
                }
            }
        }
    }
}