#include<bits/stdc++.h>
using namespace std;

enum class VehicleType { CAR, BIKE, TRUCK };

string VehicleTypeToString(VehicleType type) {
    switch (type) {
        case VehicleType::CAR: return "Car";
        case VehicleType::BIKE: return "Bike";
        case VehicleType::TRUCK: return "Truck";
        default: return "Unknown";
    }
}
class Vehicle{
    string LicencePlate;
    VehicleType type;
public:
Vehicle(string plate,VehicleType t) : LicencePlate(plate), type(t) {}
VehicleType getType(){return type;}
string getLicencePlate(){return LicencePlate;}

};
class ParkingSpot{
    int id;
    bool isOccupied;
    VehicleType type;
    Vehicle *vehicle;
public:
ParkingSpot(int i, VehicleType t) : id(i), type(t), isOccupied(false), vehicle(nullptr) {}
bool getIsOccupied(){return isOccupied;}
VehicleType getType(){return type;}
Vehicle* getVehicle(){return vehicle;}
void parkVehicle(Vehicle *v){
    vehicle = v;
    isOccupied = true;
}
void removeVehicle(){
    vehicle = NULL;
    isOccupied = false;
}
string info(){
if(!isOccupied) return "Spot " + to_string(id) + " (" + VehicleTypeToString(type) + ") : Empty";
else return "Spot " + to_string(id) + " (" + VehicleTypeToString(type) + ") : Occupied by " + vehicle->getLicencePlate();
}
};

class ParkingFloor{
    int floorNumber;
    vector<ParkingSpot> spots;
    public:
    ParkingFloor(int floor,int bike,int car,int truck) : floorNumber(floor){
        int spotIndex = 0;
        for(int i=0;i<bike;i++) {
            spots.push_back(ParkingSpot(spotIndex++, VehicleType::BIKE));
        }
        for(int i=0;i<car;i++) {
            spots.push_back(ParkingSpot(spotIndex++, VehicleType::CAR));
        }
        for(int i=0;i<truck;i++) {
            spots.push_back(ParkingSpot(spotIndex++, VehicleType::TRUCK));
        }
    }

    ParkingSpot* getSpot(VehicleType type){
        for(auto & spot : spots){
            if(spot.getType() == type && !spot.getIsOccupied()){
                return &spot;
            }
        }
        return nullptr;
    }

    bool parkVehicle(Vehicle* v){
        ParkingSpot* spot = getSpot(v->getType());
        if(!spot){return false;}
        spot->parkVehicle(v);
        return true;
    }

    bool removeVehicle(string plate){
        for(auto & spot : spots){
            if(spot.getIsOccupied() && spot.getVehicle()->getLicencePlate() == plate){
                spot.removeVehicle();
                return true;
            }
        }
        return false;
    }

    void display(){
        cout<<"Floor No. :"<<floorNumber<<endl;
        for(auto & spot : spots){
            cout<<' '<<spot.info()<<endl;
        }

    }

};

class ParkingLot{
    string name;
    vector<ParkingFloor> floors;
public:
ParkingLot(string n,int nfloor,int nbike,int ncar,int ntruck): name(n) {
    for(int i=0;i<nfloor;i++){
        floors.push_back(ParkingFloor(i,nbike,ncar,ntruck));
    }
}
bool parkVehicle(Vehicle* v){
    for(auto & floor : floors){
        if(floor.parkVehicle(v)){
            cout<<"Parked vehicle "<<v->getLicencePlate()<<endl;
            return true;
        }
    }
    cout<<"Parking Full for vehicle "<<v->getLicencePlate()<<endl;
    return false;
}
bool unparkVehicle(string Plate){
    for(auto & floor : floors){
        if(floor.removeVehicle(Plate)){
            cout<<"Unparked vehicle "<<Plate<<endl;
            return true;
        }
    }
    cout<<"Vehicle "<<Plate<<" not found"<<endl;
    return false;
}
void displayStatus(){
    cout<<"Parking Lot: "<<name<<endl;
    for(auto & floor : floors)floor.display();
    cout<<"============================================"<<endl;
}
};

int main() {
    ParkingLot lot("MyParkingLot",3,2,3,1);
    lot.displayStatus();
    Vehicle v1("PB-01-HH-1234",VehicleType::CAR);
    Vehicle v2("PB-01-HH-9999",VehicleType::CAR);
    Vehicle v3("PB-01-BB-0001",VehicleType::BIKE);
    Vehicle v4("PB-01-HH-7777",VehicleType::BIKE);
    Vehicle v5("PB-01-HH-2701",VehicleType::TRUCK);
    Vehicle v6("PB-11-HH-3141",VehicleType::TRUCK);

    lot.parkVehicle(&v1);
    lot.parkVehicle(&v2);
    lot.parkVehicle(&v3);
    lot.parkVehicle(&v4);

    lot.displayStatus();
    lot.unparkVehicle("PB-01-HH-9999");
    lot.displayStatus();
    
    return 0;
}