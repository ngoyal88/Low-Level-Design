#include<bits/stdc++.h>
using namespace std;

enum class Direction { UP = 1, DOWN = -1, IDLE = 0};

string DirectionToString(Direction dir){
    switch(dir){
        case Direction::UP: return "UP";
        case Direction::DOWN: return "DOWN";
        case Direction::IDLE: return "IDLE";
        default: return "UNKNOWN";
    }
}

struct Request{
    int from;
    Direction direction;
    Request(int f,Direction d): from(f),direction(d){}
};

class Elevator{
    int id;
    int currentFloor;
    Direction direction;
    bool doorOpen;
    int capacity;
    int currentLoad;
    set<int> upStops;
    set<int ,greater<>> downStops; 
    public:
    Elevator(int i,int startFloor): id(i),currentFloor(startFloor),direction(Direction::IDLE),doorOpen(false),capacity(10),currentLoad(0){}
    
    int getid(){return id;}
    int getcurrentFloor(){return currentFloor;}
    Direction getDirection(){return direction;}
    bool isIdle(){return direction == Direction::IDLE;}
    bool enter(){ 
        if(currentLoad < capacity){
            currentLoad++;
            return true;
        }
        return false;
    }
    bool exit(){
        if(currentLoad > 0){
            currentLoad--;
            return true;
        }
        return false;
    }

    void addTarget(int floor){
        if(floor == currentFloor){
            doorOpen = true;
            return;
        }
        if(floor > currentFloor) upStops.insert(floor);
        else downStops.insert(floor);

        if(direction  == Direction::IDLE){
            if(!upStops.empty()) direction = Direction::UP;
            else if(!downStops.empty()) direction = Direction::DOWN;
        }
    }

    int distance(int floor){
        return abs(currentFloor - floor);
     }

    void selectFloor(int floor){
        addTarget(floor);
     }

     bool isgoingTowards(int floor,Direction reqDir){
        if(direction == Direction::UP && floor>= currentFloor && reqDir == Direction::UP) return true;
        if(direction == Direction::DOWN && floor<= currentFloor && reqDir == Direction::DOWN) return true;
        return false;
     }

    string status(){
        string s = "Elevator " + to_string(id) + " | Floor: " + to_string(currentFloor) +
                    " | dir: " + DirectionToString(direction) + " | door: " + (doorOpen ? "Open" : "Closed");

        s += " upStops: ";
        for(int f :upStops) s+=to_string(f)+ " ";
        s+=" downStops: ";
        for(int f :downStops) s+=to_string(f)+ " ";
        return s;
    }

    void step(){
        if(doorOpen){
            doorOpen = false;
            removeStop(currentFloor);
            updateDirection();
            return;
        }

        if(direction == Direction::UP){ 
            currentFloor++;
            if(hasStopAt(currentFloor)) {
                doorOpen = true;
            }
        }
        else if(direction == Direction::DOWN){ 
            currentFloor--;
            if(hasStopAt(currentFloor)) {
                doorOpen = true; 
            }
        }
        if(direction != Direction::IDLE) updateDirection();
    }


private:
    bool hasStopAt(int floor){
        return upStops.count(floor) || downStops.count(floor);
    }

    void removeStop(int floor){
        upStops.erase(floor);
        downStops.erase(floor);
    }

    void updateDirection(){

        upStops.erase(currentFloor);
        downStops.erase(currentFloor);

        if(direction == Direction::UP ){
            auto it = upStops.lower_bound(currentFloor);
            if(it != upStops.end() && *it > currentFloor) {
                direction = Direction::UP;
                return;
            }
            if(!downStops.empty()){ 
                direction = Direction::DOWN;
                return;
            }
            if(upStops.empty() && downStops.empty()) direction = Direction::IDLE;
        }else if(direction == Direction::DOWN){
            if(!downStops.empty()){
                // downStops is ordered with greater<>, so lower_bound(currentFloor)
                // gives the first element <= currentFloor. We need a strictly lower stop.
                auto it = downStops.lower_bound(currentFloor);
                if(it != downStops.end() && *it < currentFloor) {
                    direction = Direction::DOWN;
                    return;
                }
            }
            if(!upStops.empty()){
                direction = Direction::UP;
                return;
            }
            if(upStops.empty() && downStops.empty()) direction = Direction::IDLE;
        }else{
            if(!upStops.empty()) direction = Direction::UP;
            else if(!downStops.empty()) direction = Direction::DOWN;
        }
    }
};

