#include "Stronghold.h"

Bank::Bank() {
    reserves = 5000;
    loanCount = 100;
    loans = new int[loanCount];
    interestRate = 5;
    isCorrupt = false;
    transactions = new int[1000];

    for (int i = 0; i < loanCount; i++) {
        loans[i] = 0;
    }
}

Bank::~Bank() {
    delete[] loans;
    delete[] transactions;
}

void Bank::processLoan(int amount) {
    if (amount <= reserves) {
        for (int i = 0; i < loanCount; i++) {
            if (loans[i] == 0) {
                loans[i] = amount;
                reserves -= amount;
                break;
            }
        }
    }
}

void Bank::auditAccounts() {
    int totalLoans = 0;
    for (int i = 0; i < loanCount; i++) {
        totalLoans += loans[i];
    }

    if (totalLoans + reserves < 4000) {
        isCorrupt = true;
    }
}

void Bank::updateInterestRates() {
    if (reserves < 1000) {
        interestRate += 2;
    }
    else if (reserves > 8000) {
        interestRate -= 1;
        if (interestRate < 2) interestRate = 2;
    }
}