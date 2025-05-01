#define _CRT_SECURE_NO_WARNINGS
#include "Stronghold.h"
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

// Forward declarations
void displayBox(const char* title);
void displayEvent(const char* event, const char* description);
void displayDetailedReports(Population* pop, Economy* eco, Military* mil, Resources* res, Leadership* lead, Bank* bank, SocialClass* peasants, SocialClass* merchants, SocialClass* nobles);
void saveGameState(const char* filename, Population* pop, Economy* eco, Military* mil, Resources* res, 
                  SocialClass* peasants, SocialClass* merchants, SocialClass* nobles, Leadership* lead, Bank* bank, EventSystem* events);
void loadGameState(const char* filename, Population* pop, Economy* eco, Military* mil, Resources* res,
                  SocialClass* peasants, SocialClass* merchants, SocialClass* nobles, Leadership* lead, Bank* bank, EventSystem* events);
void updateSocialClassHappiness(SocialClass* socialClass, Economy* eco, Resources* res);
void manageResources(Resources* res, Economy* eco);
void manageEconomy(Economy* eco, Resources* res);
void manageMilitary(Military* mil, Resources* res, Population* pop, Economy* eco);
void manageKingdom(Population* pop, Economy* eco, Military* mil, Resources* res, Leadership* lead, Bank* bank, SocialClass* peasants, SocialClass* merchants, SocialClass* nobles);
void manageLeadership(Leadership* lead, Population* pop, Economy* eco, Military* mil, SocialClass* peasants, SocialClass* merchants, SocialClass* nobles);
void manageBank(Bank* bank, Economy* eco);

// Global turn counter
int turn = 1;

void displayBox(const char* title) 
{
    cout << "\n+" << string(80, '-') << "+\n";
    cout << "| " << left << setw(78) << title << " |\n";
    cout << "+" << string(80, '-') << "+\n";
}

void displayEvent(const char* event, const char* description) 
{
    cout << "\n+" << string(80, '-') << "+\n";
    cout << "| " << left << setw(78) << event << " |\n";
    cout << "+" << string(80, '-') << "+\n";
    
    // Split description into words and wrap text
    string desc(description);
    istringstream words(desc);
    string word;
    string line;
    
    // Process each word
    while (words >> word) {
        // If adding this word would exceed line length
        if (line.length() + word.length() + 1 > 76) {  // +1 for the space
            // Output current line with proper padding
            cout << "| " << left << setw(78) << line << " |\n";
            line = word;  // Start new line with current word
        } else {
            // Add word to current line
            if (!line.empty()) {
                line += " ";  // Add space before word if line isn't empty
            }
            line += word;
        }
    }
    
    // Output last line if any
    if (!line.empty()) {
        cout << "| " << left << setw(78) << line << " |\n";
    }
    
    cout << "+" << string(80, '-') << "+\n";
}

void saveGameState(const char* filename, Population* pop, Economy* eco, Military* mil, Resources* res, 
                  SocialClass* peasants, SocialClass* merchants, SocialClass* nobles, Leadership* lead, Bank* bank, EventSystem* events) {
    ofstream save(filename);
    
    // Save turn number
    save << turn << "\n";
    
    // Save Population data (just total)
    save << pop->totalPopulation << "\n";
    
    // Save Economy data
    save << eco->treasury << " " << eco->taxRate << " " << eco->inflation << "\n";
    
    // Save Military data
    for (int i = 0; i < 5; i++) {
        save << mil->soldiers[i] << " ";
    }
    save << "\n";
    
    for (int i = 0; i < 5; i++) {
        save << mil->training[i] << " ";
    }
    save << "\n";
    
    save << mil->morale << " " << mil->cost << " " << mil->weaponCount << " "
         << mil->corruptionLevel << " " << mil->isLoyal << "\n";
    
    // Save weapons
    for (int i = 0; i < 10; i++) {
        save << mil->weapons[i] << " ";
    }
    save << "\n";
    
    // Save Resources data
    save << res->resourceCount << "\n";
    for (int i = 0; i < res->resourceCount; i++) {
        save << res->resourceTypes[i] << " ";
    }
    save << "\n";
    
    for (int i = 0; i < res->resourceCount; i++) {
        save << res->productionRates[i] << " ";
    }
    save << "\n";
    
    for (int i = 0; i < res->resourceCount; i++) {
        save << res->consumptionRates[i] << " ";
    }
    save << "\n";
    
    for (int i = 0; i < res->resourceCount; i++) {
        save << res->storage[i] << " ";
    }
    save << "\n";

    for (int i = 0; i < res->resourceCount; i++) {
        save << res->productionEfficiency[i] << " ";
    }
    save << "\n";

    for (int i = 0; i < res->resourceCount; i++) {
        save << res->hasGathered[i] << " ";
    }
    save << "\n";
    
    // Save Social Classes data with all attributes
    save << peasants->className << "\n";
    save << peasants->population << " " << peasants->happiness << " " << peasants->wealth << " " 
         << peasants->influence << " " << peasants->birthRate << " " << peasants->deathRate << " "
         << peasants->foodConsumption << " " << peasants->isRevolting << "\n";
         
    save << merchants->className << "\n";
    save << merchants->population << " " << merchants->happiness << " " << merchants->wealth << " " 
         << merchants->influence << " " << merchants->birthRate << " " << merchants->deathRate << " "
         << merchants->foodConsumption << " " << merchants->isRevolting << "\n";
         
    save << nobles->className << "\n";
    save << nobles->population << " " << nobles->happiness << " " << nobles->wealth << " " 
         << nobles->influence << " " << nobles->birthRate << " " << nobles->deathRate << " "
         << nobles->foodConsumption << " " << nobles->isRevolting << "\n";
    
    // Save Leadership data
    save << lead->currentLeader << "\n";
    save << lead->popularity << " " << lead->term << " " << lead->isCoup << " " << lead->supporterCount << "\n";
    for (int i = 0; i < lead->supporterCount; i++) {
        save << lead->supporters[i] << " ";
    }
    save << "\n";
    
    // Save Bank data
    save << bank->treasury << " " << bank->interestRate << " " << bank->activeLoanCount << "\n";
    for (int i = 0; i < bank->activeLoanCount; i++) {
        if (bank->borrowers[i] != "" && bank->loanAmounts[i] > 0) {
            save << bank->borrowers[i] << " " << bank->loanAmounts[i] << " " << bank->loanDurations[i] << "\n";
        }
    }
    save << bank->fraudAttempts << " " << bank->stolenAmount << "\n";
    
    // Save Event System data
    save << events->eventCount << "\n";
    for (int i = 0; i < events->eventCount; i++) {
        save << events->activeEvents[i] << " ";
    }
    save << "\n";
    
    for (int i = 0; i < events->eventCount; i++) {
        save << events->eventSeverity[i] << " ";
    }
    save << "\n";
    
    for (int i = 0; i < events->eventCount; i++) {
        save << events->eventDuration[i] << " ";
    }
    save << "\n";

    for (int i = 0; i < events->eventCount; i++) {
        save << events->eventDescriptions[i] << "\n";
    }
    
    save.close();
    displayEvent("GAME SAVED", "All game state data has been saved successfully!");
}

