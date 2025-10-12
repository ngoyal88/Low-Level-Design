#include<bits/stdc++.h>
using namespace std;

enum class ItemType{COKE, PEPSI, JUICE, WATER};
enum class CoinType{ONE=1, TWO=2, FIVE=5, TEN=10};

int cointoValue(CoinType c){
    switch(c){
        case CoinType::ONE: return 1;
        case CoinType::TWO: return 2;
        case CoinType::FIVE: return 5;
        case CoinType::TEN: return 10;
        default: return 0;
    }
}

class Item{
    ItemType type;
    double price;

    public:
    Item(double p, ItemType t): price(p), type(t) {}
    ItemType getType() const { return type; }
    double getPrice() const { return price; }
    void setPrice(double p){price = p;}
    void setType(ItemType t){ type = t; }
};

class ItemShelf{

    vector<Item> items;
    ItemType  type;
    bool isSoldOut;
    int capacity;
    public:
    ItemShelf(ItemType  type,double p): capacity(10), isSoldOut(false), type(type) {
        for(int i=0;i<capacity;i++) items.push_back(Item(p,type));
    }

    void addItem(const Item &item){
        if(items.size() < capacity && item.getType() == type){
            items.push_back(item);
            isSoldOut = false;
        }
        if(items.size() == capacity){ isSoldOut = true; }
    }

    bool dispenseItem(){
        if(!items.empty()){
            items.pop_back();
            if(items.empty()) isSoldOut = true;
            return true;
        }
        return false;
    }
    bool isFull() const { return items.size() == capacity; }
    int getItemCount() const { return items.size(); }
    bool soldOut() const { return isSoldOut; }
    ItemType getType() const { return type; }
};

class CashInventory{
    map<CoinType,int> cashMap;
    public:
    CashInventory(){
        cashMap[CoinType::ONE] = 10;
        cashMap[CoinType::TWO] = 10;
        cashMap[CoinType::FIVE] = 10;
        cashMap[CoinType::TEN] = 10;
    }

    void addCash(CoinType c,int count){
        cashMap[c] += count;
    }

    bool hasSufficientChange(int amount){
        int total = 0;
        for(auto &p : cashMap){
            total += cointoValue(p.first) * p.second;
        }
        return total >= amount;
    }
    map<CoinType,int> dispenseChange(int amount){
        if(!hasSufficientChange(amount)) return {};
        map<CoinType,int> toDispense;
        int rem = amount;
        vector<CoinType> denominations = {CoinType::TEN, CoinType::FIVE, CoinType::TWO, CoinType::ONE};
        int i=0;
        while(rem > 0 && i < (int)denominations.size()){
            CoinType denom = denominations[i];
            int val = cointoValue(denom);
            int available = cashMap[denom];
            int needed = rem / val;
            int toGive = min(available, needed);
            if(toGive > 0){
                toDispense[denom] = toGive;
                rem -= toGive * val;
                cashMap[denom] -= toGive;
            }
            i++;
        }
        return toDispense;
    }

    
};
class Inventory{
    vector<ItemShelf> shelf;
    int capacity;
    public:
    Inventory(int cokeshelf,int pepsishelf,int juiceshelf,int watershelf){
        capacity = cokeshelf + pepsishelf + juiceshelf + watershelf;
        for(int i=0;i<cokeshelf;i++) shelf.push_back(ItemShelf(ItemType::COKE,40.0));
        for(int i=0;i<pepsishelf;i++) shelf.push_back(ItemShelf(ItemType::PEPSI,35.0));
        for(int i=0;i<juiceshelf;i++) shelf.push_back(ItemShelf(ItemType::JUICE,30.0));
        for(int i=0;i<watershelf;i++) shelf.push_back(ItemShelf(ItemType::WATER,20.0));
    }

