#define _CRT_SECURE_NO_WARNINGS
#include "Stronghold.h"
#include <cstring>
#include <iostream>
#include <string> 

Conflict::Conflict() {
    warCount = 0;
    for (int i = 0; i < MAX_CONFLICTS; i++) {
        wars[i].attacker[0] = '\0';
        wars[i].defender[0] = '\0';
        wars[i].turnStarted = 0;
    }
}

void Conflict::declareWar(const char* attacker, const char* defender, int turn, Military* attackerMil,
    Military* defenderMil, Resources* attackerRes, Resources* defenderRes, Alliance* alliance) {
    if (warCount >= MAX_CONFLICTS) {
        displayEvent("ERROR", "War storage full!");
        return;
    }
    if (alliance->isAllied(attacker, defender)) {
        alliance->breakTreaty(attacker, defender);
        attackerMil->morale = attackerMil->morale > 20 ? attackerMil->morale - 20 : 0; // Penalty for betrayal
        alliance->reputation = alliance->reputation > 15 ? alliance->reputation - 15 : 0;
    }
    strncpy(wars[warCount].attacker, attacker, MAX_NAME_LEN - 1);
    wars[warCount].attacker[MAX_NAME_LEN - 1] = '\0';
    strncpy(wars[warCount].defender, defender, MAX_NAME_LEN - 1);
    wars[warCount].defender[MAX_NAME_LEN - 1] = '\0';
    wars[warCount].turnStarted = turn;
    warCount++;
    char message[100];
    strcpy(message, attacker);
    strcat(message, " declares war on ");
    strcat(message, defender);
    displayEvent("WAR DECLARED", message);
    resolveBattle(attacker, defender, attackerMil, defenderMil, attackerRes, defenderRes, alliance);
}

void Conflict::resolveBattle(const char* attacker, const char* defender, Military* attackerMil,
    Military* defenderMil, Resources* attackerRes, Resources* defenderRes, Alliance* alliance) {
    int attackerStrength = attackerMil->soldiers[0] * (attackerMil->morale / 100) * (attackerMil->training[0] / 100);
    int defenderStrength = defenderMil->soldiers[0] * (defenderMil->morale / 100) * (defenderMil->training[0] / 100);
    if (attackerStrength > defenderStrength) {
        int losses = defenderMil->soldiers[0] / 4;
        defenderMil->soldiers[0] = defenderMil->soldiers[0] > losses ? defenderMil->soldiers[0] - losses : 0;
        defenderMil->morale = defenderMil->morale > 10 ? defenderMil->morale - 10 : 0;
        defenderRes->resourceTypes[0] = defenderRes->resourceTypes[0] > 100 ? defenderRes->resourceTypes[0] - 100 : 0;
        char message[200];
        strcpy(message, attacker);
        strcat(message, " wins battle against ");
        strcat(message, defender);
        strcat(message, "! ");
        std::string lossStr = std::to_string(losses); 
        strcat(message, lossStr.c_str());
        strcat(message, " soldiers lost.");
        displayEvent("BATTLE RESULT", message);
    }
    else {
        int losses = attackerMil->soldiers[0] / 4;
        attackerMil->soldiers[0] = attackerMil->soldiers[0] > losses ? attackerMil->soldiers[0] - losses : 0;
        attackerMil->morale = attackerMil->morale > 10 ? attackerMil->morale - 10 : 0;
        attackerRes->resourceTypes[0] = attackerRes->resourceTypes[0] > 100 ? attackerRes->resourceTypes[0] - 100 : 0;
        char message[200];
        strcpy(message, defender);
        strcat(message, " repels ");
        strcat(message, attacker);
        strcat(message, "! ");
        std::string lossStr = std::to_string(losses);
        strcat(message, lossStr.c_str());
        strcat(message, " soldiers lost.");
        displayEvent("BATTLE RESULT", message);
    }
}