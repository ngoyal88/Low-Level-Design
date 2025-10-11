#include<bits/stdc++.h>
using namespace std;

enum class InventoryOperation {ADD,REMOVE,TRANSFER,ADJUST};
enum class ItemType {ELECTRONICS, GROCERIES, CLOTHING, FURNITURE, OTHER};


//<===========ITEM CLASS===========>
class Item{
    string SKU;
    string name;
    double price;
    int quantity;
    int threshold;
    ItemType type;
    public:
    string getSKU(){ return SKU; }
    string getName(){ return name; }
    double getPrice(){ return price; }
    int getQuantity(){ return quantity; }
    int getThreshold(){ return threshold; }
    ItemType getType(){ return type; }

    void setSKU(string &sku){ SKU = sku; }
    void setName(string &n){ name = n; }
    void setPrice(double p){ price = p; }
    void setQuantity(int q){ quantity = q; }
    void setThreshold(int t){ threshold = t; }
    void setType(ItemType t){ type = t; }
};

//<========Clothing Item =========>
class ClothingItem : public Item{
    string size;
    string color;
    public:
    ClothingItem(string sku,string name,double price, int quantity,int threshold){
        setName(name);
        setSKU(sku);
        setPrice(price);
        setQuantity(quantity);
        setThreshold(threshold);
        setType(ItemType::CLOTHING);
    }

    string getSize() {return size;}
    string getColor() {return color;}

    void setSize(string s){ size = s;}
    void setColor(string c){ color = c;}
};


//<========Electronis Item =========>
class ElectronicsItem : public Item{
    string brand;
    int warranty;
    public:
    ElectronicsItem(string sku,string name,double price, int quantity,int threshold){
        setName(name);
        setSKU(sku);
        setPrice(price);
        setQuantity(quantity);
        setThreshold(threshold);
        setType(ItemType::ELECTRONICS);
    }

    string getBrand() {return brand;}
    int getWarranty() {return warranty;}

    void setBrand(string b){ brand = b;}
    void setWarranty(int w){ warranty = w;}
};

//<========Grocery Item =========>
class GroceryItem : public Item{
    int shelfLife; // in days
    bool isRefrigerated;
    string color;
    public:
    GroceryItem(string sku,string name,double price, int quantity,int threshold){
        setName(name);
        setSKU(sku);
        setPrice(price);
        setQuantity(quantity);
        setThreshold(threshold);
        setType(ItemType::GROCERIES);
    }

    int getShelfLife() { return shelfLife; }
    bool getIsRefrigerated() { return isRefrigerated; }

    void setShelfLife(int s){ shelfLife = s;}
    void setIsRefrigerated(bool r){ isRefrigerated = r;}
};

class ItemFactory{
    public:
    Item* createItem(ItemType type, string sku,string name,double price, int quantity,int threshold){
        switch(type){
            case ItemType::CLOTHING:
                return new ClothingItem(sku,name,price,quantity,threshold);
            case ItemType::ELECTRONICS:
                return new ElectronicsItem(sku,name,price,quantity,threshold);
            case ItemType::GROCERIES:
                return new GroceryItem(sku,name,price,quantity,threshold);
            default:
                return new Item();
        }
    }
};
//========REPLENISHMENT STRATEGY =======>
class ReplenishmentStrategy{
    public:
    virtual ~ReplenishmentStrategy() = default;
    virtual void replenish(Item *item) = 0;
};

class BulkOrder : public ReplenishmentStrategy{
    public:
    void replenish(Item *item) override {
        cout<<"Placing bulk order for item: "<<item->getName()<<endl;
    }
};

class JustInTime : public ReplenishmentStrategy{
    public:
    void replenish(Item *item) override {
        cout<<"Placing just-in-time order for item: "<<item->getName()<<endl;
    }
};

//=======WAREHOUSE CLASS========
class Warehouse{
    int id;
    string name;
    string location;
    map<string,Item*> inventory;
    public:
    Warehouse(int id,string name){
        this->id = id;
        this->name = name;
    }
    int getId() const { return id; }
    void setLocation(string loc){ location = loc; }

    void addItem(Item *item,int quantity){
        string sku = item->getSKU();
        if(inventory.find(sku) != inventory.end()){
            Item *existing = inventory[sku];
            existing->setQuantity(existing->getQuantity() + quantity);
        } else {
            item->setQuantity(quantity);
            inventory[sku] = item;
        }
        cout<<"Added "<<quantity<<" of "<<item->getName()<<" to warehouse "<<name<<endl;
    }

    bool removeItem(string sku,int quantity){
        if(inventory.find(sku) != inventory.end()){
            Item *existing = inventory[sku];
            if(existing->getQuantity() >= quantity){
                existing->setQuantity(existing->getQuantity() - quantity);
                cout<<"Removed "<<quantity<<" of "<<existing->getName()<<" from warehouse "<<name<<endl;
                return true;
            } else {
                cout<<"Insufficient quantity of "<<existing->getName()<<" in warehouse "<<name<<endl;
                return false;
            }
        } else {
            cout<<"Item with SKU "<<sku<<" not found in warehouse "<<name<<endl;
            return false;
        }
    }