void loadGameState(const char* filename, Population* pop, Economy* eco, Military* mil, Resources* res,
                  SocialClass* peasants, SocialClass* merchants, SocialClass* nobles, Leadership* lead, Bank* bank, EventSystem* events) {
    ifstream load(filename);
    if (load.is_open()) {
        // Load turn number
        load >> turn;
        
        // Load Population data (just total)
        load >> pop->totalPopulation;
        
        // Load Economy data
        load >> eco->treasury >> eco->taxRate >> eco->inflation;
        
        // Load Military data
        for (int i = 0; i < 5; i++) {
            load >> mil->soldiers[i];
        }
        
        for (int i = 0; i < 5; i++) {
            load >> mil->training[i];
        }
        
        load >> mil->morale >> mil->cost >> mil->weaponCount 
             >> mil->corruptionLevel >> mil->isLoyal;
        
        // Load weapons
        for (int i = 0; i < 10; i++) {
            load >> mil->weapons[i];
        }
        
        // Load Resources data
        load >> res->resourceCount;
        for (int i = 0; i < res->resourceCount; i++) {
            load >> res->resourceTypes[i];
        }
        
        for (int i = 0; i < res->resourceCount; i++) {
            load >> res->productionRates[i];
        }
        
        for (int i = 0; i < res->resourceCount; i++) {
            load >> res->consumptionRates[i];
        }
        
        for (int i = 0; i < res->resourceCount; i++) {
            load >> res->storage[i];
        }

        for (int i = 0; i < res->resourceCount; i++) {
            load >> res->productionEfficiency[i];
        }

        for (int i = 0; i < res->resourceCount; i++) {
            load >> res->hasGathered[i];
        }
        
        // Load Social Classes data with all attributes
        string className;
        
        load >> className;
        delete[] peasants->className;
        peasants->className = new char[className.length() + 1];
        strcpy(peasants->className, className.c_str());
        load >> peasants->population >> peasants->happiness >> peasants->wealth 
             >> peasants->influence >> peasants->birthRate >> peasants->deathRate 
             >> peasants->foodConsumption >> peasants->isRevolting;
        
        load >> className;
        delete[] merchants->className;
        merchants->className = new char[className.length() + 1];
        strcpy(merchants->className, className.c_str());
        load >> merchants->population >> merchants->happiness >> merchants->wealth 
             >> merchants->influence >> merchants->birthRate >> merchants->deathRate 
             >> merchants->foodConsumption >> merchants->isRevolting;
        
        load >> className;
        delete[] nobles->className;
        nobles->className = new char[className.length() + 1];
        strcpy(nobles->className, className.c_str());
        load >> nobles->population >> nobles->happiness >> nobles->wealth 
             >> nobles->influence >> nobles->birthRate >> nobles->deathRate 
             >> nobles->foodConsumption >> nobles->isRevolting;
        
        // Load Leadership data
        string leaderName;
        load >> leaderName;
        delete[] lead->currentLeader;
        lead->currentLeader = new char[leaderName.length() + 1];
        strcpy(lead->currentLeader, leaderName.c_str());
        
        load >> lead->popularity >> lead->term >> lead->isCoup >> lead->supporterCount;
        delete[] lead->supporters;
        lead->supporters = new int[lead->supporterCount];
        for (int i = 0; i < lead->supporterCount; i++) {
            load >> lead->supporters[i];
        }
        
        // Load Bank data
        load >> bank->treasury >> bank->interestRate >> bank->activeLoanCount;
        
        // Clear existing loans
        for (int i = 0; i < Bank::MAX_LOANS; i++) {
            bank->borrowers[i] = "";
            bank->loanAmounts[i] = 0;
            bank->loanDurations[i] = 0;
        }
        
        // Load active loans
        string borrower;
        double amount;
        int duration;
        for (int i = 0; i < bank->activeLoanCount; i++) {
            load >> borrower >> amount >> duration;
            bank->borrowers[i] = borrower;
            bank->loanAmounts[i] = amount;
            bank->loanDurations[i] = duration;
        }
        
        load >> bank->fraudAttempts >> bank->stolenAmount;
        
        // Load Event System data
        load >> events->eventCount;
        delete[] events->activeEvents;
        delete[] events->eventSeverity;
        delete[] events->eventDuration;
        delete[] events->eventDescriptions;
        events->activeEvents = new bool[events->eventCount];
        events->eventSeverity = new int[events->eventCount];
        events->eventDuration = new int[events->eventCount];
        events->eventDescriptions = new string[events->eventCount];
        
        for (int i = 0; i < events->eventCount; i++) {
            load >> events->activeEvents[i];
        }
        
        for (int i = 0; i < events->eventCount; i++) {
            load >> events->eventSeverity[i];
        }
        
        for (int i = 0; i < events->eventCount; i++) {
            load >> events->eventDuration[i];
        }

        for (int i = 0; i < events->eventCount; i++) {
            load.ignore(); // Skip any whitespace
            getline(load, events->eventDescriptions[i]);
        }
        
        load.close();
        displayEvent("GAME LOADED", "All game state data has been restored successfully!");
    }
    else {
        displayEvent("LOAD ERROR", "No saved game found or file could not be opened.");
    }
}

void updateTotalPopulation(Population* pop, SocialClass* peasants, SocialClass* merchants, SocialClass* nobles) {
    // Total population is just the sum of all social classes
    pop->totalPopulation = peasants->population + merchants->population + nobles->population;
}

void updateInflation(Economy* eco) 
{
    // Base inflation change based on tax rate
    if (eco->taxRate > 30) {
        eco->inflation += (eco->taxRate - 30) / 2;  // Higher taxes increase inflation faster
    } else if (eco->taxRate < 20) {
        eco->inflation = max(0, eco->inflation - 1);  // Lower taxes slowly reduce inflation
    }
    
    // Cap inflation at reasonable levels
    if (eco->inflation > 100) eco->inflation = 100;
    if (eco->inflation < 0) eco->inflation = 0;
   
}

