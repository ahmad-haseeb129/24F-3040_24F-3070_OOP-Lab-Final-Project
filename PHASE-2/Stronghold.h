#pragma once
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;

// Forward declarations of utility functions
void displayEvent(const char* event, const char* description);
void displayBox(const char* title);

// Base class for social classes
class SocialClass {
public:
    char* className;
    int happiness;
    int wealth;
    int influence;
    int population;
    int birthRate;
    int deathRate;
    int foodConsumption;
    bool isRevolting;

    SocialClass(const char* name);
    ~SocialClass();
    void interact(SocialClass* other);
    void updateStatus();
};

class Population {
public:
    int totalPopulation;
    Population();
    ~Population();
};

class Military {
public:
    int* soldiers;
    int* training;
    int morale;
    int cost;
    int* weapons;
    int weaponCount;
    int corruptionLevel;
    bool isLoyal;

    Military();
    ~Military();
    void updateMilitary(int pay);
    void trainTroops();
    void assessMorale();
    void handleCorruption();
};

class Leadership {
public:
    char* currentLeader;
    int popularity;
    int term;
    bool isCoup;
    int* supporters;
    int supporterCount;

    Leadership(const char* leader);
    ~Leadership();
    void holdElection();
    void checkStability();
    void implementPolicy(int c);
};

class Economy {
public:
    int treasury;
    int taxRate;
    int inflation;

    Economy();
    ~Economy();
    void collectTaxes(int population);
    void adjustInflation();
    void handleTrade();
};

class Bank {
public:
    static const int MAX_LOANS = 100;
    double treasury;
    double interestRate;
    string borrowers[MAX_LOANS];
    double loanAmounts[MAX_LOANS];
    int loanDurations[MAX_LOANS];
    int activeLoanCount;
    int fraudAttempts;
    double stolenAmount;

    Bank(double initialFunds = 10000, double rate = 0.1);
    ~Bank();
    void giveLoan(const string& borrower, double amount);
    void collectInterest(const string& borrower);
    void repayLoan(const string& borrower, double amount);
    void simulateFraud();
    void conductAudit();
    void updateLoans();
    double getTreasury() const { return treasury; }
    double getInterestRate() const { return interestRate; }
    double getLoanAmount(const string& borrower) const;
    void displayStatus() const;
};

class Resources {
public:
    int* resourceTypes;
    int resourceCount;
    int* productionRates;
    int* consumptionRates;
    int* storage;
    float* productionEfficiency;
    bool* hasGathered;

    Resources();
    ~Resources();
    void gather();
    void gatherResource(int resourceType);
    void resetGatherStatus();
    void consume(SocialClass* peasants, SocialClass* merchants, SocialClass* nobles);
    void trade();
    void updateProductionEfficiency(SocialClass* peasants, SocialClass* merchants);
};

class EventSystem {
public:
    bool* activeEvents;
    int* eventSeverity;
    int eventCount;
    int* eventDuration;
    string* eventDescriptions;

    EventSystem();
    ~EventSystem();
    void triggerRandomEvent();
    void resolveEvents();
    void calculateImpact(Resources* res, Population* pop, Economy* eco, Military* mil,
        SocialClass* peasants, SocialClass* merchants, SocialClass* nobles);
};

// New classes for Module 2
class Communication {
public:
    static const int MAX_MESSAGES = 100;
    static const int MAX_NAME_LEN = 50;
    static const int MAX_CONTENT_LEN = 200;
    struct Message {
        char sender[MAX_NAME_LEN];
        char receiver[MAX_NAME_LEN];
        char content[MAX_CONTENT_LEN];
        int turn;
    };
    Message messages[MAX_MESSAGES];
    int messageCount;

    Communication();
    void sendMessage(const char* sender, const char* receiver, const char* content, int turn);
    void displayMessages(const char* kingdom);
};

class Alliance {
public:
    static const int MAX_TREATIES = 50;
    static const int MAX_NAME_LEN = 50;
    struct Treaty {
        char kingdom1[MAX_NAME_LEN];
        char kingdom2[MAX_NAME_LEN];
        char type[20]; // "Alliance", "Non-Aggression", "Trade"
        int duration; // Turns remaining
    };
    Treaty treaties[MAX_TREATIES];
    int treatyCount;
    int reputation; // Kingdom's diplomatic reputation (0-100)

    Alliance();
    void proposeTreaty(const char* kingdom1, const char* kingdom2, const char* type, int duration);
    void breakTreaty(const char* kingdom1, const char* kingdom2);
    bool isAllied(const char* kingdom1, const char* kingdom2);
    bool hasTreaty(const char* kingdom1, const char* kingdom2, const char* type);
    void updateTreaties();
};

class Market {
public:
    static const int MAX_OFFERS = 50;
    static const int MAX_NAME_LEN = 50;
    struct TradeOffer {
        char sender[MAX_NAME_LEN];
        char receiver[MAX_NAME_LEN];
        int resourceType; // 0: Food, 1: Wood, 2: Stone, 3: Steel
        int giveAmount;
        int receiveType;
        int receiveAmount;
        bool isSmuggling; // True for black-market trades
    };
    TradeOffer offers[MAX_OFFERS];
    int offerCount;
    char embargoes[MAX_NAME_LEN][MAX_NAME_LEN]; // Kingdoms under embargo
    int embargoCount;

    Market();
    void proposeTrade(const char* sender, const char* receiver, int resourceType, int giveAmount,
        int receiveType, int receiveAmount, bool isSmuggling);
    void acceptTrade(int offerIndex, Resources* senderRes, Resources* receiverRes, Economy* senderEco);
    bool isEmbargoed(const char* kingdom1, const char* kingdom2);
    void addEmbargo(const char* kingdom1, const char* kingdom2);
    void processSmuggling(Resources* senderRes, Economy* senderEco, int offerIndex);
};

class Conflict {
public:
    static const int MAX_CONFLICTS = 50;
    static const int MAX_NAME_LEN = 50;
    struct War {
        char attacker[MAX_NAME_LEN];
        char defender[MAX_NAME_LEN];
        int turnStarted;
    };
    War wars[MAX_CONFLICTS];
    int warCount;

    Conflict();
    void declareWar(const char* attacker, const char* defender, int turn, Military* attackerMil, Military* defenderMil,
        Resources* attackerRes, Resources* defenderRes, Alliance* alliance);
    void resolveBattle(const char* attacker, const char* defender, Military* attackerMil, Military* defenderMil,
        Resources* attackerRes, Resources* defenderRes, Alliance* alliance);
};

class Map {
public:
    static const int GRID_SIZE = 5;
    static const int MAX_NAME_LEN = 50;
    struct Location {
        char kingdom[MAX_NAME_LEN];
        bool isOccupied;
    };
    Location grid[GRID_SIZE][GRID_SIZE];
    int controlZones[MAX_NAME_LEN]; // Number of zones controlled per kingdom

    Map();
    void placeKingdom(const char* kingdom, int x, int y);
    void moveKingdom(const char* kingdom, int newX, int newY);
    int getDistance(const char* kingdom1, const char* kingdom2);
    void expandControl(const char* kingdom);
    void displayMap();
};