#pragma once
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <map>
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
    int totalPopulation;  // Just maintains the total count

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
    static const int MAX_LOANS = 100;  // Maximum number of loans
    double treasury;
    double interestRate;
    
    // Arrays for loan management
    string borrowers[MAX_LOANS];
    double loanAmounts[MAX_LOANS];
    int loanDurations[MAX_LOANS];
    int activeLoanCount;
    
    // Fraud detection
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
    
    // Getters
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

class EventSystem
{
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
                           class SocialClass* peasants, class SocialClass* merchants, SocialClass* nobles);
};