void updateSocialClassHappiness(SocialClass* socialClass, Economy* eco, Resources* res) {
    // Base happiness affected by tax rate and inflation
    int happinessChange = 0;
    
    // Different classes react differently to conditions
    if (strcmp(socialClass->className, "Peasants") == 0) {
        // Peasants care most about food, tax rate, and inflation
        happinessChange -= (eco->taxRate / 5);  // Reduced from /3
        happinessChange -= (eco->inflation / 8);  // Reduced from /5
        
        // Food impact is now more positive
        if (res->resourceTypes[0] > 200) {
            happinessChange += 8;  // Increased from 5
        }
        if (res->resourceTypes[0] > 500) {
            happinessChange += 5;  // Additional bonus for abundant food
        }
        
        // Natural population growth/decline for peasants
        int growthRate = 0;
        
        // Base growth rate depends on happiness
        if (socialClass->happiness > 70) growthRate = 3;
        else if (socialClass->happiness > 50) growthRate = 2;
        else if (socialClass->happiness > 30) growthRate = 1;
        else if (socialClass->happiness > 10) growthRate = 0;
        else growthRate = -1;  // Only decline at very low happiness
        
        // Food bonus
        if (res->resourceTypes[0] > 500) growthRate += 1;
        
        // Apply growth/decline with smaller percentage
        int populationChange = (socialClass->population * growthRate) / 200;  // Reduced from /100 to /200
        socialClass->population = max(0, socialClass->population + populationChange);
        
        // Warnings and crisis events
        if (res->resourceTypes[0] < 100) {
            displayEvent("PEASANT CONCERN", "Low food supplies are causing worry among peasants.");
            happinessChange -= 5;  // Reduced from -10
        }
        
        if (eco->inflation > 50) {
            displayEvent("ECONOMIC STRAIN", "High inflation is affecting peasant families.");
            happinessChange -= 5;  // Reduced from -8
        }
    }
    else if (strcmp(socialClass->className, "Merchants") == 0) {
        // Merchants care about economy and trade
        happinessChange = (eco->treasury > 1000) ? 2 : -2;  // Reduced from ±3
        happinessChange -= (eco->inflation / 6);  // Reduced from /4
        happinessChange += (res->resourceTypes[1] + res->resourceTypes[2]) / 400; // Reduced from /300
        
        // Merchant population changes
        int growthRate = 0;
        if (eco->treasury > 1500) growthRate = 2;
        else if (eco->treasury > 1000) growthRate = 1;
        else if (eco->treasury > 500) growthRate = 0;
        else growthRate = -1;
        
        if (eco->inflation < 30) growthRate += 1;
        
        // Apply growth/decline
        int populationChange = (socialClass->population * growthRate) / 200;  // Reduced rate
        socialClass->population = max(0, socialClass->population + populationChange);
        
        if (eco->taxRate > 40) {
            displayEvent("MERCHANT CONCERN", "High tax rates are affecting trade.");
            happinessChange -= 5;  // Reduced from -8
        }
    }
    else if (strcmp(socialClass->className, "Nobles") == 0) {
        // Nobles care about wealth and stability
        happinessChange = (eco->treasury > 2000) ? 3 : -3;  // Reduced from ±5
        happinessChange -= (eco->inflation / 8);  // Reduced from /6
        
        // Noble population changes very slowly
        int growthRate = 0;
        if (eco->treasury > 2500 && socialClass->happiness > 70) growthRate = 1;
        else if (eco->treasury < 500 && socialClass->happiness < 30) growthRate = -1;
        
        // Apply growth/decline
        int populationChange = (socialClass->population * growthRate) / 400;  // Very slow rate
        socialClass->population = max(0, socialClass->population + populationChange);
    }
    
    // Update happiness with bounds checking
    socialClass->happiness = max(0, min(100, socialClass->happiness + happinessChange));
    
    // Population decline due to severe conditions - now less punishing
    if (socialClass->happiness < 15 || eco->inflation > 95) {  // More extreme thresholds
        // Calculate leaving population based on both happiness and inflation
        double leavingPercentage = 0.03;  // Reduced from 0.05
        
        if (eco->inflation > 95) {
            leavingPercentage += (eco->inflation - 95) / 300.0;  // Reduced impact
        }
        
        if (socialClass->happiness < 15) {
            leavingPercentage += (15 - socialClass->happiness) / 300.0;  // Reduced impact
        }
        
        // Make sure at least 1 person leaves if conditions are terrible
        int leavingPop = max(1, static_cast<int>(socialClass->population * leavingPercentage));
        if (leavingPop > 0) {
            socialClass->population = max(0, socialClass->population - leavingPop);
            
            if (socialClass->population > 0) {
                string title = string(socialClass->className) + " POPULATION DECLINE";
                string message = "Some " + string(socialClass->className) + " are leaving the kingdom.\n\n";
                message += "Lost Population: " + to_string(leavingPop) + "\n";
                message += "Remaining " + string(socialClass->className) + ": " + to_string(socialClass->population);
                displayEvent(title.c_str(), message.c_str());
            }
        }
    }
}

void updateKingdomState(Population* pop, Economy* eco, Resources* res, 
                       SocialClass* peasants, SocialClass* merchants, SocialClass* nobles) {
    // Update inflation based on tax rate
    updateInflation(eco);
    
    // Update social classes - they handle their own population changes
    updateSocialClassHappiness(peasants, eco, res);
    updateSocialClassHappiness(merchants, eco, res);
    updateSocialClassHappiness(nobles, eco, res);
    
    // Update resource production efficiency based on class status
    res->updateProductionEfficiency(peasants, merchants);
    
    // Gather resources with updated efficiency
    res->gather();
    
    // Consume resources based on class populations
    res->consume(peasants, merchants, nobles);
    
    // Update total population count
    updateTotalPopulation(pop, peasants, merchants, nobles);
    
    // Display economic status if significant changes
    if (eco->inflation > 50) {
        string message = "Current Inflation: " + to_string(eco->inflation) + "%\n";
        message += "High inflation is affecting the kingdom's economy!";
        displayEvent("ECONOMIC WARNING", message.c_str());
    }
    
    // Display resource warnings if any are critically low
    for (int i = 0; i < res->resourceCount; i++) {
        if (res->resourceTypes[i] < res->consumptionRates[i] * 2) {
            string resourceName;
            switch(i) {
                case 0: resourceName = "Food"; break;
                case 1: resourceName = "Wood"; break;
                case 2: resourceName = "Stone"; break;
                case 3: resourceName = "Steel"; break;
            }
            string message = resourceName + " supplies are critically low!\n";
            message += "Current: " + to_string(res->resourceTypes[i]) + "\n";
            message += "Consumption: " + to_string(res->consumptionRates[i]) + " per turn";
            displayEvent("RESOURCE WARNING", message.c_str());
        }
    }
}

void manageResources(Resources* res, Economy* eco) {
    while (true) {
        displayBox("RESOURCE MANAGEMENT");
        cout << "\nCurrent Resources:";
        cout << "\n1. Food: " << res->resourceTypes[0] << " (Can" << (res->hasGathered[0] ? "not" : "") << " gather)";
        cout << "\n2. Wood: " << res->resourceTypes[1] << " (Can" << (res->hasGathered[1] ? "not" : "") << " gather)";
        cout << "\n3. Stone: " << res->resourceTypes[2] << " (Can" << (res->hasGathered[2] ? "not" : "") << " gather)";
        cout << "\n4. Steel: " << res->resourceTypes[3] << " (Can" << (res->hasGathered[3] ? "not" : "") << " gather)";
        cout << "\n\nProduction Rates:";
        for (int i = 0; i < res->resourceCount; i++) {
            cout << "\n Resource " << i + 1 << ": " << res->productionRates[i] 
                 << " (Efficiency: " << res->productionEfficiency[i] * 100 << "%)";
        }
        cout << "\nTreasury: " << eco->treasury << " gold";
        cout << "\n\nActions:";
        cout << "\n1. Gather Resources";
        cout << "\n2. Trade Resources";
        cout << "\n3. View Storage Capacity";
        cout << "\n4. Back to Main Menu";
        cout << "\nChoice: ";
        
        int choice;
        cin >> choice;
        
        if (choice == 4) break;
        
        switch (choice) {
            case 1: {
                cout << "\nSelect resource to gather (1-4): ";
                int resourceType;
                cin >> resourceType;
                
                if (resourceType >= 1 && resourceType <= 4) {
                    res->gatherResource(resourceType - 1);
                }
                break;
            }
            case 2: {
                res->trade();
                break;
            }
            case 3: {
                cout << "\nStorage Capacity:";
                for (int i = 0; i < res->resourceCount; i++) {
                    cout << "\nResource " << i + 1 << ": " << res->storage[i];
                }
                cout << "\nPress Enter to continue...";
                cin.ignore();
                cin.get();
                break;
            }
        }
    }
}

