#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <string>
#include <functional>

using namespace std;

const int NUM_FILES = 16;
const int MAX_KEY_LEN = 64;

struct Record {
    char key[MAX_KEY_LEN + 1];
    int value;
    bool deleted;
    
    Record() : value(0), deleted(false) {
        memset(key, 0, sizeof(key));
    }
    
    Record(const string& k, int v, bool del = false) : value(v), deleted(del) {
        memset(key, 0, sizeof(key));
        strncpy(key, k.c_str(), MAX_KEY_LEN);
    }
};

class FileDatabase {
private:
    string getFileName(const string& key) {
        hash<string> hasher;
        size_t h = hasher(key) % NUM_FILES;
        return "db_" + to_string(h) + ".dat";
    }
    
    vector<Record> loadFile(const string& filename) {
        vector<Record> records;
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            return records;
        }
        
        Record rec;
        while (file.read((char*)&rec, sizeof(Record))) {
            records.push_back(rec);
        }
        file.close();
        return records;
    }
    
    void saveFile(const string& filename, const vector<Record>& records) {
        ofstream file(filename, ios::binary | ios::trunc);
        for (const auto& rec : records) {
            file.write((const char*)&rec, sizeof(Record));
        }
        file.close();
    }
    
    void appendRecord(const string& filename, const Record& rec) {
        ofstream file(filename, ios::binary | ios::app);
        file.write((const char*)&rec, sizeof(Record));
        file.close();
    }
    
    void compactFile(const string& filename) {
        vector<Record> records = loadFile(filename);
        vector<Record> active;
        
        for (const auto& rec : records) {
            if (!rec.deleted) {
                active.push_back(rec);
            }
        }
        
        saveFile(filename, active);
    }
    
    bool shouldCompact(const string& filename) {
        vector<Record> records = loadFile(filename);
        if (records.size() < 100) return false;
        
        int deleted_count = 0;
        for (const auto& rec : records) {
            if (rec.deleted) deleted_count++;
        }
        
        return deleted_count > records.size() / 3;
    }
    
public:
    void insert(const string& key, int value) {
        string filename = getFileName(key);
        vector<Record> records = loadFile(filename);
        
        // Check if already exists
        for (const auto& rec : records) {
            if (!rec.deleted && strcmp(rec.key, key.c_str()) == 0 && rec.value == value) {
                return; // Already exists
            }
        }
        
        // Append new record
        appendRecord(filename, Record(key, value, false));
        
        // Compact if needed
        if (shouldCompact(filename)) {
            compactFile(filename);
        }
    }
    
    void remove(const string& key, int value) {
        string filename = getFileName(key);
        vector<Record> records = loadFile(filename);
        
        bool found = false;
        for (auto& rec : records) {
            if (!rec.deleted && strcmp(rec.key, key.c_str()) == 0 && rec.value == value) {
                rec.deleted = true;
                found = true;
                break;
            }
        }
        
        if (found) {
            saveFile(filename, records);
        }
    }
    
    void findKey(const string& key) {
        string filename = getFileName(key);
        vector<Record> records = loadFile(filename);
        vector<int> values;
        
        for (const auto& rec : records) {
            if (!rec.deleted && strcmp(rec.key, key.c_str()) == 0) {
                values.push_back(rec.value);
            }
        }
        
        if (values.empty()) {
            cout << "null" << endl;
        } else {
            sort(values.begin(), values.end());
            for (size_t i = 0; i < values.size(); i++) {
                if (i > 0) cout << " ";
                cout << values[i];
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