    bool addItem(const Item &item){
        for(auto &s : shelf){
            if(s.getType() == item.getType() && !s.isFull()){
                s.addItem(item);
                return true;
            }
        }
        return false;
    }

    bool dispenseItem(ItemType type){
        for(auto &s : shelf){
            if(s.getType() == type && !s.soldOut()){
                return s.dispenseItem();
            }
        }
        return false;
    }

    bool hasItem(ItemType type){
        for(auto &s : shelf){
            if(s.getType() == type && !s.soldOut()) return true;
        }
        return false;
    }
    void availableItems(){
        cout<<"Available items:\n";
        vector<int> counts(4,0);
        for(auto &s : shelf){
            counts[static_cast<int>(s.getType())] += s.getItemCount();
        }
        cout<<"COKE : "<<counts[0]<<"  | Cost : 40.0\n";
        cout<<"PEPSI: "<<counts[1]<<"  | Cost : 35.0\n";
        cout<<"JUICE: "<<counts[2]<<"  | Cost : 30.0\n";
        cout<<"WATER: "<<counts[3]<<"  | Cost : 20.0\n";
    }
};

class VendingMachine{
    Inventory inventory;
    CashInventory cashInventory;
    int currentBalance;
    public:
    VendingMachine(int cokeshelf,int pepsishelf,int juiceshelf,int watershelf)
    : inventory(cokeshelf,pepsishelf,juiceshelf,watershelf), currentBalance(0) {}

    void insertCoin(CoinType c){
        cashInventory.addCash(c,1);
        currentBalance += cointoValue(c);
        cout<<"Inserted coin: "<<cointoValue(c)<<", Current balance: "<<currentBalance<<"\n";
    }

    bool selectItem(ItemType type){
        if(!inventory.hasItem(type)){
            cout<<"Selected item is sold out\n";
            return false;
        }
        double price = 0.0;
        switch(type){
            case ItemType::COKE: price = 40.0; break;
            case ItemType::PEPSI: price = 35.0; break;
            case ItemType::JUICE: price = 30.0; break;
            case ItemType::WATER: price = 20.0; break;
            default: price = 0.0; break;
        }
        if(currentBalance < price){
            cout<<"Insufficient balance. Please insert more coins.\n";
            return false;
        }
        // dispense item
        if(inventory.dispenseItem(type)){
            currentBalance -= price;
            cout<<"Dispensed item of type "<<static_cast<int>(type)<<"\n";
            // dispense change if any
            if(currentBalance > 0){
                auto change = cashInventory.dispenseChange(currentBalance);
                if(!change.empty()){
                    cout<<"Dispensed change:\n";
                    for(auto &p : change){
                        cout<<cointoValue(p.first)<<" x "<<p.second<<"\n";
                    }
                    currentBalance = 0;
                } else {
                    cout<<"Unable to dispense full change. Please contact support.\n";
                }
            }
            return true;
        }
        return false;
    }
    void cancelTransaction(){
        if(currentBalance > 0){
            auto change = cashInventory.dispenseChange(currentBalance);
            if(!change.empty()){
                cout<<"Transaction cancelled. Dispensed refund:\n";
                for(auto &p : change){
                    cout<<cointoValue(p.first)<<" x "<<p.second<<"\n";
                }
                currentBalance = 0;
            } else {
                cout<<"Unable to dispense full refund. Please contact support.\n";
            }
        } else {
            cout<<"No balance to refund.\n";
        }
    }
    void showAvailableItems(){
        inventory.availableItems();
    }

    int getCurrentBalance() const { return currentBalance; }
};

int main(){
    VendingMachine vm(5,5,5,5);
    vm.insertCoin(CoinType::ONE);
    vm.insertCoin(CoinType::FIVE);
    vm.insertCoin(CoinType::FIVE);
    vm.insertCoin(CoinType::FIVE);
    vm.showAvailableItems();
    vm.selectItem(ItemType::COKE);
    vm.cancelTransaction();
    return 0;
}