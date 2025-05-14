#define _CRT_SECURE_NO_WARNINGS
#include "Stronghold.h"
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

// Global constants
const int MAX_KINGDOMS = 4;
const int MAX_NAME_LEN = 50;

// Kingdom structure to hold all systems
struct Kingdom {
    char name[MAX_NAME_LEN];
    SocialClass* peasants;
    SocialClass* merchants;
    SocialClass* nobles;
    Population* population;
    Military* military;
    Leadership* leadership;
    Economy* economy;
    Bank* bank;
    Resources* resources;
    EventSystem* events;
};

// Forward declarations
void displayBox(const char* title);
void displayEvent(const char* event, const char* description);
void displayDetailedReports(Population* pop, Economy* eco, Military* mil, Resources* res, Leadership* lead, Bank* bank, SocialClass* peasants, SocialClass* merchants, SocialClass* nobles);

void updateSocialClassHappiness(SocialClass* socialClass, Economy* eco, Resources* res);
void manageResources(Resources* res, Economy* eco);
void manageEconomy(Economy* eco, Resources* res);
void manageMilitary(Military* mil, Resources* res, Population* pop, Economy* eco);
void manageKingdom(Kingdom* kingdoms, int kingdomCount, int currentKingdom, Communication* comm, Alliance* alliance, Market* market, Conflict* conflict, Map* map);
void manageLeadership(Leadership* lead, Population* pop, Economy* eco, Military* mil, SocialClass* peasants, SocialClass* merchants, SocialClass* nobles);
void manageBank(Bank* bank, Economy* eco);

void manageDiplomacy(Kingdom* kingdoms, int kingdomCount, int currentKingdom, Communication* comm, Alliance* alliance, Market* market, Conflict* conflict, Map* map);
void saveGameState(const char* filename, Kingdom* kingdoms, int kingdomCount, int turn, Communication* comm, Alliance* alliance, Market* market, Conflict* conflict, Map* map);
bool loadGameState(const char* filename, Kingdom* kingdoms, int& kingdomCount, int& turn, Communication* comm, Alliance* alliance, Market* market, Conflict* conflict, Map* map);
void updateTotalPopulation(Population* pop, SocialClass* peasants, SocialClass* merchants, SocialClass* nobles);
void updateMilitaryTraining(Military* mil, Resources* res);

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

void saveGameState(const char* filename, Kingdom* kingdoms, int kingdomCount, int turn, Communication* comm, Alliance* alliance, Market* market, Conflict* conflict, Map* map) {
    std::ofstream save(filename);
    if (!save.is_open()) {
        displayEvent("SAVE ERROR", "Unable to open file for saving.");
        return;
    }

    // Save turn and kingdom count
    save << turn << " " << kingdomCount << "\n";

    // Save each kingdom's data
    for (int k = 0; k < kingdomCount; k++) {
        // Save kingdom name
        save << kingdoms[k].name << "\n";

        // Save Population
        save << kingdoms[k].population->totalPopulation << "\n";

        // Save Economy
        save << kingdoms[k].economy->treasury << " " << kingdoms[k].economy->taxRate << " "
            << kingdoms[k].economy->inflation << "\n";

        // Save Military
        for (int i = 0; i < 5; i++) {
            save << kingdoms[k].military->soldiers[i] << " ";
        }
        for (int i = 0; i < 5; i++) {
            save << kingdoms[k].military->training[i] << " ";
        }
        save << kingdoms[k].military->morale << " " << kingdoms[k].military->cost << " "
            << kingdoms[k].military->weaponCount << " " << kingdoms[k].military->corruptionLevel << " "
            << kingdoms[k].military->isLoyal << " ";
        for (int i = 0; i < 10; i++) {
            save << kingdoms[k].military->weapons[i] << " ";
        }
        save << "\n";

        // Save Resources
        save << kingdoms[k].resources->resourceCount << " ";
        for (int i = 0; i < kingdoms[k].resources->resourceCount; i++) {
            save << kingdoms[k].resources->resourceTypes[i] << " ";
        }
        for (int i = 0; i < kingdoms[k].resources->resourceCount; i++) {
            save << kingdoms[k].resources->productionRates[i] << " ";
        }
        for (int i = 0; i < kingdoms[k].resources->resourceCount; i++) {
            save << kingdoms[k].resources->consumptionRates[i] << " ";
        }
        for (int i = 0; i < kingdoms[k].resources->resourceCount; i++) {
            save << kingdoms[k].resources->storage[i] << " ";
        }
        for (int i = 0; i < kingdoms[k].resources->resourceCount; i++) {
            save << kingdoms[k].resources->productionEfficiency[i] << " ";
        }
        for (int i = 0; i < kingdoms[k].resources->resourceCount; i++) {
            save << kingdoms[k].resources->hasGathered[i] << " ";
        }
        save << "\n";

        // Save Social Classes
        save << kingdoms[k].peasants->className << " "
            << kingdoms[k].peasants->population << " " << kingdoms[k].peasants->happiness << " "
            << kingdoms[k].peasants->wealth << " " << kingdoms[k].peasants->influence << " "
            << kingdoms[k].peasants->birthRate << " " << kingdoms[k].peasants->deathRate << " "
            << kingdoms[k].peasants->foodConsumption << " " << kingdoms[k].peasants->isRevolting << "\n";

        save << kingdoms[k].merchants->className << " "
            << kingdoms[k].merchants->population << " " << kingdoms[k].merchants->happiness << " "
            << kingdoms[k].merchants->wealth << " " << kingdoms[k].merchants->influence << " "
            << kingdoms[k].merchants->birthRate << " " << kingdoms[k].merchants->deathRate << " "
            << kingdoms[k].merchants->foodConsumption << " " << kingdoms[k].merchants->isRevolting << "\n";

        save << kingdoms[k].nobles->className << " "
            << kingdoms[k].nobles->population << " " << kingdoms[k].nobles->happiness << " "
            << kingdoms[k].nobles->wealth << " " << kingdoms[k].nobles->influence << " "
            << kingdoms[k].nobles->birthRate << " " << kingdoms[k].nobles->deathRate << " "
            << kingdoms[k].nobles->foodConsumption << " " << kingdoms[k].nobles->isRevolting << "\n";

        // Save Leadership
        save << kingdoms[k].leadership->currentLeader << " "
            << kingdoms[k].leadership->popularity << " " << kingdoms[k].leadership->term << " "
            << kingdoms[k].leadership->isCoup << " " << kingdoms[k].leadership->supporterCount << "\n";

        // Save Bank
        save << kingdoms[k].bank->treasury << " " << kingdoms[k].bank->interestRate << " "
            << kingdoms[k].bank->activeLoanCount << "\n";
        for (int i = 0; i < kingdoms[k].bank->activeLoanCount; i++) {
            save << kingdoms[k].bank->borrowers[i] << " " << kingdoms[k].bank->loanAmounts[i] << " "
                << kingdoms[k].bank->loanDurations[i] << "\n";
        }
        save << kingdoms[k].bank->fraudAttempts << " " << kingdoms[k].bank->stolenAmount << "\n";

        // Save Event System
        save << kingdoms[k].events->eventCount << "\n";
        for (int i = 0; i < kingdoms[k].events->eventCount; i++) {
            save << kingdoms[k].events->activeEvents[i] << " ";
        }
        save << "\n";
        for (int i = 0; i < kingdoms[k].events->eventCount; i++) {
            save << kingdoms[k].events->eventSeverity[i] << " ";
        }
        save << "\n";
        for (int i = 0; i < kingdoms[k].events->eventCount; i++) {
            save << kingdoms[k].events->eventDuration[i] << " ";
        }
        save << "\n";
        for (int i = 0; i < kingdoms[k].events->eventCount; i++) {
            save << kingdoms[k].events->eventDescriptions[i] << "\n";
        }
    }

    // Save Communication
    save << comm->messageCount << "\n";
    for (int i = 0; i < comm->messageCount; i++) {
        save << comm->messages[i].sender << " " << comm->messages[i].receiver << " "
            << comm->messages[i].content << " " << comm->messages[i].turn << "\n";
    }

    // Save Alliance
    save << alliance->treatyCount << " " << alliance->reputation << "\n";
    for (int i = 0; i < alliance->treatyCount; i++) {
        save << alliance->treaties[i].kingdom1 << " " << alliance->treaties[i].kingdom2 << " "
            << alliance->treaties[i].type << " " << alliance->treaties[i].duration << "\n";
    }

    // Save Market
    save << market->offerCount << " " << market->embargoCount << "\n";
    for (int i = 0; i < market->offerCount; i++) {
        save << market->offers[i].sender << " " << market->offers[i].receiver << " "
            << market->offers[i].resourceType << " " << market->offers[i].giveAmount << " "
            << market->offers[i].receiveType << " " << market->offers[i].receiveAmount << " "
            << (market->offers[i].isSmuggling ? 1 : 0) << "\n";
    }
    for (int i = 0; i < market->embargoCount; i++) {
        save << market->embargoes[i] << "\n";
    }

    // Save Conflict
    save << conflict->warCount << "\n";
    for (int i = 0; i < conflict->warCount; i++) {
        save << conflict->wars[i].attacker << " " << conflict->wars[i].defender << " "
            << conflict->wars[i].turnStarted << "\n";
    }

    // Save Map
    for (int i = 0; i < Map::GRID_SIZE; i++) {
        for (int j = 0; j < Map::GRID_SIZE; j++) {
            save << map->grid[i][j].kingdom << " " << (map->grid[i][j].isOccupied ? 1 : 0) << "\n";
        }
    }
    for (int i = 0; i < MAX_NAME_LEN; i++) {
        save << map->controlZones[i] << " ";
    }
    save << "\n";

    if (save.fail()) {
        displayEvent("SAVE ERROR", "Failed to write game state to file.");
        save.close();
        return;
    }

    save.close();
    displayEvent("GAME SAVED", "All game state data has been saved successfully!");
}

