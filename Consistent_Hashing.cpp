#include <bits/stdc++.h>
using namespace std;
const int MOD = 1e9 + 7;
struct Node {
    string id;
    int weight;
    Node(string id, int weight) {
        this->id = id;
        this->weight = weight;
    }
};

struct Request {
    string id;
    Request(string id) {
        this->id = id;
    }
};

class ConsistentHashing {
    unordered_map<string, vector<long long>> nodePosition;
    map<long long, Node*> nodeMapping;
    int pointMultiplier;

    static long long defaultHash(string &s) {
        long long h = 0;
        for (char c : s) {
            h = (h * 131 + c) % MOD;
        }
        return h;
    }

    long long hashKey(string &key) {
        return defaultHash(key);
    }
public:
    ConsistentHashing(int multiplier) {
        pointMultiplier = multiplier;
        if (pointMultiplier == 0) {
            throw invalid_argument("pointMultiplier cannot be 0");
        }
    }
    void addNode(Node* node) {
        vector<long long> positions;
        for (int i = 0; i < pointMultiplier; i++) {
            for (int j = 0; j < node->weight; j++) {
                string key = to_string(i * pointMultiplier + j) + node->id;
                long long point = hashKey(key);
                positions.push_back(point);
                nodeMapping[point] = node;
            }
        }
        nodePosition[node->id] = positions;
    }

    void removeNode(Node* node) {
        auto it = nodePosition.find(node->id);
        if (it != nodePosition.end()) {
            for (auto point : it->second) {
                nodeMapping.erase(point);
            }
            nodePosition.erase(it);
        }
    }

    Node* getAssignedNode(Request& request) {
        long long key = hashKey(request.id);
        auto it = nodeMapping.upper_bound(key);
        if (it == nodeMapping.end()) {
            return nodeMapping.begin()->second;
        }
        return it->second;
    }
};
int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    ConsistentHashing ch(10);
    Node n1("NodeA", 2);
    Node n2("NodeB", 3);
    Node n3("NodeC", 4);


    ch.addNode(&n1);
    ch.addNode(&n2);
    ch.addNode(&n3);
    string s1,s2,s3;
    cin>>s1>>s2>>s3;

    Request r1(s1), r2(s2), r3(s3);

    cout << "Request1 assigned to: " << ch.getAssignedNode(r1)->id << endl;
    cout << "Request2 assigned to: " << ch.getAssignedNode(r2)->id << endl;
    cout << "Request3 assigned to: " << ch.getAssignedNode(r3)->id << endl;

    return 0;
}

//pointMultiplier is how many virtual points per node