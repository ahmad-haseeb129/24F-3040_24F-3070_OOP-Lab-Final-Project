#include "Stronghold.h"
#include <iostream>
#include <iomanip>
using namespace std;

// Forward declarations
void displayBox(const char* title);
void displayEvent(const char* event, const char* description);
void displayDetailedReports(Population* pop, Economy* eco, Military* mil, Resources* res, Leadership* lead, Bank* bank, SocialClass* peasants, SocialClass* merchants, SocialClass* nobles);
void saveGameState(const char* filename, Population* pop, Economy* eco, Military* mil, Resources* res, SocialClass* peasants, SocialClass* merchants, SocialClass* nobles);
void loadGameState(const char* filename, Population* pop, Economy* eco, Military* mil, Resources* res, SocialClass* peasants, SocialClass* merchants, SocialClass* nobles);
void updateSocialClassHappiness(SocialClass* socialClass, Economy* eco, Resources* res);
void manageResources(Resources* res);
void manageEconomy(Economy* eco, Resources* res);
void manageMilitary(Military* mil, Resources* res);
void manageKingdom(Population* pop, Economy* eco, Military* mil, Resources* res, Leadership* lead, Bank* bank, SocialClass* peasants, SocialClass* merchants, SocialClass* nobles);

void displayBox(const char* title) {
    int width = 60;
    int titleLength = strlen(title);
    int padding = (width - titleLength - 2) / 2;
    
    cout << "\n+" << string(width - 2, '=') << "+\n";
    cout << "|" << string(padding, ' ') << title << string(padding + (titleLength % 2 ? 0 : 1), ' ') << "|\n";
    cout << "+" << string(width - 2, '=') << "+\n";
}

void displayEvent(const char* event, const char* description) {
    cout << "\n+================================================+\n";
    cout << "| " << left << setw(46) << event << " |\n";
    cout << "+================================================+\n";
    
    // Split description into words and wrap text
    string desc(description);
    string word;
    string line;
    size_t pos = 0;
    
    while ((pos = desc.find(' ')) != string::npos) {
        word = desc.substr(0, pos);
        if (line.length() + word.length() > 44) {
            cout << "| " << left << setw(46) << line << " |\n";
            line = word + " ";
        }
        else {
            line += word + " ";
        }
        desc.erase(0, pos + 1);
    }
    line += desc;
    cout << "| " << left << setw(46) << line << " |\n";
    cout << "+================================================+\n";
}

void saveGameState(const char* filename, Population* pop, Economy* eco, Military* mil, Resources* res, 
                  SocialClass* peasants, SocialClass* merchants, SocialClass* nobles) {
    ofstream save(filename);
    save << pop->totalPopulation << " " << eco->treasury << " " << mil->morale << "\n";
    
    // Save social class data
    save << peasants->population << " " << peasants->happiness << " " << peasants->wealth << " " << peasants->influence << "\n";
    save << merchants->population << " " << merchants->happiness << " " << merchants->wealth << " " << merchants->influence << "\n";
    save << nobles->population << " " << nobles->happiness << " " << nobles->wealth << " " << nobles->influence << "\n";
    
    // Save resources
    for (int i = 0; i < 4; i++) {
        save << res->resourceTypes[i] << " ";
    }
    save << "\n";
    
    // Save military training progress
    save << mil->training[0] << "\n";
    
    save.close();
}

void loadGameState(const char* filename, Population* pop, Economy* eco, Military* mil, Resources* res,
                  SocialClass* peasants, SocialClass* merchants, SocialClass* nobles) {
    ifstream load(filename);
    if (load.is_open()) {
        load >> pop->totalPopulation >> eco->treasury >> mil->morale;
        
        // Load social class data
        load >> peasants->population >> peasants->happiness >> peasants->wealth >> peasants->influence;
        load >> merchants->population >> merchants->happiness >> merchants->wealth >> merchants->influence;
        load >> nobles->population >> nobles->happiness >> nobles->wealth >> nobles->influence;
        
        // Load resources
        for (int i = 0; i < 4; i++) {
            load >> res->resourceTypes[i];
        }
        
        // Load military training progress
        load >> mil->training[0];
        
        load.close();
        displayEvent("GAME LOADED", "Previous game state restored successfully!");
    }
    else {
        displayEvent("LOAD ERROR", "No saved game found. Starting new game...");
    }
}

