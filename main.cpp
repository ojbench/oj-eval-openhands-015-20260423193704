#include <iostream>
#include <fstream>
#include <cstring>
#include <map>
#include <set>
#include <algorithm>
#include <string>

using namespace std;

const char* DB_FILE = "database.dat";
const int MAX_KEY_LEN = 64;

struct Record {
    char key[MAX_KEY_LEN + 1];
    int value;
    
    Record() : value(0) {
        memset(key, 0, sizeof(key));
    }
    
    Record(const string& k, int v) : value(v) {
        memset(key, 0, sizeof(key));
        strncpy(key, k.c_str(), MAX_KEY_LEN);
    }
};

class FileDatabase {
private:
    map<string, set<int>> data;
    
    void loadFromFile() {
        ifstream file(DB_FILE, ios::binary);
        if (!file.is_open()) {
            return;
        }
        
        Record rec;
        while (file.read((char*)&rec, sizeof(Record))) {
            data[string(rec.key)].insert(rec.value);
        }
        file.close();
    }
    
    void saveToFile() {
        ofstream file(DB_FILE, ios::binary | ios::trunc);
        for (const auto& pair : data) {
            for (int value : pair.second) {
                Record rec(pair.first, value);
                file.write((const char*)&rec, sizeof(Record));
            }
        }
        file.close();
    }
    
public:
    FileDatabase() {
        loadFromFile();
    }
    
    ~FileDatabase() {
        saveToFile();
    }
    
    void insert(const string& key, int value) {
        data[key].insert(value);
    }
    
    void remove(const string& key, int value) {
        auto it = data.find(key);
        if (it != data.end()) {
            it->second.erase(value);
            if (it->second.empty()) {
                data.erase(it);
            }
        }
    }
    
    void findKey(const string& key) {
        auto it = data.find(key);
        if (it == data.end() || it->second.empty()) {
            cout << "null" << endl;
        } else {
            bool first = true;
            for (int value : it->second) {
                if (!first) cout << " ";
                cout << value;
                first = false;
            }
            cout << endl;
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n;
    cin >> n;
    
    FileDatabase db;
    
    for (int i = 0; i < n; i++) {
        string cmd;
        cin >> cmd;
        
        if (cmd == "insert") {
            string key;
            int value;
            cin >> key >> value;
            db.insert(key, value);
        } else if (cmd == "delete") {
            string key;
            int value;
            cin >> key >> value;
            db.remove(key, value);
        } else if (cmd == "find") {
            string key;
            cin >> key;
            db.findKey(key);
        }
    }
    
    return 0;
}
