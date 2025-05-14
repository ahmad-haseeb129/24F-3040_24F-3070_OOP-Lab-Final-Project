#define _CRT_SECURE_NO_WARNINGS
#include "Stronghold.h"
#include <cstring>
#include <iostream>

Market::Market() {
    offerCount = 0;
    embargoCount = 0;
    for (int i = 0; i < MAX_OFFERS; i++) {
        offers[i].sender[0] = '\0';
        offers[i].receiver[0] = '\0';
        offers[i].resourceType = 0;
        offers[i].giveAmount = 0;
        offers[i].receiveType = 0;
        offers[i].receiveAmount = 0;
        offers[i].isSmuggling = false;
    }
    for (int i = 0; i < MAX_NAME_LEN; i++) {
        embargoes[i][0] = '\0';
    }
}

void Market::proposeTrade(const char* sender, const char* receiver, int resourceType, int giveAmount,
    int receiveType, int receiveAmount, bool isSmuggling) {
    if (offerCount >= MAX_OFFERS) {
        displayEvent("ERROR", "Trade offer storage full!");
        return;
    }
    if (isEmbargoed(sender, receiver)) {
        char message[100];
        strcpy(message, "Cannot trade with ");
        strcat(message, receiver);
        strcat(message, " due to embargo!");
        displayEvent("ERROR", message);
        return;
    }
    strncpy(offers[offerCount].sender, sender, MAX_NAME_LEN - 1);
    offers[offerCount].sender[MAX_NAME_LEN - 1] = '\0';
    strncpy(offers[offerCount].receiver, receiver, MAX_NAME_LEN - 1);
    offers[offerCount].receiver[MAX_NAME_LEN - 1] = '\0';
    offers[offerCount].resourceType = resourceType;
    offers[offerCount].giveAmount = giveAmount;
    offers[offerCount].receiveType = receiveType;
    offers[offerCount].receiveAmount = receiveAmount;
    offers[offerCount].isSmuggling = isSmuggling;
    offerCount++;
    char message[200];
    strcpy(message, "Trade proposed by ");
    strcat(message, sender);
    strcat(message, " to ");
    strcat(message, receiver);
    strcat(message, isSmuggling ? " (Smuggling)" : "");
    displayEvent("TRADE PROPOSED", message);
}

void Market::acceptTrade(int offerIndex, Resources* senderRes, Resources* receiverRes, Economy* senderEco) {
    if (offerIndex < 0 || offerIndex >= offerCount) {
        displayEvent("ERROR", "Invalid trade offer!");
        return;
    }
    TradeOffer& offer = offers[offerIndex];
    if (senderRes->resourceTypes[offer.resourceType] < offer.giveAmount ||
        receiverRes->resourceTypes[offer.receiveType] < offer.receiveAmount) {
        displayEvent("ERROR", "Insufficient resources for trade!");
        return;
    }
    if (offer.isSmuggling) {
        processSmuggling(senderRes, senderEco, offerIndex);
    }
    else {
        senderRes->resourceTypes[offer.resourceType] -= offer.giveAmount;
        receiverRes->resourceTypes[offer.resourceType] += offer.giveAmount;
        receiverRes->resourceTypes[offer.receiveType] -= offer.receiveAmount;
        senderRes->resourceTypes[offer.receiveType] += offer.receiveAmount;
        char message[200];
        strcpy(message, "Trade completed between ");
        strcat(message, offer.sender);
        strcat(message, " and ");
        strcat(message, offer.receiver);
        displayEvent("TRADE COMPLETED", message);
    }
    offers[offerIndex] = offers[offerCount - 1];
    offerCount--;
}

bool Market::isEmbargoed(const char* kingdom1, const char* kingdom2) {
    for (int i = 0; i < embargoCount; i++) {
        if ((strcmp(embargoes[i], kingdom1) == 0 && strcmp(embargoes[i + 1], kingdom2) == 0) ||
            (strcmp(embargoes[i], kingdom2) == 0 && strcmp(embargoes[i + 1], kingdom1) == 0)) {
            return true;
        }
    }
    return false;
}

void Market::addEmbargo(const char* kingdom1, const char* kingdom2) {
    if (embargoCount >= MAX_NAME_LEN - 1) {
        displayEvent("ERROR", "Embargo list full!");
        return;
    }
    strncpy(embargoes[embargoCount], kingdom1, MAX_NAME_LEN - 1);
    embargoes[embargoCount][MAX_NAME_LEN - 1] = '\0';
    embargoCount++;
    strncpy(embargoes[embargoCount], kingdom2, MAX_NAME_LEN - 1);
    embargoes[embargoCount][MAX_NAME_LEN - 1] = '\0';
    embargoCount++;
    char message[100];
    strcpy(message, "Embargo imposed between ");
    strcat(message, kingdom1);
    strcat(message, " and ");
    strcat(message, kingdom2);
    displayEvent("EMBARGO", message);
}

void Market::processSmuggling(Resources* senderRes, Economy* senderEco, int offerIndex) {
    TradeOffer& offer = offers[offerIndex];
    if (rand() % 100 < 20) { // 20% chance of detection
        senderEco->treasury = senderEco->treasury > 100 ? senderEco->treasury - 100 : 0;
        char message[100];
        strcpy(message, "Smuggling detected! ");
        strcat(message, offer.sender);
        strcat(message, " loses 100 gold.");
        displayEvent("SMUGGLING DETECTED", message);
    }
    else {
        senderRes->resourceTypes[offer.resourceType] -= offer.giveAmount;
        senderRes->resourceTypes[offer.receiveType] += offer.receiveAmount;
        char message[100];
        strcpy(message, "Smuggling successful for ");
        strcat(message, offer.sender);
        displayEvent("SMUGGLING SUCCESS", message);
    }
}