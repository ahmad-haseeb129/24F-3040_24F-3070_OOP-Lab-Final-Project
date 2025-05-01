#include "Stronghold.h"
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

EventSystem::EventSystem() {
    eventCount = 8;  // famine, plague, war, drought, storm, rebellion, trade boom, good harvest
    activeEvents = new bool[eventCount];
    eventSeverity = new int[eventCount];
    eventDuration = new int[eventCount];
    eventDescriptions = new string[eventCount];

    // Initialize event descriptions
    eventDescriptions[0] = "Famine";
    eventDescriptions[1] = "Plague";
    eventDescriptions[2] = "War";
    eventDescriptions[3] = "Drought";
    eventDescriptions[4] = "Storm";
    eventDescriptions[5] = "Rebellion";
    eventDescriptions[6] = "Trade Boom";
    eventDescriptions[7] = "Good Harvest";

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
    delete[] eventDescriptions;
}

void EventSystem::triggerRandomEvent() {
    // Only trigger new event if we're not overwhelmed with active events
    int activeCount = 0;
    for (int i = 0; i < eventCount; i++) {
        if (activeEvents[i]) activeCount++;
    }
    
    if (activeCount >= 3) return; // Max 3 concurrent events

    // Higher chance for events during certain conditions
    int eventChance = rand() % 100;
    
    // Base 15% chance for an event
    if (eventChance < 15) {
        int event = rand() % eventCount;
        if (!activeEvents[event]) {
            activeEvents[event] = true;
            eventSeverity[event] = rand() % 100 + 1;
            eventDuration[event] = rand() % 5 + 1;  // 1-5 turns
        }
    }
}

void EventSystem::resolveEvents() {
    for (int i = 0; i < eventCount; i++) {
        if (activeEvents[i] && eventDuration[i] > 0) {
            eventDuration[i]--;
            if (eventDuration[i] == 0) {
                activeEvents[i] = false;
                eventSeverity[i] = 0;
            }
        }
    }
}

void EventSystem::calculateImpact(Resources* res, Population* pop, Economy* eco, Military* mil,
                                SocialClass* peasants, SocialClass* merchants, SocialClass* nobles) {
    stringstream eventLog;
    bool hasEvents = false;

    for (int i = 0; i < eventCount; i++) {
        if (activeEvents[i]) {
            hasEvents = true;
            float severityMultiplier = eventSeverity[i] / 100.0f;
            
            switch (i) {
                case 0: { // Famine
                    int foodLoss = (int)(res->resourceTypes[0] * 0.3f * severityMultiplier);
                    res->resourceTypes[0] -= foodLoss;
                    peasants->happiness = max(0, min(100, peasants->happiness - (int)(15 * severityMultiplier)));
                    
                    eventLog << "Famine strikes the kingdom!\n";
                    eventLog << "- Lost " << foodLoss << " food\n";
                    eventLog << "- Peasant happiness decreased\n";
                    eventLog << "- Duration: " << eventDuration[i] << " turns remaining\n";
                    break;
                }
                case 1: { // Plague
                    int popLoss = (int)(peasants->population * 0.1f * severityMultiplier);
                    peasants->population = max(0, peasants->population - popLoss);
                    merchants->population = max(0, merchants->population - (int)(merchants->population * 0.05f * severityMultiplier));
                    nobles->population = max(0, nobles->population - (int)(nobles->population * 0.02f * severityMultiplier));
                    
                    eventLog << "A plague spreads through the kingdom!\n";
                    eventLog << "- " << popLoss << " peasants perished\n";
                    eventLog << "- Merchants and nobles also affected\n";
                    eventLog << "- Duration: " << eventDuration[i] << " turns remaining\n";
                    break;
                }
                case 2: { // War
                    if (mil->soldiers[0] > 0) {
                        int casualities = (int)(mil->soldiers[0] * 0.2f * severityMultiplier);
                        mil->soldiers[0] = max(0, mil->soldiers[0] - casualities);
                        eco->treasury = max(0, eco->treasury - (int)(eco->treasury * 0.15f * severityMultiplier));
                        
                        eventLog << "War breaks out!\n";
                        eventLog << "- Lost " << casualities << " soldiers\n";
                        eventLog << "- Treasury depleted from war expenses\n";
                        eventLog << "- Duration: " << eventDuration[i] << " turns remaining\n";
                    }
                    break;
                }
                case 3: { // Drought
                    int foodProdLoss = (int)(res->productionRates[0] * 0.4f * severityMultiplier);
                    res->productionRates[0] = max(0, res->productionRates[0] - foodProdLoss);
                    
                    eventLog << "Severe drought affects crops!\n";
                    eventLog << "- Food production reduced by " << foodProdLoss << "\n";
                    eventLog << "- Duration: " << eventDuration[i] << " turns remaining\n";
                    break;
                }
                case 4: { // Storm
                    int woodLoss = (int)(res->resourceTypes[1] * 0.2f * severityMultiplier);
                    int stoneLoss = (int)(res->resourceTypes[2] * 0.1f * severityMultiplier);
                    res->resourceTypes[1] = max(0, res->resourceTypes[1] - woodLoss);
                    res->resourceTypes[2] = max(0, res->resourceTypes[2] - stoneLoss);
                    
                    eventLog << "A devastating storm hits the kingdom!\n";
                    eventLog << "- Lost " << woodLoss << " wood and " << stoneLoss << " stone\n";
                    eventLog << "- Buildings damaged\n";
                    eventLog << "- Duration: " << eventDuration[i] << " turns remaining\n";
                    break;
                }
                case 5: { // Rebellion
                    if (peasants->happiness < 40 || merchants->happiness < 40) {
                        eco->treasury = max(0, eco->treasury - (int)(eco->treasury * 0.1f * severityMultiplier));
                        mil->morale = max(0, min(100, mil->morale - (int)(10 * severityMultiplier)));
                        
                        eventLog << "Civil unrest leads to rebellion!\n";
                        eventLog << "- Treasury looted\n";
                        eventLog << "- Military morale decreased\n";
                        eventLog << "- Duration: " << eventDuration[i] << " turns remaining\n";
                    }
                    break;
                }
                case 6: { // Trade Boom (Positive Event)
                    int profit = (int)(eco->treasury * 0.2f * severityMultiplier);
                    eco->treasury += profit;
                    merchants->happiness = max(0, min(100, merchants->happiness + (int)(10 * severityMultiplier)));
                    merchants->wealth = max(0, min(100, merchants->wealth + (int)(5 * severityMultiplier)));
                    
                    eventLog << "Trade routes flourish!\n";
                    eventLog << "- Treasury increased by " << profit << " gold\n";
                    eventLog << "- Merchant happiness and wealth improved\n";
                    eventLog << "- Duration: " << eventDuration[i] << " turns remaining\n";
                    break;
                }
                case 7: { // Good Harvest (Positive Event)
                    int foodBonus = (int)(res->productionRates[0] * 0.5f * severityMultiplier);
                    res->resourceTypes[0] += foodBonus;
                    peasants->happiness = max(0, min(100, peasants->happiness + (int)(10 * severityMultiplier)));
                    
                    eventLog << "Exceptional harvest season!\n";
                    eventLog << "- Gained " << foodBonus << " extra food\n";
                    eventLog << "- Peasant happiness improved\n";
                    eventLog << "- Duration: " << eventDuration[i] << " turns remaining\n";
                    break;
                }
            }
        }
    }

    if (hasEvents) {
        displayEvent("KINGDOM EVENTS", eventLog.str().c_str());
    }
}