class ElevatorController{
    vector<Elevator> elevators;
    int minFloor;
    int maxFloor;
    queue<Request> pendingRequest;
    public:
    ElevatorController(int numElevators,int minF,int maxF) : minFloor(minF), maxFloor(maxF){
        for(int i=0;i<numElevators;i++){
            elevators.push_back(Elevator(i,minF));
        }
    }

    void pickup(int floor,Direction direction){
        if(floor< minFloor || floor > maxFloor){
            cout<<"Invalid floor request "<<floor<<endl;
            return;
        }

        int bestIdx = -1;
        int bestScore = INT_MAX;

        // Prefer elevators already moving towards the request, choose the closest among them
        for(size_t i=0;i<elevators.size();i++){
            auto &e = elevators[i];
            if(e.isgoingTowards(floor,direction)){
                int dist = e.distance(floor);
                if(dist < bestScore){
                    bestScore = dist;
                    bestIdx = static_cast<int>(i);
                }
            }
        }

        if(bestIdx == -1){
            for(size_t i=0;i<elevators.size();i++){
                auto &e = elevators[i];
                if(e.isIdle()){
                    int dist = e.distance(floor);
                    if(dist < bestScore){
                        bestScore = dist;
                        bestIdx = static_cast<int>(i);
                    }
                }
            }
        }
        // If still not found, choose the nearest elevator regardless of direction (last resort)
        if(bestIdx == -1){
            for(size_t i=0;i<elevators.size();i++){
                auto &e = elevators[i];
                int dist = e.distance(floor);
                if(dist < bestScore){
                    bestScore = dist;
                    bestIdx = static_cast<int>(i);
                }
            }
        }

        if(bestIdx != -1){
            elevators[bestIdx].addTarget(floor);
            cout<<"Assigned elevator "<<elevators[bestIdx].getid()<<" to pickup at floor "<<floor<<endl;
        }else{
            pendingRequest.push(Request(floor,direction));
            cout<<"Enqueued request at floor "<<floor<<endl; 
        }
    }

    void selectFloor(int eid,int floor){
        if(eid < 0 || eid >= (int)elevators.size()) return; //out of bounds
        if(floor < minFloor || floor > maxFloor) return;
        elevators[eid].selectFloor(floor);
    }

    void step(){
        for(auto &e : elevators) e.step();

        int pendingCount = pendingRequest.size();
        for(int i=0;i<pendingCount;i++){
            Request r = pendingRequest.front();
            pendingRequest.pop();
            int bestIdx = -1;
            int bestScore = INT_MAX;
            // Try assign using same heuristic as pickup
            for(size_t j=0;j<elevators.size();j++){
                auto &e = elevators[j];
                if(e.isgoingTowards(r.from, r.direction)){
                    int dist = e.distance(r.from);
                    if(dist < bestScore){
                        bestScore = dist;
                        bestIdx = static_cast<int>(j);
                    }
                }
            }
            if(bestIdx == -1){
                for(size_t j=0;j<elevators.size();j++){
                    auto &e = elevators[j];
                    if(e.isIdle()){
                        int dist = e.distance(r.from);
                        if(dist < bestScore){
                            bestScore = dist;
                            bestIdx = static_cast<int>(j);
                        }
                    }
                }
            }
            if(bestIdx == -1){
                for(size_t j=0;j<elevators.size();j++){
                    auto &e = elevators[j];
                    int dist = e.distance(r.from);
                    if(dist < bestScore){
                        bestScore = dist;
                        bestIdx = static_cast<int>(j);
                    }
                }
            }

            if(bestIdx != -1){
                elevators[bestIdx].addTarget(r.from);
                cout<<"Assigned elevator "<<elevators[bestIdx].getid()<<" to pending pickup at floor "<<r.from<<endl;
            } else {
                // Could not assign now; requeue
                pendingRequest.push(r);
            }
        }
    }

    void status(){
        for(auto &e: elevators){
            cout<<e.status()<<endl;
        }
        cout<<"Pending requests: "<<pendingRequest.size()<<endl;
    }
};
int main(){
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    cout<<"=== Elevator LLD Simulation ==="<<endl;
    ElevatorController controller(3,0,10);

    // Seed some pickup requests
    controller.pickup(0, Direction::UP);
    controller.pickup(5, Direction::DOWN);
    controller.pickup(9, Direction::DOWN);
    controller.pickup(3, Direction::UP);

    // Run simulation for a number of ticks
    for(int t = 0; t < 20; ++t){
        cout << "\n--- Tick " << t << " ---\n";
        controller.status();
        controller.step();
        this_thread::sleep_for(chrono::milliseconds(150));
    }

    cout << "\n--- Final Status ---\n";
    controller.status();
    return 0;
}