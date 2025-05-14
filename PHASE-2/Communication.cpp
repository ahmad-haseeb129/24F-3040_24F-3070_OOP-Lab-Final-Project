#define _CRT_SECURE_NO_WARNINGS
#include "Stronghold.h"
#include <iostream>
#include <string> 

Communication::Communication() {
    messageCount = 0;
    for (int i = 0; i < MAX_MESSAGES; i++) {
        messages[i].sender[0] = '\0';
        messages[i].receiver[0] = '\0';
        messages[i].content[0] = '\0';
        messages[i].turn = 0;
    }
}

void Communication::sendMessage(const char* sender, const char* receiver, const char* content, int turn) {
    if (messageCount >= MAX_MESSAGES) {
        displayEvent("ERROR", "Message storage full!");
        return;
    }
    if (strlen(content) >= MAX_CONTENT_LEN) {
        displayEvent("ERROR", "Message too long!");
        return;
    }
    strncpy(messages[messageCount].sender, sender, MAX_NAME_LEN - 1);
    messages[messageCount].sender[MAX_NAME_LEN - 1] = '\0';
    strncpy(messages[messageCount].receiver, receiver, MAX_NAME_LEN - 1);
    messages[messageCount].receiver[MAX_NAME_LEN - 1] = '\0';
    strncpy(messages[messageCount].content, content, MAX_CONTENT_LEN - 1);
    messages[messageCount].content[MAX_CONTENT_LEN - 1] = '\0';
    messages[messageCount].turn = turn;
    messageCount++;
    char message[300];
    strcpy(message, "Message sent to ");
    strcat(message, receiver);
    strcat(message, ": ");
    strcat(message, content);
    displayEvent("MESSAGE SENT", message);
}

void Communication::displayMessages(const char* kingdom) {
    bool hasMessages = false;
    char buffer[500];
    strcpy(buffer, "Messages for ");
    strcat(buffer, kingdom);
    strcat(buffer, ":\n");
    for (int i = 0; i < messageCount; i++) {
        if (strcmp(messages[i].receiver, kingdom) == 0) {
            hasMessages = true;
            char temp[300];
            strcpy(temp, "Turn ");
            std::string turnStr = std::to_string(messages[i].turn);
            strcat(temp, turnStr.c_str());
            strcat(temp, " from ");
            strcat(temp, messages[i].sender);
            strcat(temp, ": ");
            strcat(temp, messages[i].content);
            strcat(temp, "\n");
            if (strlen(buffer) + strlen(temp) < 500) {
                strcat(buffer, temp);
            }
        }
    }
    if (hasMessages) {
        displayEvent("INBOX", buffer);
    }
    else {
        char message[100];
        strcpy(message, "No messages for ");
        strcat(message, kingdom);
        displayEvent("INBOX", message);
    }
}