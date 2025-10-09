#include <bits/stdc++.h>
using namespace std;

enum class CashType{BILL_2000 = 2000,BILL_500 = 500, BILL_200 = 200, BILL_100 = 100};

enum class TransactionType{WITHDRAWAL, DEPOSIT, BALANCE_INQUIRY};

class Account{
    string name;
    double balance;
    string accountNumber;
    public:
    Account(string n, string accNum,int bal): name(n), balance(bal), accountNumber(accNum) {}

    string getAccountNumber(){ return accountNumber; }
    string getName(){ return name; }
    double getBalance(){ return balance; }
    void deposit(double amount){ balance += amount; }
    bool withdraw(double amount){
        if(amount > balance) return false;
        balance -= amount;
        return true;
    }
};
class Card{
    Account &account;
    string cardNumber;
    int PIN;
    public:
    Card(Account &acc,string cnum,int pin): account(acc), cardNumber(cnum), PIN(pin) {}

    string getCardNumber(){ return cardNumber; }
    Account& getAccount(){ return account; }
    bool verifyPIN(int pin){ return PIN == pin; }
};
class ATMinventory{
    map<CashType,int> cashInventory; // denomination -> count
    public:
    ATMinventory(){

        initializeCash();
    }

    void initializeCash(){
        cashInventory[CashType::BILL_2000] = 10;
        cashInventory[CashType::BILL_500] = 20;
        cashInventory[CashType::BILL_200] = 30;
        cashInventory[CashType::BILL_100] = 50;
    }
    
    int getTotalCash(){
        int total = 0;
        for(auto &p : cashInventory){
            CashType bill = p.first;
            int count = p.second;
            total += static_cast<int>(bill) * count;
        }
        return total;
    }

    bool hasSufficientCash(int amount){
        return getTotalCash() >= amount;
    }

    map<CashType,int> dispenseCash(int amount){
        if(!hasSufficientCash(amount)) return {};

        map<CashType,int> toDispense;
        int remaining = amount;
        vector<CashType> denominations = {CashType::BILL_2000, CashType::BILL_500, CashType::BILL_200, CashType::BILL_100};
        for(CashType denom : denominations){
            int billValue = static_cast<int>(denom);
            int available = cashInventory[denom];
            int needed = remaining / billValue;
            int toGive = min(available, needed);
            if(toGive > 0){
                toDispense[denom] = toGive;
                remaining -= toGive * billValue;
                cashInventory[denom] -= toGive;
            }
        }
        if(remaining == 0) return toDispense;
        // If we couldn't dispense the exact amount, rollback
        for(auto &[bill,count] : toDispense){
            cashInventory[bill] += count;
        }
        return {};
    }

    void addCash(CashType type, int count){
        cashInventory[type] += count;
    }

};

// Simple transaction record
struct Transaction{
    TransactionType type;
    double amount;
    bool success;
    std::string note;
    std::time_t timestamp;
};

// ATM class -- handles card interaction + transactions
class ATM{
    ATMinventory inventory;
    Card *insertedCard = nullptr;
    bool authenticated = false;
    vector<Transaction> history;
    public:
    ATM() = default;

    bool insertCard(Card &card){
        if(insertedCard) return false; // already a card
        insertedCard = &card;
        authenticated = false;
        return true;
    }
    void ejectCard(){ insertedCard = nullptr; authenticated = false; }

    bool enterPIN(int pin){
        if(!insertedCard) return false;
        authenticated = insertedCard->verifyPIN(pin);
        return authenticated;
    }

    // Withdraw: checks account balance and ATM denominations
    bool withdraw(int amount){
        Transaction tx{TransactionType::WITHDRAWAL, (double)amount, false, "", std::time(nullptr)};
        if(!insertedCard || !authenticated){ tx.note = "No card or not authenticated"; history.push_back(tx); return false; }
        if(amount <= 0){ tx.note = "Invalid amount"; history.push_back(tx); return false; }
        if(amount % 100 != 0){ tx.note = "Amount must be multiple of 100"; history.push_back(tx); return false; }
        Account &acc = insertedCard->getAccount();
        if(acc.getBalance() < amount){ tx.note = "Insufficient funds in account"; history.push_back(tx); return false; }

        // Ask inventory to dispense first
        auto dispensed = inventory.dispenseCash(amount);
        if(dispensed.empty()){ tx.note = "ATM cannot dispense exact denominations or insufficient ATM cash"; history.push_back(tx); return false; }

        // Try to withdraw from account. If it fails, roll back inventory.
        if(!acc.withdraw(amount)){
            // rollback
            for(auto &p : dispensed) inventory.addCash(p.first, p.second);
            tx.note = "Account withdraw failed"; history.push_back(tx); return false;
        }

        tx.success = true;
        tx.note = "Dispensed cash";
        history.push_back(tx);
        printDispensed(dispensed);
        return true;
    }