void manageEconomy(Economy* eco, Resources* res) {
    static int lastTaxCollection = -3;  // Initialize to -3 so tax can be collected on turn 0
    extern int turn;  // To access the global turn variable
    
    while (true) {
        displayBox("ECONOMY MANAGEMENT");
        cout << "\nCurrent Treasury: " << eco->treasury;
        cout << "\nCurrent Tax Rate: " << eco->taxRate << "%";
        cout << "\nTurns until next tax collection: " << max(0, 3 - (turn - lastTaxCollection));
        cout << "\n\nActions:";
        cout << "\n1. Adjust Tax Rate";
        cout << "\n2. Collect Taxes";
        cout << "\n3. Invest in Economy";
        cout << "\n4. Back to Main Menu";
        cout << "\nChoice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1: {
                cout << "\nEnter new tax rate (0-50): ";
                int newRate;
                cin >> newRate;
                if (newRate >= 0 && newRate <= 50) {
                    eco->taxRate = newRate;
                    displayEvent("SUCCESS", "Tax rate updated successfully!");
                }
                else {
                    displayEvent("ERROR", "Invalid tax rate! Must be between 0 and 50.");
                }
                break;
            }
            case 2: {
                if (turn - lastTaxCollection >= 3) {
                    eco->collectTaxes(eco->treasury);
                    lastTaxCollection = turn;
                    displayEvent("TAXES COLLECTED", "The kingdom's treasury has been updated with new tax revenue.");
                }
                else {
                    string message = "Must wait " + to_string(3 - (turn - lastTaxCollection)) + 
                                   " more turns before collecting taxes again.";
                    displayEvent("TAX COLLECTION COOLDOWN", message.c_str());
                }
                break;
            }
            case 3: {
                displayBox("INVESTMENT OPTIONS");
                cout << "\nResource Investment Options:";
                cout << "\n1. Food Production (100 gold)";
                cout << "\n2. Wood Production (150 gold)";
                cout << "\n3. Stone Production (200 gold)";
                cout << "\n4. Steel Production (300 gold)";
                cout << "\n\nInvestment Type:";
                cout << "\nA. Immediate Resource Gain";
                cout << "\nB. Improve Production Rate";
                cout << "\n\nSelect resource (1-4): ";
                
                int investChoice;
                cin >> investChoice;
                
                if (investChoice >= 1 && investChoice <= 4) {
                    cout << "\nSelect investment type (A/B): ";
                    char investType;
                    cin >> investType;
                    
                    int baseCost = 100 * investChoice;
                    
                    if (toupper(investType) == 'A') {
                        // Immediate resource gain
                        if (eco->treasury >= baseCost) {
                            eco->treasury -= baseCost;
                            int resourceGain = 50 * investChoice;  // Higher tier resources give more
                            
                            if (res->resourceTypes[investChoice - 1] + resourceGain <= res->storage[investChoice - 1]) {
                                res->resourceTypes[investChoice - 1] += resourceGain;
                                string message = "Investment yielded " + to_string(resourceGain) + " ";
                                switch(investChoice) {
                                    case 1: message += "food"; break;
                                    case 2: message += "wood"; break;
                                    case 3: message += "stone"; break;
                                    case 4: message += "steel"; break;
                                }
                                displayEvent("INVESTMENT SUCCESS", message.c_str());
                            } else {
                                displayEvent("STORAGE FULL", "Not enough storage capacity for resources!");
                                eco->treasury += baseCost;  // Refund if storage full
                            }
                        } else {
                            displayEvent("ERROR", "Insufficient funds for this investment!");
                        }
                    } 
                    else if (toupper(investType) == 'B') {
                        // Production rate improvement
                        int upgradeCost = baseCost * 2;  // More expensive for permanent improvement
                        
                        if (eco->treasury >= upgradeCost) {
                            eco->treasury -= upgradeCost;
                            
                            // Calculate production increase
                            float investmentEffect = (float)upgradeCost / 200.0f;
                            int rateIncrease = (int)(20 * investmentEffect);
                            
                            if (res->productionRates[investChoice - 1] + rateIncrease <= 500) {
                                res->productionRates[investChoice - 1] += rateIncrease;
                                
                                string message = "Production rate increased by " + to_string(rateIncrease) + " for ";
                                switch(investChoice) {
                                    case 1: message += "food"; break;
                                    case 2: message += "wood"; break;
                                    case 3: message += "stone"; break;
                                    case 4: message += "steel"; break;
                                }
                                displayEvent("PRODUCTION IMPROVED", message.c_str());
                            } else {
                                displayEvent("MAXIMUM REACHED", "Production rate cannot be improved further!");
                                eco->treasury += upgradeCost;  // Refund if at max
                            }
                        } else {
                            displayEvent("ERROR", "Insufficient funds for this investment!");
                        }
                    }
                }
                break;
            }
            case 4:
                return;
        }
    }
}