    int getItemQuantity(string sku){
        if(inventory.find(sku) != inventory.end()){
            return inventory[sku]->getQuantity();
        }
        return 0;
    }


    Item* getItem(string sku){
        if(inventory.find(sku) != inventory.end()){
            return inventory[sku];
        }
        return nullptr;
    }

    vector<Item*> getStock(){
        vector<Item*> items;
        for(auto &p : inventory){
            items.push_back(p.second);
        }
        return items;
    }

    void printInventory(){
        cout<<"Inventory of Warehouse "<<name<<":\n";
        for(auto &p : inventory){
            Item *item = p.second;
            cout<<"SKU: "<<item->getSKU()<<", Name: "<<item->getName()<<", Quantity: "<<item->getQuantity()<<endl;
        }
    }
};

//========INVENTORY MANAGER CLASS========
class InventoryManager{
    vector<Warehouse> warehouses;
    ItemFactory itemFactory;
    ReplenishmentStrategy *replenishmentStrategy;
    public:
    InventoryManager(ReplenishmentStrategy *strategy){
        replenishmentStrategy = strategy;
    }

    ~InventoryManager(){
        delete replenishmentStrategy;
        replenishmentStrategy = nullptr;
    }

    void setReplenishmentStrategy(ReplenishmentStrategy *strategy){
        if(replenishmentStrategy != strategy){
            delete replenishmentStrategy;
            replenishmentStrategy = strategy;
        }
    }

    void addWarehouse(int id,string name){
        warehouses.push_back(Warehouse(id,name));
        cout<<"Added warehouse: "<<name<<endl;
    }
    Warehouse* getWarehouse(int id){
        for(auto &w : warehouses){
            if(w.getId() == id) return &w;
        }
        return nullptr;
    }

    bool removeWarehouse(int id){
        for(auto it = warehouses.begin(); it != warehouses.end(); it++){
            if(it->getId() == id){
                warehouses.erase(it);
                cout<<"Removed warehouse with id: "<<id<<endl;
                return true;
            }
        }
        cout<<"Warehouse with id "<<id<<" not found"<<endl;
        return false;
    }

    Item* getItem(string sku){
        for(auto &w : warehouses){
            Item *item = w.getItem(sku);
            if(item) return item;
        }
        return nullptr;
    }

    void checkAndReplenish(string &sku){
        Item * item = getItem(sku);
        if(item && item->getQuantity() < item->getThreshold()){
            replenishmentStrategy->replenish(item);
        }
    }

    void performInventoryCheck(){
        for(Warehouse &w : warehouses){
            for(Item *item : w.getStock()){
                if(item->getQuantity() < item->getThreshold()){
                    replenishmentStrategy->replenish(item);
                }
            }
        }
    }


};



int main(){
    ReplenishmentStrategy *replenishmentStrategy = new JustInTime();

    // Create an InventoryManager instance
    InventoryManager inventoryManager(replenishmentStrategy);

    // Create and add warehouses
    Warehouse warehouse1(1, "Warehouse 1");
    Warehouse warehouse2(2, "Warehouse 2");

    inventoryManager.addWarehouse(1, "Warehouse 1");
    inventoryManager.addWarehouse(2, "Warehouse 2");

    // Create products using ItemFactory
    ItemFactory factory;

    Item *laptop = factory.createItem(ItemType::ELECTRONICS, "SKU123", "Laptop", 1000.0, 50, 25);
    Item *tShirt = factory.createItem(ItemType::CLOTHING, "SKU456", "T-Shirt", 20.0, 200, 100);
    Item *apple  = factory.createItem(ItemType::GROCERIES, "SKU789", "Apple", 1.0, 100, 200);
    
    // Add products to warehouses
    Warehouse *w1 = inventoryManager.getWarehouse(1);
    Warehouse *w2 = inventoryManager.getWarehouse(2);

    if (w1 && w2) {
        w1->addItem(laptop, 15);
        w1->addItem(tShirt, 20);
        w2->addItem(apple, 50);
    }

    // Set replenishment strategy to Just-In-Time
    inventoryManager.setReplenishmentStrategy(new JustInTime());

    // Perform inventory check and replenish if needed
    inventoryManager.performInventoryCheck();

    // Switch replenishment strategy to Bulk Order
    inventoryManager.setReplenishmentStrategy(new BulkOrder());

    // Replenish a specific product if needed
    string skuToReplenish = "SKU123";
    inventoryManager.checkAndReplenish(skuToReplenish);

    // Optional: Print final inventories
    if (w1) w1->printInventory();
    if (w2) w2->printInventory();

    return 0;
}