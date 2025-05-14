#include "Stronghold.h"
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

Resources::Resources() {
    resourceCount = 4;  // food, wood, stone, iron
    resourceTypes = new int[resourceCount];
    productionRates = new int[resourceCount];
    consumptionRates = new int[resourceCount];
    storage = new int[resourceCount];
    productionEfficiency = new float[resourceCount];
    hasGathered = new bool[resourceCount];

    for (int i = 0; i < resourceCount; i++) {
        resourceTypes[i] = 1000;
        storage[i] = 5000;
        productionEfficiency[i] = 1.0f;
        hasGathered[i] = false;
    }

    // Set specific initial production rates
    productionRates[0] = 1500;  // Food - base production of 1500
    productionRates[1] = 100;   // Wood
    productionRates[2] = 80;    // Stone
    productionRates[3] = 50;    // Steel

    // Set initial consumption rates (will be updated based on population)
    consumptionRates[0] = 0;    // Food - will be calculated based on population
    consumptionRates[1] = 50;   // Wood
    consumptionRates[2] = 40;   // Stone
    consumptionRates[3] = 25;   // Steel
}

Resources::~Resources() {
    delete[] resourceTypes;
    delete[] productionRates;
    delete[] consumptionRates;
    delete[] storage;
    delete[] productionEfficiency;
    delete[] hasGathered;
}

void Resources::resetGatherStatus() {
    for (int i = 0; i < resourceCount; i++) {
        hasGathered[i] = false;
    }
}

void Resources::gatherResource(int resourceType) {
    if (resourceType < 0 || resourceType >= resourceCount) {
        displayEvent("ERROR", "Invalid resource type!");
        return;
    }

    if (hasGathered[resourceType]) {
        displayEvent("ERROR", "This resource has already been gathered this turn!");
        return;
    }

    int produced = (int)(productionRates[resourceType] * productionEfficiency[resourceType]);
    if (resourceTypes[resourceType] + produced <= storage[resourceType]) {
        resourceTypes[resourceType] += produced;
        hasGathered[resourceType] = true;

        string resourceName;
        switch(resourceType) {
            case 0: resourceName = "Food"; break;
            case 1: resourceName = "Wood"; break;
            case 2: resourceName = "Stone"; break;
            case 3: resourceName = "Steel"; break;
        }

        string message = "Gathered " + to_string(produced) + " " + resourceName;
        message += "\nNew total: " + to_string(resourceTypes[resourceType]);
        displayEvent("RESOURCE GATHERED", message.c_str());
    } else {
        displayEvent("STORAGE FULL", "Not enough storage capacity for more resources!");
    }
}

void Resources::gather() {
    // Calculate production for each resource
    for (int i = 0; i < resourceCount; i++) {
        int produced = (int)(productionRates[i] * productionEfficiency[i]);
        
        // Check if we can store the produced amount
        if (resourceTypes[i] + produced <= storage[i]) {
            resourceTypes[i] += produced;
        } else {
            // If we can't store everything, only store what we can
            resourceTypes[i] = storage[i];
            
            string resourceName;
            switch(i) {
                case 0: resourceName = "Food"; break;
                case 1: resourceName = "Wood"; break;
                case 2: resourceName = "Stone"; break;
                case 3: resourceName = "Steel"; break;
            }
            string message = resourceName + " storage is full!\n";
            message += "Current: " + to_string(resourceTypes[i]) + "\n";
            message += "Storage Capacity: " + to_string(storage[i]);
            displayEvent("STORAGE FULL", message.c_str());
        }
    }

    // After production, check resource status
    for (int i = 0; i < resourceCount; i++) {
        // Calculate net change per turn
        int netChange = (int)(productionRates[i] * productionEfficiency[i]) - consumptionRates[i];
        
        if (netChange < 0 && resourceTypes[i] < consumptionRates[i] * 3) {
            string resourceName;
            switch(i) {
                case 0: resourceName = "Food"; break;
                case 1: resourceName = "Wood"; break;
                case 2: resourceName = "Stone"; break;
                case 3: resourceName = "Steel"; break;
            }
            string message = resourceName + " production is insufficient!\n";
            message += "Production: " + to_string((int)(productionRates[i] * productionEfficiency[i])) + "\n";
            message += "Consumption: " + to_string(consumptionRates[i]) + "\n";
            message += "Net change per turn: " + to_string(netChange);
            displayEvent("PRODUCTION WARNING", message.c_str());
        }
    }
}