void manageMilitary(Military* mil, Resources* res, Population* pop, Economy* eco) {
    while (true) {
        displayBox("MILITARY MANAGEMENT");
        
        // Ensure valid values
        if (mil->training[0] < 0) mil->training[0] = 0;
        if (mil->training[0] > 100) mil->training[0] = 100;
        if (mil->soldiers[0] < 0) mil->soldiers[0] = 0;
        if (mil->morale < 0) mil->morale = 0;
        if (mil->morale > 100) mil->morale = 100;
        
        cout << "\n| Current Status:";
        cout << "\n| Active Soldiers: " << mil->soldiers[0];
        cout << "\n| Training Progress: " << mil->training[0] << "%";
        cout << "\n| Morale: " << mil->morale << "%";
        cout << "\n| Corruption Level: " << mil->corruptionLevel << "%";
        cout << "\n| Weapons: " << mil->weaponCount;
        cout << "\n| Monthly Cost: " << mil->cost << " gold";
        cout << "\n| Loyalty Status: " << (mil->isLoyal ? "Loyal" : "Disloyal");
        cout << "\n+" << string(60, '=') << "+";
        
        cout << "\n\nActions:";
        cout << "\n1. Train Military";
        cout << "\n2. Check Training Progress";
        cout << "\n3. Recruit Soldiers";
        cout << "\n4. Pay Troops";
        cout << "\n5. Distribute Rations";
        cout << "\n6. Conduct Anti-Corruption Campaign";
        cout << "\n7. Back to Main Menu";
        cout << "\nChoice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1: {
                // Existing training code...
                if (mil->training[0] > 0 && mil->training[0] < 100) {
                    displayEvent("TRAINING IN PROGRESS", "Military training is already underway. Check progress in option 2.");
                    break;
                }
                
                // Calculate training time based on resources
                int totalResources = 0;
                for (int i = 0; i < 4; i++) {
                    if (res->resourceTypes[i] >= 0) {
                        totalResources += res->resourceTypes[i];
                    }
                }
                
                int trainingTime = 10;
                if (totalResources > 0) {
                    trainingTime = max(1, 10 - (totalResources / 100));
                }
                
                mil->training[0] = 1;
                
                string message = "Training started! Will take " + to_string(trainingTime) + 
                               " turns to complete.\nMore resources = faster training.";
                displayEvent("TRAINING INITIATED", message.c_str());
                break;
            }
            case 2: {
                // Existing progress check code...
                if (mil->training[0] == 0) {
                    displayEvent("NO TRAINING", "No military training is currently in progress.");
                }
                else if (mil->training[0] >= 100) {
                    displayEvent("TRAINING COMPLETE", "Military training is complete! Troops are ready.");
                    mil->training[0] = 0;
                    mil->soldiers[0] += 50;
                    mil->morale += 10;
                    if (mil->morale > 100) mil->morale = 100;
                }
                else {
                    string progressMsg = "Training Progress: " + to_string(mil->training[0]) + "%\n";
                    progressMsg += "Training is ongoing. Progress increases each turn.";
                    displayEvent("TRAINING STATUS", progressMsg.c_str());
                }
                break;
            }
            case 3: {
                // New recruitment system
                cout << "\nAvailable Population for Recruitment: " << (pop->totalPopulation / 100) << " (1% of total)";
                cout << "\nEnter number of soldiers to recruit: ";
                int recruits;
                cin >> recruits;
                
                int maxRecruits = pop->totalPopulation / 100;  // Max 1% of population
                if (recruits <= 0) {
                    displayEvent("ERROR", "Invalid number of recruits!");
                }
                else if (recruits > maxRecruits) {
                    displayEvent("ERROR", "Not enough available population for recruitment!");
                }
                else {
                    int cost = recruits * 10;  // 10 gold per recruit
                    if (eco->treasury >= cost) {
                        eco->treasury -= cost;
                        mil->soldiers[0] += recruits;
                        pop->totalPopulation -= recruits;
                        string message = "Recruited " + to_string(recruits) + " soldiers\n";
                        message += "Cost: " + to_string(cost) + " gold";
                        displayEvent("RECRUITMENT SUCCESS", message.c_str());
                    }
                    else {
                        displayEvent("ERROR", "Insufficient funds for recruitment!");
                    }
                }
                break;
            }
            case 4: {
                // Pay troops
                int totalCost = mil->cost;
                cout << "\nRequired Payment: " << totalCost << " gold";
                cout << "\nPay troops? (1: Yes, 0: No): ";
                int confirm;
                cin >> confirm;
                
                if (confirm == 1) {
                    if (eco->treasury >= totalCost) {
                        eco->treasury -= totalCost;
                        mil->morale += 15;
                        if (mil->morale > 100) mil->morale = 100;
                        displayEvent("TROOPS PAID", "Morale has improved!");
                    }
                    else {
                        displayEvent("ERROR", "Insufficient funds to pay troops!");
                        mil->morale -= 20;
                        if (mil->morale < 0) mil->morale = 0;
                    }
                }
                break;
            }
            case 5: {
                // Distribute rations
                int requiredFood = mil->soldiers[0] * 2;  // 2 food per soldier
                cout << "\nRequired Food: " << requiredFood;
                cout << "\nDistribute rations? (1: Yes, 0: No): ";
                int confirm;
                cin >> confirm;
                
                if (confirm == 1) {
                    if (res->resourceTypes[0] >= requiredFood) {
                        res->resourceTypes[0] -= requiredFood;
                        mil->morale += 10;
                        if (mil->morale > 100) mil->morale = 100;
                        displayEvent("RATIONS DISTRIBUTED", "Troops are well-fed and morale has improved!");
                    }
                    else {
                        displayEvent("ERROR", "Insufficient food for rations!");
                        mil->morale -= 15;
                        if (mil->morale < 0) mil->morale = 0;
                    }
                }
                break;
            }
            case 6: {
                // Anti-corruption campaign
                int campaignCost = 500;
                cout << "\nCampaign Cost: " << campaignCost << " gold";
                cout << "\nLaunch anti-corruption campaign? (1: Yes, 0: No): ";
                int confirm;
                cin >> confirm;
                
                if (confirm == 1) {
                    if (eco->treasury >= campaignCost) {
                        eco->treasury -= campaignCost;
                        int reduction = min(mil->corruptionLevel, 30);  // Reduce up to 30%
                        mil->corruptionLevel -= reduction;
                        string message = "Corruption reduced by " + to_string(reduction) + "%\n";
                        message += "New corruption level: " + to_string(mil->corruptionLevel) + "%";
                        displayEvent("CAMPAIGN SUCCESS", message.c_str());
                    }
                    else {
                        displayEvent("ERROR", "Insufficient funds for campaign!");
                    }
                }
                break;
            }
            case 7:
                return;
        }
    }
}

void manageBank(Bank* bank, Economy* eco) {
    while (true) {
        displayBox("BANKING SYSTEM");
        bank->displayStatus();
        
        cout << "\nActions:";
        cout << "\n1. Take Loan";
        cout << "\n2. Repay Loan";
        cout << "\n3. Conduct Audit";
        cout << "\n4. Back to Main Menu";
        cout << "\nChoice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1: {
                cout << "\nEnter loan amount: ";
                double amount;
                cin >> amount;
                
                if (amount <= 0) {
                    displayEvent("ERROR", "Loan amount must be positive!");
                    break;
                }
                
                bank->giveLoan("Kingdom Treasury", amount);
                if (bank->getLoanAmount("Kingdom Treasury") > 0) {
                    eco->treasury += amount;
                }
                break;
            }
            case 2: {
                double currentLoan = bank->getLoanAmount("Kingdom Treasury");
                if (currentLoan <= 0) {
                    displayEvent("ERROR", "No active loans to repay!");
                    break;
                }
                
                cout << "\nCurrent loan amount: " << currentLoan;
                cout << "\nEnter repayment amount: ";
                double amount;
                cin >> amount;
                
                if (amount <= 0) {
                    displayEvent("ERROR", "Repayment amount must be positive!");
                    break;
                }
                
                if (amount > eco->treasury) {
                    displayEvent("ERROR", "Insufficient funds in treasury!");
                    break;
                }
                
                eco->treasury -= amount;
                bank->repayLoan("Kingdom Treasury", amount);
                break;
            }
            case 3: {
                bank->conductAudit();
                cout << "\nPress Enter to continue...";
                cin.ignore();
                cin.get();
                break;
            }
            case 4:
                return;
        }
    }
}

