#include "Stronghold.h"
#include <iostream>
#include <iomanip>
#include <random>

Bank::Bank(double initialFunds, double rate) 
    : treasury(initialFunds), interestRate(rate) {
    activeLoanCount = 0;
    fraudAttempts = 0;
    stolenAmount = 0;
    
    // Initialize arrays
    for (int i = 0; i < MAX_LOANS; i++) {
        borrowers[i] = "";
        loanAmounts[i] = 0;
        loanDurations[i] = 0;
    }
}

Bank::~Bank() {
}

void Bank::giveLoan(const string& borrower, double amount) {
    if (amount <= 0) {
        displayEvent("LOAN ERROR", "Loan amount must be positive!");
        return;
    }
    
    if (amount > treasury) {
        displayEvent("LOAN ERROR", "Insufficient funds in treasury!");
        return;
    }

    if (activeLoanCount >= MAX_LOANS) {
        displayEvent("LOAN ERROR", "Maximum number of loans reached!");
        return;
    }

    // Find existing loan or empty slot
    int loanIndex = -1;
    for (int i = 0; i < MAX_LOANS; i++) {
        if (borrowers[i] == borrower) {
            loanIndex = i;
            break;
        } else if (borrowers[i] == "" && loanIndex == -1) {
            loanIndex = i;
        }
    }

    if (borrowers[loanIndex] == "") {
        activeLoanCount++;
    }

    borrowers[loanIndex] = borrower;
    loanAmounts[loanIndex] += amount;
    loanDurations[loanIndex] = 10; // 10 turns to repay
    treasury -= amount;

    string message = "Loan given to " + borrower + "\n";
    message += "Amount: " + to_string((int)amount) + " gold\n";
    message += "Interest Rate: " + to_string((int)(interestRate * 100)) + "%\n";
    message += "Duration: 10 turns";
    displayEvent("LOAN APPROVED", message.c_str());
}

void Bank::collectInterest(const string& borrower) {
    for (int i = 0; i < MAX_LOANS; i++) {
        if (borrowers[i] == borrower && loanAmounts[i] > 0) {
            double interest = loanAmounts[i] * interestRate;
            loanAmounts[i] += interest;

            string message = "Interest collected from " + borrower + "\n";
            message += "Interest Amount: " + to_string((int)interest) + " gold\n";
            message += "New Total Owed: " + to_string((int)loanAmounts[i]) + " gold";
            displayEvent("INTEREST COLLECTED", message.c_str());
            return;
        }
    }
}

void Bank::repayLoan(const string& borrower, double amount) {
    for (int i = 0; i < MAX_LOANS; i++) {
        if (borrowers[i] == borrower && loanAmounts[i] > 0) {
            if (amount > loanAmounts[i]) {
                amount = loanAmounts[i];
            }

            loanAmounts[i] -= amount;
            treasury += amount;

            string message = "Loan repayment from " + borrower + "\n";
            message += "Amount Repaid: " + to_string((int)amount) + " gold\n";
            message += "Remaining Debt: " + to_string((int)loanAmounts[i]) + " gold";
            displayEvent("LOAN REPAYMENT", message.c_str());

            if (loanAmounts[i] <= 0) {
                borrowers[i] = "";
                loanDurations[i] = 0;
                activeLoanCount--;
            }
            return;
        }
    }
    displayEvent("REPAYMENT ERROR", "No active loan found!");
}

void Bank::simulateFraud() {
    // 10% chance of fraud occurring
    if (rand() % 100 < 10) {
        double fraudAmount = treasury * 0.1; // Steal 10% of treasury
        treasury -= fraudAmount;
        stolenAmount += fraudAmount;
        fraudAttempts++;

        string message = "Suspicious activity detected!\n";
        message += "Amount Missing: " + to_string((int)fraudAmount) + " gold";
        displayEvent("FRAUD ALERT", message.c_str());
    }
}

void Bank::conductAudit() {
    string message = "Bank Audit Results\n";
    message += "Treasury: " + to_string((int)treasury) + " gold\n";
    message += "Active Loans: " + to_string(activeLoanCount) + "\n";
    
    if (fraudAttempts > 0) {
        message += "\nSuspicious Activity Detected!\n";
        message += "Fraud Attempts: " + to_string(fraudAttempts) + "\n";
        message += "Total Amount Missing: " + to_string((int)stolenAmount) + " gold";
        
        // Reset fraud tracking after audit
        fraudAttempts = 0;
        stolenAmount = 0;
    } else {
        message += "\nNo suspicious activity detected.";
    }
    
    displayEvent("AUDIT REPORT", message.c_str());
}

void Bank::updateLoans() {
    for (int i = 0; i < MAX_LOANS; i++) {
        if (borrowers[i] != "" && loanAmounts[i] > 0) {
            // Decrease loan duration
            loanDurations[i]--;
            
            // Collect interest
            collectInterest(borrowers[i]);
            
            // Check for defaults
            if (loanDurations[i] <= 0) {
                string message = "Loan Default by " + borrowers[i] + "\n";
                message += "Unpaid Amount: " + to_string((int)loanAmounts[i]) + " gold";
                displayEvent("LOAN DEFAULT", message.c_str());
                
                // Record the default as fraud
                fraudAttempts++;
                stolenAmount += loanAmounts[i];
                
                // Remove the defaulted loan
                borrowers[i] = "";
                loanAmounts[i] = 0;
                activeLoanCount--;
            }
        }
    }
}

double Bank::getLoanAmount(const string& borrower) const {
    for (int i = 0; i < MAX_LOANS; i++) {
        if (borrowers[i] == borrower) {
            return loanAmounts[i];
        }
    }
    return 0.0;
}

void Bank::displayStatus() const {
    cout << "\nBANK STATUS";
    cout << "\n-----------";
    cout << "\nTreasury: " << treasury << " gold";
    cout << "\nInterest Rate: " << (interestRate * 100) << "%";
    cout << "\nActive Loans: " << activeLoanCount;
    
    if (activeLoanCount > 0) {
        cout << "\n\nLOAN DETAILS:";
        for (int i = 0; i < MAX_LOANS; i++) {
            if (borrowers[i] != "" && loanAmounts[i] > 0) {
                cout << "\n" << borrowers[i] << ": ";
                cout << loanAmounts[i] << " gold (";
                cout << loanDurations[i] << " turns remaining)";
            }
        }
    }
    cout << endl;
}