bool loadGameState(const char* filename, Kingdom* kingdoms, int& kingdomCount, int& turn, Communication* comm, Alliance* alliance, Market* market, Conflict* conflict, Map* map) {
    std::ifstream load(filename);
    if (!load.is_open()) {
        displayEvent("LOAD ERROR", "No saved game found or file could not be opened.");
        return false;
    }

    // Load turn and kingdom count
    if (!(load >> turn >> kingdomCount)) {
        displayEvent("LOAD ERROR", "Failed to read turn or kingdom count.");
        load.close();
        return false;
    }
    load.ignore(1000, '\n'); // Skip to the end of the line
    
    if (kingdomCount < 0 || kingdomCount > MAX_KINGDOMS) {
        displayEvent("LOAD ERROR", "Invalid kingdom count.");
        load.close();
        return false;
    }

    // Clean up previous kingdoms first to avoid memory leaks
    for (int i = 0; i < MAX_KINGDOMS; i++) {
        if (kingdoms[i].peasants) { delete kingdoms[i].peasants; kingdoms[i].peasants = nullptr; }
        if (kingdoms[i].merchants) { delete kingdoms[i].merchants; kingdoms[i].merchants = nullptr; }
        if (kingdoms[i].nobles) { delete kingdoms[i].nobles; kingdoms[i].nobles = nullptr; }
        if (kingdoms[i].population) { delete kingdoms[i].population; kingdoms[i].population = nullptr; }
        if (kingdoms[i].military) { delete kingdoms[i].military; kingdoms[i].military = nullptr; }
        if (kingdoms[i].leadership) { delete kingdoms[i].leadership; kingdoms[i].leadership = nullptr; }
        if (kingdoms[i].economy) { delete kingdoms[i].economy; kingdoms[i].economy = nullptr; }
        if (kingdoms[i].bank) { delete kingdoms[i].bank; kingdoms[i].bank = nullptr; }
        if (kingdoms[i].resources) { delete kingdoms[i].resources; kingdoms[i].resources = nullptr; }
        if (kingdoms[i].events) { delete kingdoms[i].events; kingdoms[i].events = nullptr; }
    }

    // Initialize all kingdoms and their components first before loading any data
    for (int k = 0; k < kingdomCount; k++) {
        // Set default kingdom name (will be overwritten when loading)
        kingdoms[k].name[0] = '\0';
        
        // Initialize all components with proper defaults
        kingdoms[k].peasants = new SocialClass("Peasants");
        kingdoms[k].merchants = new SocialClass("Merchants");
        kingdoms[k].nobles = new SocialClass("Nobles");
        kingdoms[k].population = new Population();
        kingdoms[k].military = new Military();
        kingdoms[k].leadership = new Leadership("Temp Leader");
        kingdoms[k].economy = new Economy();
        kingdoms[k].bank = new Bank();
        kingdoms[k].resources = new Resources();
        kingdoms[k].events = new EventSystem();
        
        // Ensure default population value
        kingdoms[k].population->totalPopulation = 0;
    }

    // Load each kingdom's data
    for (int k = 0; k < kingdomCount; k++) 
    {
        // Load kingdom name - names are single words in the save file
        load >> kingdoms[k].name;
        if (load.fail()) {
            displayEvent("LOAD ERROR", "Failed to read kingdom name.");
            load.close();
            return false;
        }
        load.ignore(1000, '\n'); // Skip to the end of the line
        
        // Load Population
        int totalPop;
        load >> totalPop;
        if (load.fail()) {
            displayEvent("LOAD ERROR", "Failed to read population data.");
            load.close();
            return false;
        }
        load.ignore(1000, '\n'); // Skip to the end of the line
        
        // Assign the loaded value
        kingdoms[k].population->totalPopulation = totalPop;

        // Load Economy
        load >> kingdoms[k].economy->treasury >> kingdoms[k].economy->taxRate >> kingdoms[k].economy->inflation;
        if (load.fail()) {
            displayEvent("LOAD ERROR", "Failed to read economy data.");
            load.close();
            return false;
        }
        load.ignore(1000, '\n'); // Skip to the end of the line

        // Load Military
        for (int i = 0; i < 5; i++) {
            load >> kingdoms[k].military->soldiers[i];
        }
        for (int i = 0; i < 5; i++) {
            load >> kingdoms[k].military->training[i];
        }
        load >> kingdoms[k].military->morale >> kingdoms[k].military->cost >> kingdoms[k].military->weaponCount
            >> kingdoms[k].military->corruptionLevel >> kingdoms[k].military->isLoyal;
        for (int i = 0; i < 10; i++) {
            load >> kingdoms[k].military->weapons[i];
        }
        if (load.fail()) {
            displayEvent("LOAD ERROR", "Failed to read military data.");
            load.close();
            return false;
        }
        load.ignore(1000, '\n'); // Skip to the end of the line

        // Load Resources
        load >> kingdoms[k].resources->resourceCount;
        if (kingdoms[k].resources->resourceCount > 4) {
            displayEvent("LOAD ERROR", "Invalid resource count.");
            load.close();
            return false;
        }
        for (int i = 0; i < kingdoms[k].resources->resourceCount; i++) {
            load >> kingdoms[k].resources->resourceTypes[i];
        }
        for (int i = 0; i < kingdoms[k].resources->resourceCount; i++) {
            load >> kingdoms[k].resources->productionRates[i];
        }
        for (int i = 0; i < kingdoms[k].resources->resourceCount; i++) {
            load >> kingdoms[k].resources->consumptionRates[i];
        }
        for (int i = 0; i < kingdoms[k].resources->resourceCount; i++) {
            load >> kingdoms[k].resources->storage[i];
        }
        for (int i = 0; i < kingdoms[k].resources->resourceCount; i++) {
            load >> kingdoms[k].resources->productionEfficiency[i];
        }
        for (int i = 0; i < kingdoms[k].resources->resourceCount; i++) {
            load >> kingdoms[k].resources->hasGathered[i];
        }
        if (load.fail()) {
            displayEvent("LOAD ERROR", "Failed to read resources data.");
            load.close();
            return false;
        }
        load.ignore(1000, '\n'); // Skip to the end of the line

        // Load Social Classes
        char className[50];
        load >> className;
        delete[] kingdoms[k].peasants->className;
        kingdoms[k].peasants->className = new char[strlen(className) + 1];
        strcpy(kingdoms[k].peasants->className, className);
        load >> kingdoms[k].peasants->population >> kingdoms[k].peasants->happiness
            >> kingdoms[k].peasants->wealth >> kingdoms[k].peasants->influence
            >> kingdoms[k].peasants->birthRate >> kingdoms[k].peasants->deathRate
            >> kingdoms[k].peasants->foodConsumption >> kingdoms[k].peasants->isRevolting;
        if (load.fail()) {
            displayEvent("LOAD ERROR", "Failed to read peasant class data.");
            load.close();
            return false;
        }
        load.ignore(1000, '\n'); // Skip to the end of the line

        load >> className;
        delete[] kingdoms[k].merchants->className;
        kingdoms[k].merchants->className = new char[strlen(className) + 1];
        strcpy(kingdoms[k].merchants->className, className);
        load >> kingdoms[k].merchants->population >> kingdoms[k].merchants->happiness
            >> kingdoms[k].merchants->wealth >> kingdoms[k].merchants->influence
            >> kingdoms[k].merchants->birthRate >> kingdoms[k].merchants->deathRate
            >> kingdoms[k].merchants->foodConsumption >> kingdoms[k].merchants->isRevolting;
        if (load.fail()) {
            displayEvent("LOAD ERROR", "Failed to read merchant class data.");
            load.close();
            return false;
        }
        load.ignore(1000, '\n'); // Skip to the end of the line

        load >> className;
        delete[] kingdoms[k].nobles->className;
        kingdoms[k].nobles->className = new char[strlen(className) + 1];
        strcpy(kingdoms[k].nobles->className, className);
        load >> kingdoms[k].nobles->population >> kingdoms[k].nobles->happiness
            >> kingdoms[k].nobles->wealth >> kingdoms[k].nobles->influence
            >> kingdoms[k].nobles->birthRate >> kingdoms[k].nobles->deathRate
            >> kingdoms[k].nobles->foodConsumption >> kingdoms[k].nobles->isRevolting;
        if (load.fail()) {
            displayEvent("LOAD ERROR", "Failed to read noble class data.");
            load.close();
            return false;
        }
        load.ignore(1000, '\n'); // Skip to the end of the line

        // Load Leadership
        char leaderName[50];
        load >> leaderName;
        // Delete the existing leadership->currentLeader memory
        delete[] kingdoms[k].leadership->currentLeader;
        // Allocate and copy new one
        kingdoms[k].leadership->currentLeader = new char[strlen(leaderName) + 1];
        strcpy(kingdoms[k].leadership->currentLeader, leaderName);
        load >> kingdoms[k].leadership->popularity >> kingdoms[k].leadership->term
            >> kingdoms[k].leadership->isCoup >> kingdoms[k].leadership->supporterCount;
        if (load.fail()) {
            displayEvent("LOAD ERROR", "Failed to read leadership data.");
            load.close();
            return false;
        }
        load.ignore(1000, '\n'); // Skip to the end of the line

        // Load Bank
        load >> kingdoms[k].bank->treasury >> kingdoms[k].bank->interestRate >> kingdoms[k].bank->activeLoanCount;
        if (kingdoms[k].bank->activeLoanCount > Bank::MAX_LOANS) {
            displayEvent("LOAD ERROR", "Invalid loan count.");
            load.close();
            return false;
        }
        load.ignore(1000, '\n'); // Skip to the end of the line
        
        for (int i = 0; i < Bank::MAX_LOANS; i++) {
            kingdoms[k].bank->borrowers[i].clear();
            kingdoms[k].bank->loanAmounts[i] = 0;
            kingdoms[k].bank->loanDurations[i] = 0;
        }
        for (int i = 0; i < kingdoms[k].bank->activeLoanCount; i++) {
            std::string borrower;
            double amount;
            int duration;
            load >> borrower >> amount >> duration;
            if (load.fail()) {
                displayEvent("LOAD ERROR", "Failed to read bank loan data.");
                load.close();
                return false;
            }
            kingdoms[k].bank->borrowers[i] = borrower;
            kingdoms[k].bank->loanAmounts[i] = amount;
            kingdoms[k].bank->loanDurations[i] = duration;
            load.ignore(1000, '\n'); // Skip to the end of the line
        }
        load >> kingdoms[k].bank->fraudAttempts >> kingdoms[k].bank->stolenAmount;
        if (load.fail()) {
            displayEvent("LOAD ERROR", "Failed to read bank data.");
            load.close();
            return false;
        }
        load.ignore(1000, '\n'); // Skip to the end of the line

        // Load Event System
        load >> kingdoms[k].events->eventCount;
        if (load.fail()) {
            displayEvent("LOAD ERROR", "Failed to read event count.");
            load.close();
            return false;
        }
        load.ignore(1000, '\n'); // Skip to the end of the line
        
        delete[] kingdoms[k].events->activeEvents;
        delete[] kingdoms[k].events->eventSeverity;
        delete[] kingdoms[k].events->eventDuration;
        delete[] kingdoms[k].events->eventDescriptions;

        // Validate event count
        if (kingdoms[k].events->eventCount < 0 || kingdoms[k].events->eventCount > 100) {
            displayEvent("LOAD ERROR", "Invalid event count.");
            load.close();
            return false;
        }

        if (kingdoms[k].events->eventCount > 0) {
            kingdoms[k].events->activeEvents = new bool[kingdoms[k].events->eventCount];
            kingdoms[k].events->eventSeverity = new int[kingdoms[k].events->eventCount];
            kingdoms[k].events->eventDuration = new int[kingdoms[k].events->eventCount];
            kingdoms[k].events->eventDescriptions = new std::string[kingdoms[k].events->eventCount];
            
            for (int i = 0; i < kingdoms[k].events->eventCount; i++) {
                load >> kingdoms[k].events->activeEvents[i];
            }
            if (load.fail()) {
                displayEvent("LOAD ERROR", "Failed to read event active status.");
                load.close();
                return false;
            }
            load.ignore(1000, '\n'); // Skip to the end of the line
            
            for (int i = 0; i < kingdoms[k].events->eventCount; i++) {
                load >> kingdoms[k].events->eventSeverity[i];
            }
            if (load.fail()) {
                displayEvent("LOAD ERROR", "Failed to read event severity.");
                load.close();
                return false;
            }
            load.ignore(1000, '\n'); // Skip to the end of the line
            
            for (int i = 0; i < kingdoms[k].events->eventCount; i++) {
                load >> kingdoms[k].events->eventDuration[i];
            }
            if (load.fail()) {
                displayEvent("LOAD ERROR", "Failed to read event duration.");
                load.close();
                return false;
            }
            load.ignore(1000, '\n'); // Skip to the end of the line
            
            for (int i = 0; i < kingdoms[k].events->eventCount; i++) {
                std::string desc;
                std::getline(load, desc);
                if (load.fail()) {
                    displayEvent("LOAD ERROR", "Failed to read event descriptions.");
                    load.close();
                    return false;
                }
                kingdoms[k].events->eventDescriptions[i] = desc;
            }
        } else {
            // Zero events - initialize with empty arrays
            kingdoms[k].events->activeEvents = new bool[1];
            kingdoms[k].events->eventSeverity = new int[1];
            kingdoms[k].events->eventDuration = new int[1];
            kingdoms[k].events->eventDescriptions = new std::string[1];
        }
    }

    // Load Communication
    load >> comm->messageCount;
    if (load.fail()) {
        displayEvent("LOAD ERROR", "Failed to read message count.");
        load.close();
        return false;
    }
    load.ignore(1000, '\n'); // Skip to the end of the line
    
    if (comm->messageCount > comm->MAX_MESSAGES || comm->messageCount < 0) {
        displayEvent("LOAD ERROR", "Invalid message count.");
        load.close();
        return false;
    }
    
    for (int i = 0; i < comm->messageCount; i++) {
        load >> comm->messages[i].sender >> comm->messages[i].receiver
            >> comm->messages[i].content >> comm->messages[i].turn;
        if (load.fail()) {
            displayEvent("LOAD ERROR", "Failed to read communication data.");
            load.close();
            return false;
        }
        load.ignore(1000, '\n'); // Skip to the end of the line
    }

    // Load Alliance
    load >> alliance->treatyCount >> alliance->reputation;
    if (load.fail()) {
        displayEvent("LOAD ERROR", "Failed to read alliance count.");
        load.close();
        return false;
    }
    load.ignore(1000, '\n'); // Skip to the end of the line
    
    if (alliance->treatyCount > alliance->MAX_TREATIES || alliance->treatyCount < 0) {
        displayEvent("LOAD ERROR", "Invalid treaty count.");
        load.close();
        return false;
    }
    
    // Initialize all treaty entries before loading
    for (int i = 0; i < alliance->MAX_TREATIES; i++) {
        alliance->treaties[i].kingdom1[0] = '\0';
        alliance->treaties[i].kingdom2[0] = '\0';
        alliance->treaties[i].type[0] = '\0';
        alliance->treaties[i].duration = 0;
    }
    
    for (int i = 0; i < alliance->treatyCount; i++) {
        // Use safer string reading approach with clear buffer sizes
        char k1[Alliance::MAX_NAME_LEN];
        char k2[Alliance::MAX_NAME_LEN];
        char type[20]; // Treaty type max length from the struct
        
        load >> k1 >> k2 >> type >> alliance->treaties[i].duration;
        if (load.fail()) {
            displayEvent("LOAD ERROR", "Failed to read alliance data.");
            load.close();
            return false;
        }
        load.ignore(1000, '\n'); // Skip to the end of the line
        
        // Copy to the actual structures with proper null termination
        strncpy(alliance->treaties[i].kingdom1, k1, Alliance::MAX_NAME_LEN - 1);
        alliance->treaties[i].kingdom1[Alliance::MAX_NAME_LEN - 1] = '\0';
        
        strncpy(alliance->treaties[i].kingdom2, k2, Alliance::MAX_NAME_LEN - 1);
        alliance->treaties[i].kingdom2[Alliance::MAX_NAME_LEN - 1] = '\0';
        
        strncpy(alliance->treaties[i].type, type, 19);
        alliance->treaties[i].type[19] = '\0';
    }

    // Load Market
    load >> market->offerCount >> market->embargoCount;
    if (load.fail()) {
        displayEvent("LOAD ERROR", "Failed to read market data counts.");
        load.close();
        return false;
    }
    load.ignore(1000, '\n'); // Skip to the end of the line
    
    if (market->offerCount < 0 || market->offerCount > market->MAX_OFFERS ||
        market->embargoCount < 0 || market->embargoCount > market->MAX_NAME_LEN) {
        displayEvent("LOAD ERROR", "Invalid market data counts.");
        load.close();
        return false;
    }
    
    for (int i = 0; i < market->offerCount; i++) {
        int smuggling;
        load >> market->offers[i].sender >> market->offers[i].receiver
            >> market->offers[i].resourceType >> market->offers[i].giveAmount
            >> market->offers[i].receiveType >> market->offers[i].receiveAmount
            >> smuggling;
        market->offers[i].isSmuggling = smuggling;
        if (load.fail()) {
            displayEvent("LOAD ERROR", "Failed to read market offer data.");
            load.close();
            return false;
        }
        load.ignore(1000, '\n'); // Skip to the end of the line
    }
    
    for (int i = 0; i < market->embargoCount; i++) {
        load >> market->embargoes[i];
        if (load.fail()) {
            displayEvent("LOAD ERROR", "Failed to read market embargo data.");
            load.close();
            return false;
        }
        load.ignore(1000, '\n'); // Skip to the end of the line
    }

    // Load Conflict
    load >> conflict->warCount;
    if (load.fail()) {
        displayEvent("LOAD ERROR", "Failed to read conflict war count.");
        load.close();
        return false;
    }
    load.ignore(1000, '\n'); // Skip to the end of the line
    
    if (conflict->warCount < 0 || conflict->warCount > conflict->MAX_CONFLICTS) {
        displayEvent("LOAD ERROR", "Invalid war count.");
        load.close();
        return false;
    }
    
    for (int i = 0; i < conflict->warCount; i++) {
        load >> conflict->wars[i].attacker >> conflict->wars[i].defender
            >> conflict->wars[i].turnStarted;
        if (load.fail()) {
            displayEvent("LOAD ERROR", "Failed to read conflict data.");
            load.close();
            return false;
        }
        load.ignore(1000, '\n'); // Skip to the end of the line
    }

    // Load Map
    for (int i = 0; i < Map::GRID_SIZE; i++) {
        for (int j = 0; j < Map::GRID_SIZE; j++) {
            int occupied;
            load >> map->grid[i][j].kingdom >> occupied;
            map->grid[i][j].isOccupied = occupied;
            if (load.fail()) {
                displayEvent("LOAD ERROR", "Failed to read map grid data.");
                load.close();
                return false;
            }
            load.ignore(1000, '\n'); // Skip to the end of the line
        }
    }
    
    for (int i = 0; i < MAX_NAME_LEN; i++) {
        load >> map->controlZones[i];
        if (load.fail()) {
            displayEvent("LOAD ERROR", "Failed to read map control zones.");
            load.close();
            return false;
        }
    }
    load.ignore(1000, '\n'); // Skip to the end of the line

    load.close();
    
    // Update total population (ensure it's correct)
    for (int k = 0; k < kingdomCount; k++) {
        updateTotalPopulation(kingdoms[k].population, kingdoms[k].peasants, kingdoms[k].merchants, kingdoms[k].nobles);
    }
    
    displayEvent("GAME LOADED", "All game state data has been restored successfully!");
    return true;
}