void manageKingdom(Population* pop, Economy* eco, Military* mil, Resources* res, Leadership* lead, 
                   Bank* bank, SocialClass* peasants, SocialClass* merchants, SocialClass* nobles) {
    while (true) {
        displayBox("KINGDOM MANAGEMENT");
        cout << "\n1. Display Kingdom Status";
        cout << "\n2. Resource Management";
        cout << "\n3. Military Management";
        cout << "\n4. Economy Management";
        cout << "\n5. Leadership Actions";
        cout << "\n6. Banking System";
        cout << "\n7. Back to Game Menu";
        cout << "\nEnter your choice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1:
                displayDetailedReports(pop, eco, mil, res, lead, bank, peasants, merchants, nobles);
                break;
            case 2:
                manageResources(res, eco);
                break;
            case 3:
                manageMilitary(mil, res, pop, eco);
                break;
            case 4:
                manageEconomy(eco, res);
                break;
            case 5:
                manageLeadership(lead, pop, eco, mil, peasants, merchants, nobles);
                break;
            case 6:
                manageBank(bank, eco);
                break;
            case 7:
                return;
        }
    }
}

void logScore(const char* filename, const char* ruler, int turn, int score, const char* reason = nullptr) 
{
    ofstream score_file(filename, ios::app);
    score_file << "Ruler: " << ruler << " | Turn " << turn << " | Score = " << score;
    if (reason != nullptr) {
        score_file << " | Game Over: " << reason;
    }
    score_file << "\n";
    score_file.close();
}

bool checkGameOver(Population* pop, Leadership* lead, string& gameOverReason) {
    if (pop->totalPopulation <= 0) {
        gameOverReason = "Kingdom has no population remaining";
        return true;
    }
    if (lead->isCoup) {
        gameOverReason = "Ruler has been overthrown by coup";
        return true;
    }
    return false;
}

void displayMainMenu() {
    cout << "\n=== STRONGHOLD: MEDIEVAL KINGDOM SIMULATOR ===\n";
    cout << "1. Start New Game\n";
    cout << "2. Load Game\n";
    cout << "3. Exit\n";
    cout << "Enter your choice: ";
}

void displayGameMenu() {
    cout << "\n=== KINGDOM MANAGEMENT ===\n";
    cout << "1. Continue Turn\n";
    cout << "2. View Detailed Reports\n";
    cout << "3. Manage Kingdom\n";
    cout << "4. Save Game\n";
    cout << "5. Exit to Main Menu\n";
    cout << "Enter your choice: ";
}