void updateSocialClassHappiness(SocialClass* socialClass, Economy* eco, Resources* res) {
    // Base happiness affected by tax rate
    int happinessChange = 0;
    
    // Different classes react differently to conditions
    if (strcmp(socialClass->className, "Peasants") == 0) {
        // Peasants care most about food and tax rate
        happinessChange -= (eco->taxRate / 2);
        happinessChange += (res->resourceTypes[0] / 100); // Food impact
        
        if (res->resourceTypes[0] < 100) { // Food shortage
            displayEvent("PEASANT UNREST", "Food shortage is causing unrest among the peasants!");
            happinessChange -= 20;
        }
    }
    else if (strcmp(socialClass->className, "Merchants") == 0) {
        // Merchants care about economy and trade resources
        happinessChange = (eco->treasury > 1000) ? 5 : -5;
        happinessChange += (res->resourceTypes[1] + res->resourceTypes[2]) / 200; // Wood and stone impact
        
        if (eco->taxRate > 40) {
            displayEvent("MERCHANT DISSATISFACTION", "High tax rates are hurting trade and commerce!");
            happinessChange -= 15;
        }
    }
    else if (strcmp(socialClass->className, "Nobles") == 0) {
        // Nobles care about wealth and military
        happinessChange = (eco->treasury > 2000) ? 10 : -10;
        
        if (eco->taxRate < 20) {
            displayEvent("NOBLE UNREST", "Low tax rates are reducing the nobility's income!");
            happinessChange -= 10;
        }
    }
    
    // Update happiness with bounds checking
    socialClass->happiness = max(0, min(100, socialClass->happiness + happinessChange));
    
    // Population changes based on happiness
    if (socialClass->happiness < 30) {
        int leavingPop = socialClass->population * 0.1; // 10% leave
        if (leavingPop > 0) {
            socialClass->population -= leavingPop;
            string message = string(socialClass->className) + " are leaving the kingdom due to low satisfaction!";
            displayEvent("POPULATION DECLINE", message.c_str());
        }
    }
}

void manageResources(Resources* res) {
    while (true) {
        displayBox("RESOURCE MANAGEMENT");
        cout << "\nCurrent Resources:";
        cout << "\n1. Food: " << res->resourceTypes[0];
        cout << "\n2. Wood: " << res->resourceTypes[1];
        cout << "\n3. Stone: " << res->resourceTypes[2];
        cout << "\n4. Steel: " << res->resourceTypes[3];
        cout << "\n\nActions:";
        cout << "\n1. Gather Resources";
        cout << "\n2. Trade Resources";
        cout << "\n3. Back to Main Menu";
        cout << "\nChoice: ";
        
        int choice;
        cin >> choice;
        
        if (choice == 3) break;
        
        if (choice == 1 || choice == 2) {
            cout << "\nSelect resource type (1-4): ";
            int resourceType;
            cin >> resourceType;
            
            if (resourceType >= 1 && resourceType <= 4) {
                cout << "Enter amount: ";
                int amount;
                cin >> amount;
                
                if (amount < 0) {
                    displayEvent("ERROR", "Cannot process negative amounts!");
                    continue;
                }
                
                if (choice == 1) {
                    res->resourceTypes[resourceType - 1] += amount;
                    displayEvent("SUCCESS", "Resources gathered successfully!");
                }
                else {
                    if (res->resourceTypes[resourceType - 1] >= amount) {
                        res->resourceTypes[resourceType - 1] -= amount;
                        displayEvent("SUCCESS", "Trade completed successfully!");
                    }
                    else {
                        displayEvent("ERROR", "Insufficient resources for trade!");
                    }
                }
            }
        }
    }
}

