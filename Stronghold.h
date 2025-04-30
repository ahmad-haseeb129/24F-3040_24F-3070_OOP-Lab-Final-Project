#pragma once
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;

// Base class for social classes
class SocialClass {
public:
    char* className;
    int happiness;
    int wealth;
    int influence;
    int population;

    SocialClass(const char* name);
    ~SocialClass();
    void interact(SocialClass* other);
    void updateStatus();
};

class Population {
public:
    int totalPopulation;
    int birthRate;
    int deathRate;
    int foodConsumption;
    bool isRevolting;
    int happiness;
    int* ageGroups;
    int diseaseRate;

    Population();
    ~Population();
    void updatePopulation(int food);
    void calculateGrowth();
    void checkRevoltConditions();
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
    void implementPolicy();
};

class Economy {
public:
    int treasury;
    int taxRate;
    int inflation;
    int* marketPrices;
    int marketSize;
    int* tradingPartners;

    Economy();
    ~Economy();
    void collectTaxes(int population);
    void adjustInflation();
    void handleTrade();
};

class Bank {
public:
    int reserves;
    int* loans;
    int loanCount;
    int interestRate;
    bool isCorrupt;
    int* transactions;

    Bank();
    ~Bank();
    void processLoan(int amount);
    void auditAccounts();
    void updateInterestRates();
};

class Resources {
public:
    int* resourceTypes;
    int resourceCount;
    int* productionRates;
    int* consumptionRates;
    int* storage;

    Resources();
    ~Resources();
    void gather();
    void consume();
    void trade();
};

class EventSystem
{
    public:
        bool* activeEvents;
        int* eventSeverity;
        int eventCount;
        int* eventDuration;

        EventSystem();
        ~EventSystem();
        void triggerRandomEvent();
        void resolveEvents();
        void calculateImpact();
};