void displayDetailedReports(Population* pop, Economy* eco, Military* mil, Resources* res, Leadership* lead, Bank* bank, SocialClass* peasants, SocialClass* merchants, SocialClass* nobles) {
    displayBox("KINGDOM DETAILED REPORTS");

    // Social Classes Section
    displayBox("SOCIAL CLASSES");
    cout << "\nPeasants:";
    cout << "\n- Population: " << peasants->population;
    cout << "\n- Happiness: " << peasants->happiness << "%";
    cout << "\n- Wealth: " << peasants->wealth;
    cout << "\n- Influence: " << peasants->influence;
    
    cout << "\n\nMerchants:";
    cout << "\n- Population: " << merchants->population;
    cout << "\n- Happiness: " << merchants->happiness << "%";
    cout << "\n- Wealth: " << merchants->wealth;
    cout << "\n- Influence: " << merchants->influence;
    
    cout << "\n\nNobles:";
    cout << "\n- Population: " << nobles->population;
    cout << "\n- Happiness: " << nobles->happiness << "%";
    cout << "\n- Wealth: " << nobles->wealth;
    cout << "\n- Influence: " << nobles->influence;
    
    // Population Section
    displayBox("POPULATION");
    cout << "\n- Total Population: " << pop->totalPopulation;
    
    // Economy Section
    displayBox("ECONOMY");
    cout << "\n| Treasury: " << left << setw(49) << eco->treasury << " gold |\n";
    cout << "| Tax Rate: " << left << setw(50) << eco->taxRate << "% |\n";
    cout << "| Inflation: " << left << setw(49) << eco->inflation << "% |\n";
    cout << "+" << string(60, '=') << "+\n";
    
    // Military Section
    displayBox("MILITARY");
    cout << "\n- Active Soldiers: " << mil->soldiers[0];
    cout << "\n- Training Progress: " << mil->training[0] << "%";
    cout << "\n- Morale: " << mil->morale << "%";
    cout << "\n- Corruption Level: " << mil->corruptionLevel << "%";
    cout << "\n- Loyalty Status: " << (mil->isLoyal ? "Loyal" : "Disloyal");
    cout << "\n- Weapons Available: " << mil->weaponCount;
    
    // Resources Section
    displayBox("RESOURCES");
    cout << "\n- Food: " << res->resourceTypes[0];
    cout << "\n- Wood: " << res->resourceTypes[1];
    cout << "\n- Stone: " << res->resourceTypes[2];
    cout << "\n- Steel: " << res->resourceTypes[3];
    cout << "\n\nProduction Rates:";
    for (int i = 0; i < res->resourceCount; i++) {
        cout << "\n- Resource " << i + 1 << ": +" << res->productionRates[i] << " per turn";
    }
    cout << "\n\nConsumption Rates:";
    for (int i = 0; i < res->resourceCount; i++) {
        cout << "\n- Resource " << i + 1 << ": -" << res->consumptionRates[i] << " per turn";
    }
    
    // Leadership Section
    displayBox("LEADERSHIP");
    cout << "\n- Current Leader: " << lead->currentLeader;
    cout << "\n- Popularity: " << lead->popularity << "%";
    cout << "\n- Term Length: " << lead->term << " turns";
    cout << "\n- Supporter Count: " << lead->supporterCount;
    cout << "\n- Coup Risk: " << (lead->isCoup ? "High" : "Low");
    
    // Banking Section
    displayBox("BANKING");
    cout << "\n- Bank Treasury: " << bank->getTreasury() << " gold";
    cout << "\n- Interest Rate: " << (bank->getInterestRate() * 100) << "%";
    double kingdomLoan = bank->getLoanAmount("Kingdom Treasury");
    cout << "\n- Current Kingdom Loan: " << kingdomLoan << " gold";
    
    cout << "\n\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

void updateMilitaryTraining(Military* mil, Resources* res) {
    if (mil->training[0] > 0 && mil->training[0] < 100) {
        // Calculate progress increment based on resources
        int totalResources = 0;
        for (int i = 0; i < 4; i++) {
            if (res->resourceTypes[i] >= 0) {
                totalResources += res->resourceTypes[i];
            }
        }
        
        // More resources = faster progress
        int progressIncrement = max(5, min(20, totalResources / 50));
        mil->training[0] += progressIncrement;
        
        if (mil->training[0] >= 100) {
            mil->training[0] = 100;
            displayEvent("TRAINING COMPLETE", "Military training has been completed! Troops are ready.");
            mil->soldiers[0] += 50;  // Add trained soldiers
            mil->morale += 10;       // Boost morale
            if (mil->morale > 100) mil->morale = 100;
        }
    }
}

void manageLeadership(Leadership* lead, Population* pop, Economy* eco, Military* mil, SocialClass* peasants, SocialClass* merchants, SocialClass* nobles) {
    while (true) {
        displayBox("LEADERSHIP MANAGEMENT");
        
        cout << "\n| Current Leader: " << lead->currentLeader;
        cout << "\n| Popularity: " << lead->popularity << "%";
        cout << "\n| Term Length: " << lead->term << " turns";
        cout << "\n| Supporter Count: " << lead->supporterCount;
        cout << "\n| Coup Risk: " << (lead->isCoup ? "High" : "Low");
        cout << "\n+" << string(60, '=') << "+";
        
        cout << "\n\nActions:";
        cout << "\n1. Hold Elections";
        cout << "\n2. Implement Policy";
        cout << "\n3. Address Population";
        cout << "\n4. Handle Political Crisis";
        cout << "\n5. Back to Main Menu";
        cout << "\nChoice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1: {
                // Elections cost money and require minimum popularity
                int electionCost = 1000;
                if (eco->treasury < electionCost) {
                    displayEvent("ERROR", "Insufficient funds to hold elections!");
                    break;
                }
                
                if (lead->popularity < 30) {
                    displayEvent("WARNING", "Low popularity might lead to losing the election!");
                }
                
                cout << "\nHold elections? (1: Yes, 0: No): ";
                int confirm;
                cin >> confirm;
                
                if (confirm == 1) {
                    eco->treasury -= electionCost;
                    lead->holdElection();
                    
                    if (lead->popularity >= 50) {
                        lead->term++;
                        string message = "Election successful!\n";
                        message += "Term extended and popularity increased.";
                        displayEvent("ELECTION SUCCESS", message.c_str());
                    }
                    else {
                        lead->isCoup = true;
                        displayEvent("ELECTION LOST", "The population has voted for a new leader!");
                    }
                }
                break;
            }
            case 2: {
                cout << "\nChoose policy to implement:";
                cout << "\n1. Economic Focus (+Treasury, -Military)";
                cout << "\n2. Military Focus (+Morale, -Treasury)";
                cout << "\n3. Population Focus (+Happiness, -Resources)";
                cout << "\nChoice: ";
                
                int policyChoice;
                cin >> policyChoice;
                
                switch (policyChoice) {
                    case 1: {
                        eco->treasury += 500;
                        mil->morale -= 10;
                        lead->implementPolicy(1);  // Economic Focus
                        
                        // Economic policy affects classes differently
                        merchants->happiness += 15;  // Merchants benefit most
                        merchants->wealth += 10;
                        nobles->happiness += 10;    // Nobles benefit moderately
                        nobles->wealth += 5;
                        peasants->happiness -= 5;   // Peasants might suffer
                        
                        displayEvent("ECONOMIC POLICY", "Economic focus policy implemented.\nMerchants and nobles are pleased, but peasants are concerned.\nTreasury increased but military morale reduced.");
                        break;
                    }
                    case 2: {
                        eco->treasury -= 300;
                        mil->morale += 20;
                        lead->implementPolicy(2);  // Military Focus
                        
                        // Military policy affects security and stability
                        nobles->happiness += 10;    // Nobles appreciate strong military
                        merchants->happiness += 5;  // Merchants like stability
                        peasants->happiness -= 10;  // Peasants bear the burden
                        peasants->wealth -= 5;      // Military recruitment affects peasant workforce
                        
                        displayEvent("MILITARY POLICY", "Military focus policy implemented.\nMilitary morale improved but treasury reduced.\nNobles support the strong military stance.");
                        break;
                    }
                    case 3: {
                        // Population focus helps all classes but costs resources
                        peasants->happiness += 15;
                        peasants->birthRate = min(10, peasants->birthRate + 1);
                        merchants->happiness += 10;
                        merchants->birthRate = min(8, merchants->birthRate + 1);
                        nobles->happiness += 5;
                        
                        lead->implementPolicy(3);  // Population Focus
                        displayEvent("POPULATION POLICY", "Population focus policy implemented.\nAll classes show improved happiness.\nBirth rates have increased.");
                        break;
                    }
                }
                break;
            }
            case 3: {
                // Address population affects all classes
                int eventCost = 300;
                if (eco->treasury >= eventCost) {
                    eco->treasury -= eventCost;
                    lead->popularity += 15;
                    
                    // Each class reacts differently to being addressed
                    peasants->happiness += 15;   // Peasants appreciate direct attention most
                    merchants->happiness += 10;  // Merchants are moderately impressed
                    nobles->happiness += 5;      // Nobles are least impressed by public speeches
                    
                    displayEvent("ADDRESS SUCCESS", "Your speech has improved relations with all social classes!");
                }
                else {
                    displayEvent("ERROR", "Insufficient funds to organize the address!");
                }
                break;
            }
            case 4: {
                if (lead->isCoup) {
                    cout << "\nHandle crisis through:";
                    cout << "\n1. Military Force (Requires high military morale)";
                    cout << "\n2. Diplomatic Solution (Requires treasury)";
                    cout << "\n3. Public Appeal (Requires popularity)";
                    cout << "\nChoice: ";
                    
                    int crisisChoice;
                    cin >> crisisChoice;
                    
                    switch (crisisChoice) {
                        case 1:
                            if (mil->morale >= 70 && mil->isLoyal) {
                                lead->isCoup = false;
                                lead->popularity -= 20;
                                // Military action affects classes differently
                                nobles->happiness += 10;     // Nobles support strong action
                                merchants->happiness -= 10;  // Merchants dislike instability
                                peasants->happiness -= 20;   // Peasants suffer most from military action
                                displayEvent("CRISIS HANDLED", "Military force has maintained your position, but at a cost to civilian happiness.");
                            }
                            else {
                                displayEvent("CRISIS WORSENS", "Military refused to intervene due to low morale or loyalty!");
                            }
                            break;
                        case 2:
                            if (eco->treasury >= 2000) {
                                eco->treasury -= 2000;
                                lead->isCoup = false;
                                // Diplomatic solution has balanced effects
                                nobles->happiness -= 5;      // Nobles might see it as weak
                                merchants->happiness += 10;  // Merchants appreciate stability
                                peasants->happiness += 5;    // Peasants appreciate peaceful solution
                                displayEvent("CRISIS HANDLED", "Diplomatic solution has resolved the crisis.");
                            }
                            else {
                                displayEvent("ERROR", "Insufficient funds for diplomatic solution!");
                            }
                            break;
                        case 3:
                            if (lead->popularity >= 60) {
                                lead->isCoup = false;
                                lead->popularity += 10;
                                // Public appeal affects all classes positively
                                peasants->happiness += 15;
                                merchants->happiness += 10;
                                nobles->happiness += 5;
                                displayEvent("CRISIS HANDLED", "Public support has helped maintain stability!");
                            }
                            else {
                                displayEvent("CRISIS WORSENS", "Insufficient public support to resolve crisis!");
                            }
                            break;
                    }
                }
                else {
                    displayEvent("NO CRISIS", "There is currently no political crisis to handle.");
                }
                break;
            }
            case 5:
                return;
        }
    }
}