void manageEconomy(Economy* eco, Resources* res) {
    while (true) {
        displayBox("ECONOMY MANAGEMENT");
        cout << "\nCurrent Treasury: " << eco->treasury;
        cout << "\nCurrent Tax Rate: " << eco->taxRate << "%";
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
                eco->collectTaxes(eco->treasury);
                displayEvent("TAXES COLLECTED", "The kingdom's treasury has been updated with new tax revenue.");
                break;
            }
            case 3: {
                displayBox("INVESTMENT OPTIONS");
                cout << "\n1. Food Production (100 gold)";
                cout << "\n2. Wood Production (150 gold)";
                cout << "\n3. Stone Production (200 gold)";
                cout << "\n4. Steel Production (300 gold)";
                cout << "\nChoice: ";
                
                int investChoice;
                cin >> investChoice;
                
                if (investChoice >= 1 && investChoice <= 4) {
                    int cost = 100 * investChoice;
                    if (eco->treasury >= cost) {
                        eco->treasury -= cost;
                        res->resourceTypes[investChoice - 1] += 50;
                        displayEvent("INVESTMENT SUCCESS", "Your investment has yielded new resources!");
                    }
                    else {
                        displayEvent("ERROR", "Insufficient funds for this investment!");
                    }
                }
                break;
            }
            case 4:
                return;
        }
    }
}

void manageMilitary(Military* mil, Resources* res) {
    while (true) {
        displayBox("MILITARY MANAGEMENT");
        
        // Ensure valid values
        if (mil->training[0] < 0) mil->training[0] = 0;
        if (mil->training[0] > 100) mil->training[0] = 100;
        if (mil->soldiers[0] < 0) mil->soldiers[0] = 0;
        if (mil->morale < 0) mil->morale = 0;
        if (mil->morale > 100) mil->morale = 100;
        
        cout << "\nCurrent Status:";
        cout << "\n- Soldiers: " << mil->soldiers[0];
        cout << "\n- Training Progress: " << mil->training[0] << "%";
        cout << "\n- Morale: " << mil->morale << "%";
        cout << "\n\nActions:";
        cout << "\n1. Train Military";
        cout << "\n2. Check Training Progress";
        cout << "\n3. Back to Main Menu";
        cout << "\nChoice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1: {
                // Check if training is already in progress
                if (mil->training[0] > 0 && mil->training[0] < 100) {
                    displayEvent("TRAINING IN PROGRESS", "Military training is already underway. Check progress in option 2.");
                    break;
                }
                
                // Calculate training time based on resources
                int totalResources = 0;
                for (int i = 0; i < 4; i++) {
                    if (res->resourceTypes[i] >= 0) {  // Ensure valid resource values
                        totalResources += res->resourceTypes[i];
                    }
                }
                
                int trainingTime = 10;  // Base training time
                if (totalResources > 0) {
                    trainingTime = max(1, 10 - (totalResources / 100));  // Min 1 turn, Max 10 turns
                }
                
                // Initialize training
                mil->training[0] = 0;  // Start progress at 0%
                
                string message = "Training started! Will take " + to_string(trainingTime) + 
                               " turns to complete.\nMore resources = faster training.";
                displayEvent("TRAINING INITIATED", message.c_str());
                break;
            }
            case 2: {
                if (mil->training[0] <= 0) {
                    displayEvent("NO TRAINING", "No military training is currently in progress.");
                }
                else if (mil->training[0] >= 100) {
                    displayEvent("TRAINING COMPLETE", "Military training is complete! Troops are ready.");
                    // Reset training and update soldiers/morale
                    mil->training[0] = 100;
                    mil->soldiers[0] += 50;  // Add trained soldiers
                    mil->morale += 10;       // Boost morale
                    if (mil->morale > 100) mil->morale = 100;
                }
                else {
                    string progressMsg = "Training Progress: " + to_string(mil->training[0]) + "%\n";
                    progressMsg += "Training is ongoing. Progress increases each turn.";
                    displayEvent("TRAINING STATUS", progressMsg.c_str());
                }
                break;
            }
            case 3:
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
        cout << "\n3. Population Management";
        cout << "\n4. Military Management";
        cout << "\n5. Economy Management";
        cout << "\n6. Leadership Actions";
        cout << "\n7. Banking System";
        cout << "\n8. Back to Game Menu";
        cout << "\nEnter your choice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1:
                displayDetailedReports(pop, eco, mil, res, lead, bank, peasants, merchants, nobles);
                break;
            case 2:
                manageResources(res);
                break;
            case 3:
                // Population management to be implemented
                break;
            case 4:
                manageMilitary(mil, res);
                break;
            case 5:
                manageEconomy(eco, res);
                break;
            case 6:
                // Leadership actions to be implemented
                break;
            case 7:
                // Banking system to be implemented
                break;
            case 8:
                return;
        }
    }
}

