#include <bits/stdc++.h>
using namespace std;

class BloomFilter {
private:
    vector<unsigned long long> points;
    int size;
    std::hash<string> h; 

    int hashFunction(const string &value, int multiplier) const {
        size_t baseHash = h(value);
        return int((baseHash * multiplier) % size);
    }

    void mark(int h) {
        h %= size;
        points[h >> 6] |= (1ULL << (h & 63));
    }

    bool isMarked(int h) const {
        h %= size;
        return (points[h >> 6] & (1ULL << (h & 63))) != 0;
    }

public:
    BloomFilter(int s) {
        size = s;
        points.resize((size + 63) >> 6, 0);
    }

    void add(const string& value) {
        mark(hashFunction(value, 29));
        mark(hashFunction(value, 19));
        mark(hashFunction(value, 17));
    }

    bool exists(const string& value) const {
        return isMarked(hashFunction(value, 29)) &&
               isMarked(hashFunction(value, 19)) &&
               isMarked(hashFunction(value, 17));
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    BloomFilter bf(1000);

    bf.add("141432");
    bf.add("2354364");

    cout << boolalpha;
    cout << bf.exists("141432") << endl; // true
    cout << bf.exists("723") << endl;    // false (maybe true in rare case of collision)
    cout << bf.exists("1432") << endl;   // false (but bloom filter allows false positives)

    return 0;
}