int main() 
{
    srand(time(0));
    bool programRunning = true;

    while (programRunning) {
        //system("pause");  // Keep this one before main menu
        system("cls");
        displayBox("MAIN MENU");
        cout << "1. Start New Game\n";
        cout << "2. Load Game\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        
        int mainChoice;
        cin >> mainChoice;

        if (mainChoice == 3) {
            break;  // Exit program
        }

        string rulerName;
        if (mainChoice == 1) {
            cout << "\nEnter your ruler's name: ";
            cin.ignore();
            getline(cin, rulerName);
        }

        // Initialize all systems
        SocialClass* peasants = new SocialClass("Peasants");
        SocialClass* nobles = new SocialClass("Nobles");
        SocialClass* merchants = new SocialClass("Merchants");
        Population* population = new Population();
        Military* military = new Military();
        Leadership* leadership = new Leadership(mainChoice == 1 ? rulerName.c_str() : "King Edward");
        Economy* economy = new Economy();
        Bank* bank = new Bank();
        Resources* resources = new Resources();
        EventSystem* events = new EventSystem();

        if (mainChoice == 1) {  // Only set initial values for new game
            // Set initial populations
            peasants->population = 1000;
            peasants->birthRate = 5;
            peasants->deathRate = 3;
            peasants->foodConsumption = peasants->population * 2;
            
            merchants->population = 200;
            merchants->birthRate = 4;
            merchants->deathRate = 3;
            merchants->foodConsumption = merchants->population * 2;
            
            nobles->population = 50;
            nobles->birthRate = 3;
            nobles->deathRate = 2;
            nobles->foodConsumption = nobles->population * 3;  // Nobles consume more per person
            
            // Update total population
            updateTotalPopulation(population, peasants, merchants, nobles);

            // Set other initial values
            economy->taxRate = 20;
            economy->inflation = 0;
            economy->treasury = 1000;

            resources->resourceTypes[0] = 500;  // Food
            resources->resourceTypes[1] = 300;  // Wood
            resources->resourceTypes[2] = 200;  // Stone
            resources->resourceTypes[3] = 100;  // Steel
        } else if (mainChoice == 2) {
            loadGameState("game_save.txt", population, economy, military, resources, peasants, merchants, nobles, leadership, bank, events);
            rulerName = leadership->currentLeader; // Get ruler name from loaded game
        }

        bool gameRunning = true;
        turn = 1;
        string gameOverReason;

        while (gameRunning) {
            system("cls");
            string turnTitle = "TURN " + to_string(turn);
            displayBox(turnTitle.c_str());
            
            // Check for game over conditions
            if (checkGameOver(population, leadership, gameOverReason)) {
                displayEvent("GAME OVER", gameOverReason.c_str());
                // Calculate final score
                int finalScore = population->totalPopulation +
                    economy->treasury +
                    military->morale * 10 +
                    leadership->popularity * 10;
                logScore("score.txt", rulerName.c_str(), turn, finalScore, gameOverReason.c_str());
                system("pause");
                break;
            }
            
            // Basic status
            cout << "\nQuick Status Report:";
            cout << "\nRuler: " << leadership->currentLeader;
            cout << "\nPopulation: " << population->totalPopulation;
            cout << "\nTreasury: " << economy->treasury;
            cout << "\nMilitary Morale: " << military->morale;
            cout << "\nFood Supply: " << resources->resourceTypes[0];
            cout << "\nLeader Popularity: " << leadership->popularity << "\n";

            displayGameMenu();
            int gameChoice;
            cin >> gameChoice;

            string processTitle;
            
            switch (gameChoice) 
            {
                case 1:
                {
                    system("cls");
                    processTitle = "PROCESSING TURN " + to_string(turn);
                    displayBox(processTitle.c_str());

                    // Update all systems
                    resources->gather();
                    resources->consume(peasants, merchants, nobles);
                    
                    // Update social classes first
                    peasants->updateStatus();
                    merchants->updateStatus();
                    nobles->updateStatus();
                    
                    // Update total population after social classes are updated
                    updateTotalPopulation(population, peasants, merchants, nobles);
                    
                    military->updateMilitary(economy->treasury / 10);
                    economy->collectTaxes(population->totalPopulation);
                    bank->updateLoans();
                    
                    // Check for coup risk and progression
                    bool wasCoup = leadership->isCoup;
                    leadership->checkStability();
                    
                    // Display coup warnings based on conditions
                    if (leadership->popularity < 30) {
                        displayEvent("COUP WARNING", "Your low popularity is causing unrest among your supporters!");
                    }
                    if (leadership->term > 8) {
                        displayEvent("SUCCESSION CRISIS", "Your long reign is causing some nobles to question your leadership!");
                    }
                    if (nobles->happiness < 30) {
                        displayEvent("NOBLE CONSPIRACY", "The nobility's dissatisfaction is leading to secret meetings!");
                    }
                    
                    // If coup occurs, game ends immediately
                    if (!wasCoup && leadership->isCoup) {
                        displayEvent("COUP D'ÉTAT", "You have been overthrown by a coalition of disgruntled nobles and military leaders!");
                        system("pause");
                        break;  // Exit turn processing
                    }

                    // Social class interactions
                    peasants->interact(nobles);
                    merchants->interact(nobles);
                    peasants->interact(merchants);

                    // Random events - increased chance based on conditions
                    if (peasants->happiness < 30 || merchants->happiness < 30 || nobles->happiness < 30) {
                        // Higher chance of negative events during unrest
                        if (rand() % 5 == 0) events->triggerRandomEvent();
                    } else {
                        // Normal random event chance
                        events->triggerRandomEvent();
                    }
                    
                    // Process active events and their effects
                    events->calculateImpact(resources, population, economy, military, 
                                         peasants, merchants, nobles);
                    events->resolveEvents();

                    // Update military training
                    updateMilitaryTraining(military, resources);

                    // Update kingdom state
                    updateKingdomState(population, economy, resources, peasants, merchants, nobles);

                    // Calculate and log score
                    int score = population->totalPopulation +
                        economy->treasury +
                        military->morale * 10 +
                        leadership->popularity * 10;
                    logScore("score.txt", leadership->currentLeader, turn, score);

                    turn++;
                    cout << "\nPress Enter to continue...";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cin.get();
                    break;
                }

                case 2:
                    displayDetailedReports(population, economy, military, resources, leadership, bank, peasants, merchants, nobles);
                    system("pause");  // Keep this one after reports
                    break;

                case 3:
                    manageKingdom(population, economy, military, resources, leadership, bank, peasants, merchants, nobles);
                    system("pause");  // Keep this one after management
                    break;

                case 4:
                    saveGameState("game_save.txt", population, economy, military, resources, peasants, merchants, nobles, leadership, bank, events);
                    system("pause");  // Keep this one after save
                    break;

                case 5:
                    gameRunning = false;
                    break;
            }
        }

        // Cleanup
        delete peasants;
        delete nobles;
        delete merchants;
        delete population;
        delete military;
        delete leadership;
        delete economy;
        delete bank;
        delete resources;
        delete events;
    }

    return 0;
}