void Resources::consume(SocialClass* peasants, SocialClass* merchants, SocialClass* nobles) {
    // Calculate new consumption rates based on population
    // Food consumption - exactly as specified
    consumptionRates[0] = peasants->population +     // Peasants consume 1 unit
                         merchants->population +      // Merchants consume 1 unit
                         (nobles->population * 2);    // Nobles consume 2 units

    // Wood consumption
    consumptionRates[1] = (peasants->population / 20) +    // Basic housing and tools
                         (merchants->population / 10) +     // Shops and trade
                         (nobles->population);             // Luxury housing

    // Stone consumption
    consumptionRates[2] = (peasants->population / 40) +    // Basic infrastructure
                         (merchants->population / 20) +     // Markets and warehouses
                         (nobles->population / 2);         // Mansions and fortifications

    // Steel consumption
    consumptionRates[3] = (peasants->population / 100) +   // Basic tools
                         (merchants->population / 50) +     // Trade goods
                         (nobles->population / 4);         // Weapons and luxury items

    // Apply consumption for each resource
    for (int i = 0; i < resourceCount; i++) {
        // Check if we're running low on resources
        if (resourceTypes[i] < consumptionRates[i]) {
            string resourceName;
            switch(i) {
                case 0: resourceName = "Food"; break;
                case 1: resourceName = "Wood"; break;
                case 2: resourceName = "Stone"; break;
                case 3: resourceName = "Steel"; break;
            }
            string message = resourceName + " supplies are critically low!\n";
            message += "Available: " + to_string(resourceTypes[i]) + "\n";
            message += "Required: " + to_string(consumptionRates[i]) + " per turn";
            displayEvent("RESOURCE WARNING", message.c_str());
        }
        
        // Consume resources
        resourceTypes[i] = max(0, resourceTypes[i] - consumptionRates[i]);
    }
}

void Resources::updateProductionEfficiency(SocialClass* peasants, SocialClass* merchants) {
    // Food production efficiency affected by peasant happiness and merchant trade
    // Increased base efficiency and happiness impact
    productionEfficiency[0] = 1.2f + (peasants->happiness / 100.0f) * 0.8f;  // Can range from 1.2 to 2.0
    
    // Wood production affected by peasant population and merchant investment
    productionEfficiency[1] = 1.0f + (peasants->population / 1000.0f) * 0.3f;
    
    // Stone production affected by peasant workforce and merchant organization
    productionEfficiency[2] = 1.0f + (merchants->influence / 100.0f) * 0.4f;
    
    // Steel production affected by merchant wealth and overall development
    productionEfficiency[3] = 1.0f + (merchants->wealth / 100.0f) * 0.6f;
}

void Resources::trade() {
    cout << "\nTRADE RESOURCES";
    cout << "\nWhat resource to give:";
    cout << "\n1. Food: " << resourceTypes[0];
    cout << "\n2. Wood: " << resourceTypes[1];
    cout << "\n3. Stone: " << resourceTypes[2];
    cout << "\n4. Steel: " << resourceTypes[3];
    cout << "\nChoice (1-4): ";
    
    int giveType;
    cin >> giveType;
    
    if (giveType < 1 || giveType > 4) {
        displayEvent("ERROR", "Invalid resource type!");
        return;
    }
    
    cout << "Amount to give: ";
    int giveAmount;
    cin >> giveAmount;
    
    if (giveAmount <= 0) {
        displayEvent("ERROR", "Amount must be positive!");
        return;
    }
    
    if (resourceTypes[giveType - 1] < giveAmount) {
        displayEvent("ERROR", "Insufficient resources!");
        return;
    }
    
    cout << "\nWhat resource to receive:";
    cout << "\n1. Food";
    cout << "\n2. Wood";
    cout << "\n3. Stone";
    cout << "\n4. Steel";
    cout << "\nChoice (1-4): ";
    
    int receiveType;
    cin >> receiveType;
    
    if (receiveType < 1 || receiveType > 4 || receiveType == giveType) {
        displayEvent("ERROR", "Invalid resource selection!");
        return;
    }
    
    // Calculate trade rates based on resource values
    float tradeRates[4] = {1.0, 1.2, 1.5, 2.0};  // Base value multipliers
    
    // Calculate received amount based on relative values
    int receiveAmount = (int)(giveAmount * (tradeRates[giveType-1] / tradeRates[receiveType-1]));
    
    // Execute trade
    resourceTypes[giveType - 1] -= giveAmount;
    resourceTypes[receiveType - 1] += receiveAmount;
    
    string message = "Traded " + to_string(giveAmount) + " ";
    switch(giveType) {
        case 1: message += "food"; break;
        case 2: message += "wood"; break;
        case 3: message += "stone"; break;
        case 4: message += "steel"; break;
    }
    message += " for " + to_string(receiveAmount) + " ";
    switch(receiveType) {
        case 1: message += "food"; break;
        case 2: message += "wood"; break;
        case 3: message += "stone"; break;
        case 4: message += "steel"; break;
    }
    displayEvent("TRADE COMPLETE", message.c_str());
}