void manageDiplomacy(Kingdom* kingdoms, int kingdomCount, int currentKingdom, Communication* comm, Alliance* alliance, Market* market, Conflict* conflict, Map* map) {
    while (true) 
    {
        displayBox("DIPLOMACY MANAGEMENT");
        cout << "\nCurrent Kingdom: " << kingdoms[currentKingdom].name;
        cout << "\n1. Send Message";
        cout << "\n2. View Messages";
        cout << "\n3. Propose Treaty";
        cout << "\n4. Break Treaty";
        cout << "\n5. Propose Trade";
        cout << "\n6. Accept Trade";
        cout << "\n7. Declare War";
        cout << "\n8. View Map";
        cout << "\n9. Move Kingdom";
        cout << "\n10. Expand Control";
        cout << "\n11. Add Embargo";
        cout << "\n12. Back to Main Menu";
        cout << "\nChoice: ";
        int choice;
        cin >> choice;
        switch (choice) 
        {
            case 1: 
            {
                int recipient;
                while (true)
                {
                    try
                    {
                        cout << "\nSelect recipient kingdom (1-" << kingdomCount << "): ";
                        cin >> recipient;
                        if (cin.fail())
                        {
                            cin.clear();
                            cin.ignore(1000, '\n');
                            throw "Invalid input. Please enter a number.";
                        }
                        break;
                    }
                    catch (const char* msg)
                    {
                        cout << msg;
                    }
                }
            
                if (recipient < 1 || recipient > kingdomCount || recipient - 1 == currentKingdom) {
                    displayEvent("ERROR", "Invalid kingdom!");
                    break;
                }
                cout << "\nEnter message: ";
                char content[200];
                cin.ignore();
                cin.getline(content, 200);
                comm->sendMessage(kingdoms[currentKingdom].name, kingdoms[recipient - 1].name, content, turn);
                break;
            }
            case 2: 
            {
                comm->displayMessages(kingdoms[currentKingdom].name);
                cout << "\nPress Enter to continue...";
                cin.ignore();
                cin.get();
                break;
            }
            case 3: 
            {
                cout << "\nSelect kingdom (1-" << kingdomCount << "): ";
                int other;
                cin >> other;
                if (other < 1 || other > kingdomCount || other - 1 == currentKingdom) {
                    displayEvent("ERROR", "Invalid kingdom!");
                    break;
                }
                cout << "\nTreaty type (1: Alliance, 2: Non-Aggression, 3: Trade): ";
                int type;
                cin >> type;
                cout << "\nDuration (turns): ";
                int duration;
                cin >> duration;
                char treatyType[20];
                if (type == 1) strcpy(treatyType, "Alliance");
                else if (type == 2) strcpy(treatyType, "Non-Aggression");
                else if (type == 3) strcpy(treatyType, "Trade");
                else {
                    displayEvent("ERROR", "Invalid treaty type!");
                    break;
                }
                alliance->proposeTreaty(kingdoms[currentKingdom].name, kingdoms[other - 1].name, treatyType, duration);
                break;
            }
            case 4: 
            {
                cout << "\nSelect kingdom (1-" << kingdomCount << "): ";
                int other;
                cin >> other;
                if (other < 1 || other > kingdomCount || other - 1 == currentKingdom) {
                    displayEvent("ERROR", "Invalid kingdom!");
                    break;
                }
                alliance->breakTreaty(kingdoms[currentKingdom].name, kingdoms[other - 1].name);
                break;
            }
            case 5: 
            {
                cout << "\nSelect kingdom (1-" << kingdomCount << "): ";
                int other;
                cin >> other;
                if (other < 1 || other > kingdomCount || other - 1 == currentKingdom) {
                    displayEvent("ERROR", "Invalid kingdom!");
                    break;
                }
                cout << "\nResource to give (1: Food, 2: Wood, 3: Stone, 4: Steel): ";
                int giveType;
                cin >> giveType;
                cout << "\nAmount to give: ";
                int giveAmount;
                cin >> giveAmount;
                cout << "\nResource to receive (1: Food, 2: Wood, 3: Stone, 4: Steel): ";
                int receiveType;
                cin >> receiveType;
                cout << "\nAmount to receive: ";
                int receiveAmount;
                cin >> receiveAmount;
                cout << "\nSmuggling? (1: Yes, 0: No): ";
                int smuggling;
                cin >> smuggling;
                market->proposeTrade(kingdoms[currentKingdom].name, kingdoms[other - 1].name,
                    giveType - 1, giveAmount, receiveType - 1, receiveAmount, smuggling == 1);
                break;
            }
        case 6: {
            cout << "\nSelect trade offer (0-" << market->offerCount - 1 << "): ";
            int offerIndex;
            cin >> offerIndex;
            market->acceptTrade(offerIndex, kingdoms[currentKingdom].resources,
                kingdoms[currentKingdom].resources, kingdoms[currentKingdom].economy);
            break;
        }
        case 7: {
            cout << "\nSelect kingdom to attack (1-" << kingdomCount << "): ";
            int defender;
            cin >> defender;
            if (defender < 1 || defender > kingdomCount || defender - 1 == currentKingdom) {
                displayEvent("ERROR", "Invalid kingdom!");
                break;
            }
            conflict->declareWar(kingdoms[currentKingdom].name, kingdoms[defender - 1].name, turn,
                kingdoms[currentKingdom].military, kingdoms[defender - 1].military,
                kingdoms[currentKingdom].resources, kingdoms[defender - 1].resources, alliance);
            break;
        }
        case 8: {
            map->displayMap();
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
            break;
        }
        case 9: {
            cout << "\nEnter new X coordinate (0-4): ";
            int x;
            cin >> x;
            cout << "\nEnter new Y coordinate (0-4): ";
            int y;
            cin >> y;
            map->moveKingdom(kingdoms[currentKingdom].name, x, y);
            break;
        }
        case 10: {
            map->expandControl(kingdoms[currentKingdom].name);
            break;
        }
        case 11: {
            cout << "\nSelect kingdom to embargo (1-" << kingdomCount << "): ";
            int other;
            cin >> other;
            if (other < 1 || other > kingdomCount || other - 1 == currentKingdom) {
                displayEvent("ERROR", "Invalid kingdom!");
                break;
            }
            market->addEmbargo(kingdoms[currentKingdom].name, kingdoms[other - 1].name);
            break;
        }
        case 12:
            return;
        }
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

void manageKingdom(Kingdom* kingdoms, int kingdomCount, int currentKingdom, Communication* comm, Alliance* alliance, Market* market, Conflict* conflict, Map* map) {
    while (true) {
        displayBox("KINGDOM MANAGEMENT");
        cout << "\nCurrent Kingdom: " << kingdoms[currentKingdom].name;
        cout << "\n1. Display Kingdom Status";
        cout << "\n2. Resource Management";
        cout << "\n3. Military Management";
        cout << "\n4. Economy Management";
        cout << "\n5. Leadership Actions";
        cout << "\n6. Banking System";
        cout << "\n7. Diplomacy Management";
        cout << "\n8. Back to Game Menu";
        cout << "\nEnter your choice: ";
        int choice;
        cin >> choice;
        switch (choice) {
        case 1:
            displayDetailedReports(kingdoms[currentKingdom].population, kingdoms[currentKingdom].economy,
                kingdoms[currentKingdom].military, kingdoms[currentKingdom].resources,
                kingdoms[currentKingdom].leadership, kingdoms[currentKingdom].bank,
                kingdoms[currentKingdom].peasants, kingdoms[currentKingdom].merchants,
                kingdoms[currentKingdom].nobles);
            break;
        case 2:
            manageResources(kingdoms[currentKingdom].resources, kingdoms[currentKingdom].economy);
            break;
        case 3:
            manageMilitary(kingdoms[currentKingdom].military, kingdoms[currentKingdom].resources,
                kingdoms[currentKingdom].population, kingdoms[currentKingdom].economy);
            break;
        case 4:
            manageEconomy(kingdoms[currentKingdom].economy, kingdoms[currentKingdom].resources);
            break;
        case 5:
            manageLeadership(kingdoms[currentKingdom].leadership, kingdoms[currentKingdom].population,
                kingdoms[currentKingdom].economy, kingdoms[currentKingdom].military,
                kingdoms[currentKingdom].peasants, kingdoms[currentKingdom].merchants,
                kingdoms[currentKingdom].nobles);
            break;
        case 6:
            manageBank(kingdoms[currentKingdom].bank, kingdoms[currentKingdom].economy);
            break;
        case 7:
            manageDiplomacy(kingdoms, kingdomCount, currentKingdom, comm, alliance, market, conflict, map);
            break;
        case 8:
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

int main() {
    srand(time(0));
    bool programRunning = true;
    Kingdom kingdoms[MAX_KINGDOMS];
    int kingdomCount = 0;
    int turn = 1;
    Communication* comm = nullptr;
    Alliance* alliance = nullptr;
    Market* market = nullptr;
    Conflict* conflict = nullptr;
    Map* map = nullptr;

    // Initialize pointers to null
    for (int i = 0; i < MAX_KINGDOMS; i++) {
        kingdoms[i].peasants = nullptr;
        kingdoms[i].merchants = nullptr;
        kingdoms[i].nobles = nullptr;
        kingdoms[i].population = nullptr;
        kingdoms[i].military = nullptr;
        kingdoms[i].leadership = nullptr;
        kingdoms[i].economy = nullptr;
        kingdoms[i].bank = nullptr;
        kingdoms[i].resources = nullptr;
        kingdoms[i].events = nullptr;
    }

    while (programRunning) {
        // Clean up old game components if they exist
        if (comm) { delete comm; comm = nullptr; }
        if (alliance) { delete alliance; alliance = nullptr; }
        if (market) { delete market; market = nullptr; }
        if (conflict) { delete conflict; conflict = nullptr; }
        if (map) { delete map; map = nullptr; }

        // Create new shared game components
        comm = new Communication();
        alliance = new Alliance();
        market = new Market();
        conflict = new Conflict();
        map = new Map();

        system("cls");
        displayBox("MAIN MENU");
        cout << "1. Start New Game\n";
        cout << "2. Load Game\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        int mainChoice;
        cin >> mainChoice;
        if (mainChoice == 3) break;

        bool gameLoaded = false;

        if (mainChoice == 1) {
            try
            {
                cout << "\nEnter number of kingdoms (2-4): ";
                cin >> kingdomCount;
                if (cin.fail())
                {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    throw "Must be a number!";
                }
            }
            catch (const char* msg)
            {
                cout << "\nERROR: " << msg << endl;
                system("pause");
                continue;
            }
            
            if (kingdomCount < 2 || kingdomCount > MAX_KINGDOMS) {
                displayEvent("ERROR", "Invalid number of kingdoms!");
                continue;
            }
            
            // Clean up any existing kingdom data
            for (int i = 0; i < MAX_KINGDOMS; i++) {
                if (kingdoms[i].peasants) { delete kingdoms[i].peasants; kingdoms[i].peasants = nullptr; }
                if (kingdoms[i].merchants) { delete kingdoms[i].merchants; kingdoms[i].merchants = nullptr; }
                if (kingdoms[i].nobles) { delete kingdoms[i].nobles; kingdoms[i].nobles = nullptr; }
                if (kingdoms[i].population) { delete kingdoms[i].population; kingdoms[i].population = nullptr; }
                if (kingdoms[i].military) { delete kingdoms[i].military; kingdoms[i].military = nullptr; }
                if (kingdoms[i].leadership) { delete kingdoms[i].leadership; kingdoms[i].leadership = nullptr; }
                if (kingdoms[i].economy) { delete kingdoms[i].economy; kingdoms[i].economy = nullptr; }
                if (kingdoms[i].bank) { delete kingdoms[i].bank; kingdoms[i].bank = nullptr; }
                if (kingdoms[i].resources) { delete kingdoms[i].resources; kingdoms[i].resources = nullptr; }
                if (kingdoms[i].events) { delete kingdoms[i].events; kingdoms[i].events = nullptr; }
            }
            
            cin.ignore();
            for (int i = 0; i < kingdomCount; i++) {
                cout << "\nEnter name for kingdom " << i + 1 << ": ";
                char name[50];
                cin.getline(name, 50);
                strncpy(kingdoms[i].name, name, MAX_NAME_LEN - 1);
                kingdoms[i].name[MAX_NAME_LEN - 1] = '\0';
                kingdoms[i].peasants = new SocialClass("Peasants");
                kingdoms[i].merchants = new SocialClass("Merchants");
                kingdoms[i].nobles = new SocialClass("Nobles");
                kingdoms[i].population = new Population();
                kingdoms[i].military = new Military();
                kingdoms[i].leadership = new Leadership(name);
                kingdoms[i].economy = new Economy();
                kingdoms[i].bank = new Bank();
                kingdoms[i].resources = new Resources();
                kingdoms[i].events = new EventSystem();
                kingdoms[i].peasants->population = 1000;
                kingdoms[i].peasants->birthRate = 5;
                kingdoms[i].peasants->deathRate = 3;
                kingdoms[i].peasants->foodConsumption = kingdoms[i].peasants->population * 2;
                kingdoms[i].merchants->population = 200;
                kingdoms[i].merchants->birthRate = 4;
                kingdoms[i].merchants->deathRate = 3;
                kingdoms[i].merchants->foodConsumption = kingdoms[i].merchants->population * 2;
                kingdoms[i].nobles->population = 50;
                kingdoms[i].nobles->birthRate = 3;
                kingdoms[i].nobles->deathRate = 2;
                kingdoms[i].nobles->foodConsumption = kingdoms[i].nobles->population * 3;
                updateTotalPopulation(kingdoms[i].population, kingdoms[i].peasants, kingdoms[i].merchants, kingdoms[i].nobles);
                kingdoms[i].economy->taxRate = 20;
                kingdoms[i].economy->inflation = 0;
                kingdoms[i].economy->treasury = 1000;
                kingdoms[i].resources->resourceTypes[0] = 500;
                kingdoms[i].resources->resourceTypes[1] = 300;
                kingdoms[i].resources->resourceTypes[2] = 200;
                kingdoms[i].resources->resourceTypes[3] = 100;
                int x = i % Map::GRID_SIZE;
                int y = i / Map::GRID_SIZE;
                map->placeKingdom(kingdoms[i].name, x, y);
            }
            gameLoaded = true;
            turn = 1; // Reset turn counter for new game
        }
        else if (mainChoice == 2) 
        {
            gameLoaded = loadGameState("game_save.txt", kingdoms, kingdomCount, turn, comm, alliance, market, conflict, map);
            if (!gameLoaded) {
                cout << "\nFailed to load game. Returning to main menu.\n";
                system("pause");
                continue;
            }
        }

        bool gameRunning = gameLoaded; // Only start the game if successfully created or loaded
        while (gameRunning) 
        {
            for (int currentKingdom = 0; currentKingdom < kingdomCount && gameRunning; currentKingdom++) 
            {
                system("cls");
                string turnTitle = string(kingdoms[currentKingdom].name) + " - TURN " + to_string(turn);
                displayBox(turnTitle.c_str());
                string gameOverReason;
                if (checkGameOver(kingdoms[currentKingdom].population, kingdoms[currentKingdom].leadership, gameOverReason)) {
                    char message[200];
                    strcpy(message, kingdoms[currentKingdom].name);
                    strcat(message, ": ");
                    strcat(message, gameOverReason.c_str());
                    displayEvent("GAME OVER", message);
                    int finalScore = kingdoms[currentKingdom].population->totalPopulation +
                        kingdoms[currentKingdom].economy->treasury +
                        kingdoms[currentKingdom].military->morale * 10 +
                        kingdoms[currentKingdom].leadership->popularity * 10;
                    logScore("score.txt", kingdoms[currentKingdom].name, turn, finalScore, gameOverReason.c_str());
                    gameRunning = false;
                    break;
                }
                cout << "\nQuick Status Report:";
                cout << "\nKingdom: " << kingdoms[currentKingdom].name;
                cout << "\nPopulation: " << kingdoms[currentKingdom].population->totalPopulation;
                cout << "\nTreasury: " << kingdoms[currentKingdom].economy->treasury;
                cout << "\nMilitary Morale: " << kingdoms[currentKingdom].military->morale;
                cout << "\nFood Supply: " << kingdoms[currentKingdom].resources->resourceTypes[0];
                cout << "\nLeader Popularity: " << kingdoms[currentKingdom].leadership->popularity << "\n";
                displayGameMenu();
                int gameChoice;
                cin >> gameChoice;
                switch (gameChoice) {
                case 1: 
                {
                    system("cls");
                    string processTitle = "PROCESSING " + string(kingdoms[currentKingdom].name) + " TURN " + to_string(turn);
                    displayBox(processTitle.c_str());
                    kingdoms[currentKingdom].resources->gather();
                    kingdoms[currentKingdom].resources->consume(kingdoms[currentKingdom].peasants,
                        kingdoms[currentKingdom].merchants,
                        kingdoms[currentKingdom].nobles);
                    kingdoms[currentKingdom].peasants->updateStatus();
                    kingdoms[currentKingdom].merchants->updateStatus();
                    kingdoms[currentKingdom].nobles->updateStatus();
                    updateTotalPopulation(kingdoms[currentKingdom].population,
                        kingdoms[currentKingdom].peasants,
                        kingdoms[currentKingdom].merchants,
                        kingdoms[currentKingdom].nobles);
                    kingdoms[currentKingdom].military->updateMilitary(kingdoms[currentKingdom].economy->treasury / 10);
                    kingdoms[currentKingdom].economy->collectTaxes(kingdoms[currentKingdom].population->totalPopulation);
                    kingdoms[currentKingdom].bank->updateLoans();
                    bool wasCoup = kingdoms[currentKingdom].leadership->isCoup;
                    kingdoms[currentKingdom].leadership->checkStability();
                    if (kingdoms[currentKingdom].leadership->popularity < 30) {
                        displayEvent("COUP WARNING", "Your low popularity is causing unrest among your supporters!");
                    }
                    if (kingdoms[currentKingdom].leadership->term > 8) {
                        displayEvent("SUCCESSION CRISIS", "Your long reign is causing some nobles to question your leadership!");
                    }
                    if (kingdoms[currentKingdom].nobles->happiness < 30) {
                        displayEvent("NOBLE CONSPIRACY", "The nobility's dissatisfaction is leading to secret meetings!");
                    }
                    if (!wasCoup && kingdoms[currentKingdom].leadership->isCoup) {
                        char message[200];
                        strcpy(message, kingdoms[currentKingdom].name);
                        strcat(message, ": You have been overthrown by a coalition of disgruntled nobles and military leaders!");
                        displayEvent("COUP D'ETAT", message);
                        break;
                    }
                    kingdoms[currentKingdom].peasants->interact(kingdoms[currentKingdom].nobles);
                    kingdoms[currentKingdom].merchants->interact(kingdoms[currentKingdom].nobles);
                    kingdoms[currentKingdom].peasants->interact(kingdoms[currentKingdom].merchants);
                    if (kingdoms[currentKingdom].peasants->happiness < 30 || kingdoms[currentKingdom].merchants->happiness < 30 || kingdoms[currentKingdom].nobles->happiness < 30) {
                        if (rand() % 5 == 0) kingdoms[currentKingdom].events->triggerRandomEvent();
                    }
                    else {
                        kingdoms[currentKingdom].events->triggerRandomEvent();
                    }
                    kingdoms[currentKingdom].events->calculateImpact(kingdoms[currentKingdom].resources,
                        kingdoms[currentKingdom].population,
                        kingdoms[currentKingdom].economy,
                        kingdoms[currentKingdom].military,
                        kingdoms[currentKingdom].peasants,
                        kingdoms[currentKingdom].merchants,
                        kingdoms[currentKingdom].nobles);
                    kingdoms[currentKingdom].events->resolveEvents();
                    updateMilitaryTraining(kingdoms[currentKingdom].military, kingdoms[currentKingdom].resources);
                    updateKingdomState(kingdoms[currentKingdom].population, kingdoms[currentKingdom].economy,
                        kingdoms[currentKingdom].resources, kingdoms[currentKingdom].peasants,
                        kingdoms[currentKingdom].merchants, kingdoms[currentKingdom].nobles);
                    alliance->updateTreaties();
                    int score = kingdoms[currentKingdom].population->totalPopulation +
                        kingdoms[currentKingdom].economy->treasury +
                        kingdoms[currentKingdom].military->morale * 10 +
                        kingdoms[currentKingdom].leadership->popularity * 10;
                    logScore("score.txt", kingdoms[currentKingdom].name, turn, score);
                    cout << "\nPress Enter to continue...";
                    cin.ignore();
                    cin.get();
                    break;
                }
                case 2:
                    displayDetailedReports(kingdoms[currentKingdom].population, kingdoms[currentKingdom].economy,
                        kingdoms[currentKingdom].military, kingdoms[currentKingdom].resources,
                        kingdoms[currentKingdom].leadership, kingdoms[currentKingdom].bank,
                        kingdoms[currentKingdom].peasants, kingdoms[currentKingdom].merchants,
                        kingdoms[currentKingdom].nobles);
                    system("pause");
                    break;
                case 3:
                    manageKingdom(kingdoms, kingdomCount, currentKingdom, comm, alliance, market, conflict, map);
                    system("pause");
                    break;
                case 4:
                    saveGameState("game_save.txt", kingdoms, kingdomCount, turn, comm, alliance, market, conflict, map);
                    system("pause");
                    break;
                case 5:
                    gameRunning = false;
                    break;
                }
            }
            turn++;
        }

        // Cleanup kingdom memory
        for (int i = 0; i < kingdomCount; i++) {
            if (kingdoms[i].peasants) { delete kingdoms[i].peasants; kingdoms[i].peasants = nullptr; }
            if (kingdoms[i].merchants) { delete kingdoms[i].merchants; kingdoms[i].merchants = nullptr; }
            if (kingdoms[i].nobles) { delete kingdoms[i].nobles; kingdoms[i].nobles = nullptr; }
            if (kingdoms[i].population) { delete kingdoms[i].population; kingdoms[i].population = nullptr; }
            if (kingdoms[i].military) { delete kingdoms[i].military; kingdoms[i].military = nullptr; }
            if (kingdoms[i].leadership) { delete kingdoms[i].leadership; kingdoms[i].leadership = nullptr; }
            if (kingdoms[i].economy) { delete kingdoms[i].economy; kingdoms[i].economy = nullptr; }
            if (kingdoms[i].bank) { delete kingdoms[i].bank; kingdoms[i].bank = nullptr; }
            if (kingdoms[i].resources) { delete kingdoms[i].resources; kingdoms[i].resources = nullptr; }
            if (kingdoms[i].events) { delete kingdoms[i].events; kingdoms[i].events = nullptr; }
        }
    }

    // Final cleanup before exiting
    if (comm) delete comm;
    if (alliance) delete alliance;
    if (market) delete market;
    if (conflict) delete conflict;
    if (map) delete map;

    return 0;
}