#include "Stronghold.h"

EventSystem::EventSystem() {
    eventCount = 4;  // famine, plague, war, drought
    activeEvents = new bool[eventCount];
    eventSeverity = new int[eventCount];
    eventDuration = new int[eventCount];

    for (int i = 0; i < eventCount; i++) {
        activeEvents[i] = false;
        eventSeverity[i] = 0;
        eventDuration[i] = 0;
    }
}

EventSystem::~EventSystem() {
    delete[] activeEvents;
    delete[] eventSeverity;
    delete[] eventDuration;
}

void EventSystem::triggerRandomEvent() {
    int event = rand() % eventCount;
    if (!activeEvents[event]) {
        activeEvents[event] = true;
        eventSeverity[event] = rand() % 100 + 1;
        eventDuration[event] = rand() % 5 + 1;
    }
}

void EventSystem::resolveEvents() {
    for (int i = 0; i < eventCount; i++) {
        if (activeEvents[i]) {
            if (eventDuration[i] > 0) {
                eventDuration[i]--;
            }
            else {
                activeEvents[i] = false;
                eventSeverity[i] = 0;
            }
        }
    }
}

void EventSystem::calculateImpact() {
    for (int i = 0; i < eventCount; i++) {
        if (activeEvents[i]) {
            // Impact varies based on event type and severity
            switch (i) {
            case 0: // famine
                break;
            case 1: // plague
                break;
            case 2: // war
                break;
            case 3: // drought
                break;
            }
        }
    }
}