void logScore(const char* filename, int turn, int score) 
{
    ofstream score_file(filename, ios::app);
    score_file << "Turn " << turn << ": Score = " << score << "\n";
    score_file.close();
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
    cout << "\n- Birth Rate: " << pop->birthRate << " per 1000";
    cout << "\n- Death Rate: " << pop->deathRate << " per 1000";
    cout << "\n- Disease Rate: " << pop->diseaseRate << "%";
    
    // Economy Section
    displayBox("ECONOMY");
    cout << "\n- Treasury: " << eco->treasury << " gold";
    cout << "\n- Tax Rate: " << eco->taxRate << "%";
    cout << "\n- Inflation: " << eco->inflation << "%";
    cout << "\n- Market Prices: ";
    for (int i = 0; i < eco->marketSize; i++) {
        cout << "\n  * Item " << i + 1 << ": " << eco->marketPrices[i];
    }
    
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
    cout << "\n- Bank Reserves: " << bank->reserves << " gold";
    cout << "\n- Interest Rate: " << bank->interestRate << "%";
    cout << "\n- Active Loans: " << bank->loanCount;
    cout << "\n- Corruption Status: " << (bank->isCorrupt ? "Detected" : "None");
    
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

int main() 
{
    srand(time(0));
    bool programRunning = true;

    while (programRunning) {
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

        // Initialize all systems
        SocialClass* peasants = new SocialClass("Peasants");
        SocialClass* nobles = new SocialClass("Nobles");
        SocialClass* merchants = new SocialClass("Merchants");
        Population* population = new Population();
        Military* military = new Military();
        Leadership* leadership = new Leadership("King Edward");
        Economy* economy = new Economy();
        Bank* bank = new Bank();
        Resources* resources = new Resources();
        EventSystem* events = new EventSystem();

        if (mainChoice == 2) {
            loadGameState("game_save.txt", population, economy, military, resources, peasants, merchants, nobles);
        }

        bool gameRunning = true;
        int turn = 1;

        while (gameRunning) {
            string turnTitle = "TURN " + to_string(turn);
            displayBox(turnTitle.c_str());
            
            // Basic status
            cout << "\nQuick Status Report:";
            cout << "\nPopulation: " << population->totalPopulation;
            cout << "\nTreasury: " << economy->treasury;
            cout << "\nMilitary Morale: " << military->morale;
            cout << "\nFood Supply: " << resources->resourceTypes[0];
            cout << "\nLeader Popularity: " << leadership->popularity << "\n";

            displayGameMenu();
            int gameChoice;
            cin >> gameChoice;

            switch (gameChoice) 
            {
                case 1:
                {// Continue Turn
                    // Update all systems
                    resources->gather();
                    resources->consume();
                    population->updatePopulation(resources->resourceTypes[0]);
                    military->updateMilitary(economy->treasury / 10);
                    economy->collectTaxes(population->totalPopulation);
                    bank->updateInterestRates();
                    leadership->checkStability();

                    // Social class interactions
                    peasants->interact(nobles);
                    merchants->interact(nobles);
                    peasants->interact(merchants);

                    // Random events
                    if (rand() % 10 == 0) {
                        events->triggerRandomEvent();
                    }
                    events->resolveEvents();

                    // Update social class happiness
                    updateSocialClassHappiness(peasants, economy, resources);
                    updateSocialClassHappiness(merchants, economy, resources);
                    updateSocialClassHappiness(nobles, economy, resources);

                    // Calculate and log score
                    int score = population->totalPopulation +
                        economy->treasury +
                        military->morale * 10 +
                        leadership->popularity * 10;
                    logScore("score.txt", turn, score);

                    // Update military training
                    updateMilitaryTraining(military, resources);

                    turn++;
                    break;
                }

                case 2:  // View Detailed Reports
                    displayDetailedReports(population, economy, military, resources, leadership, bank, peasants, merchants, nobles);
                    break;

                case 3:  // Manage Kingdom
                    manageKingdom(population, economy, military, resources, leadership, bank, peasants, merchants, nobles);
                    break;

                case 4:  // Save Game
                    saveGameState("game_save.txt", population, economy, military, resources, peasants, merchants, nobles);
                    displayEvent("GAME SAVED", "Your progress has been saved successfully!");
                    break;

                case 5:  // Exit to Main Menu
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