    // Deposit: accept a map of denominations -> counts
    bool deposit(const map<CashType,int> &depositBills){
        Transaction tx{TransactionType::DEPOSIT, 0.0, false, "", std::time(nullptr)};
        if(!insertedCard || !authenticated){ tx.note = "No card or not authenticated"; history.push_back(tx); return false; }
        int total = 0;
        for(auto &p : depositBills){
            total += static_cast<int>(p.first) * p.second;
        }
        if(total <= 0){ tx.note = "No cash provided"; history.push_back(tx); return false; }
        // add to ATM inventory
        for(auto &p : depositBills) inventory.addCash(p.first, p.second);
        // credit account
        insertedCard->getAccount().deposit(total);
        tx.type = TransactionType::DEPOSIT;
        tx.amount = total;
        tx.success = true;
        tx.note = "Deposit accepted";
        history.push_back(tx);
        return true;
    }

    double balanceInquiry(){
        Transaction tx{TransactionType::BALANCE_INQUIRY, 0.0, false, "", std::time(nullptr)};
        if(!insertedCard || !authenticated){ tx.note = "No card or not authenticated"; history.push_back(tx); return -1; }
        double bal = insertedCard->getAccount().getBalance();
        tx.type = TransactionType::BALANCE_INQUIRY;
        tx.amount = bal;
        tx.success = true;
        tx.note = "Balance returned";
        history.push_back(tx);
        return bal;
    }

    const vector<Transaction>& getHistory() const{ return history; }

    void printDispensed(const map<CashType,int> &d){
        std::cout<<"Dispensed:\n";
        for(auto &p : d){
            std::cout<<static_cast<int>(p.first)<<" x "<<p.second<<"\n";
        }
    }

    void printATMStatus(){
        std::cout<<"ATM total cash: "<<inventory.getTotalCash()<<"\n";
    }
};




int main(){
    cout << "Welcome to ATM" << endl;
    Account acc("Nikhil", "12345", 25000);
    Card card(acc, "98765", 4321);
    ATM atm;

    // Insert card
    if (!atm.insertCard(card)) {
        cout << "Failed to insert card." << endl;
        return 1;
    }

    // Enter PIN
    if (!atm.enterPIN(4321)) {
        cout << "PIN incorrect." << endl;
        return 1;
    }

    // Deposit cash
    map<CashType, int> depositBills = {
        {CashType::BILL_2000, 2}, // 4000
        {CashType::BILL_500, 4}   // 2000
    };
    
    if (atm.deposit(depositBills)) {
        cout << "Deposit successful." << endl;
    } else {
        cout << "Deposit failed." << endl;
    }

    // Withdraw cash
    int withdrawAmount = 5000;
    if (atm.withdraw(withdrawAmount)) {
        cout << "Withdraw successful." << endl;
    } else {
        cout << "Withdraw failed." << endl;
    }

    // Balance inquiry
    double bal = atm.balanceInquiry();

    // Print ATM status
    atm.printATMStatus();

    // Print transaction history
    cout << "\nTransaction History:" << endl;
    for (const auto& tx : atm.getHistory()) {
        cout << "Type: ";
        switch (tx.type) {
            case TransactionType::WITHDRAWAL: cout << "Withdrawal"; break;
            case TransactionType::DEPOSIT: cout << "Deposit"; break;
            case TransactionType::BALANCE_INQUIRY: cout << "Balance Inquiry"; break;
        }
        cout << ", Amount: " << tx.amount << ", Success: " << (tx.success ? "Yes" : "No") << ", Note: " << tx.note << endl;
    }

    atm.ejectCard();
    return 0;
}