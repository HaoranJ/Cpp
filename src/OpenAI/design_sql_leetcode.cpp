#include <iostream>
#include <sys/types.h>
#include <unordered_map>
#include <vector>
using namespace std;

/* 
Index: 
  1. B Tree: search, insert, delete in O(logN) time. 
     - Self balanced tree, all the leaves appear at the same level.
     - Optimized for disk access.
     - Use blocks to speed up insertions and deletions.
     - Range queries are efficient because It stores keys in sorted order within nodes. 
  2. Hash Indexing: suited for equality comparison (=)
  3. Generalized Inverted Index: full-text search.
 
Which approach would you choose if the table might become sparse due to many deletions, and why? 
Consider the impact on memory usage and performance.
*/
class SQL {
public:
  SQL(vector<string>& names, vector<int>& columns) {
    for (size_t i = 0; i < names.size(); ++i) {
      string n = names[i];
      ids.insert({n, 1});
      col_nums.insert({n, columns[i]});
      tables.insert({n, unordered_map<int, vector<string>>()});
    }
  }

  bool ins(string name, vector<string> row) {
    if (ids.find(name) == ids.end() || col_nums[name] != row.size()) {
      return false;
    }
    int id_to_insert = ids[name];
    ids[name]++;
    auto& table = tables[name];
    table.insert({id_to_insert, row});
    return true;
  }

  bool rmv(string name, int row_id) {
    if (ids.find(name) == ids.end()) {
      return false;
    }
    auto& table = tables[name];
    if (!table.count(row_id)) {
      return false;
    }
    table.erase(row_id);
    return true;
  }

  string sel(string table_name, int row_id, int col_id) {
    if (!ids.count(table_name)
      || row_id >= ids[table_name]
      || col_id < 0
      || static_cast<size_t>(col_id) > col_nums[table_name]
      || !tables[table_name].count(row_id)) {
      return "<null>";
    }
    return tables[table_name][row_id][col_id-1];
  }

  vector<string> exp(string table_name) {
    vector<string> csv;
    if (ids.find(table_name) == ids.end()) {
      return csv;
    }
    for (const auto& row : tables[table_name]) {
      string entry = to_string(row.first) + ",";
      for (size_t i = 0; i < row.second.size(); ++i) {
        string val = row.second[i];
        entry += val;
        if (i < row.second.size() - 1) {
          entry += ",";
        }
      }
      csv.push_back(entry);
    }
    return csv;
  }

private:
  unordered_map<string, unordered_map<int, vector<string>>> tables;
  unordered_map<string, int> ids;
  unordered_map<string, size_t> col_nums;
};

// To execute C++, please define "int main()"
int main() {
  auto words = { "Hello, ", "World!", "\n" };
  for (const char* const& word : words) {
    cout << word;
  }

  vector<string> names = {"one","two","three"};
  vector<int> columns = {2,3,1};
  SQL sql(names, columns);
  sql.ins("two",{"first","second","third"}); // true
  cout << sql.sel("two", 1, 3) << std::endl;
  cout << sql.rmv("two", 1) << std::endl;
  cout << sql.sel("two", 1, 2) << std::endl;
  sql.ins("two",{"fourth","fifth"});
  sql.ins("two",{"fourth","fifth","sixth"});
  auto res = sql.exp("two");
  for (string str : res) {
    cout << str << std::endl;
  }
  return 0;
}

/*
struct Table {
  // Define default constructor because `tables[name].col_size` will insert a default Table if 
  // `tables[name]` does not exist. 
  Table() : name(""), next_row_id(1), col_size(0) {}
  Table(string name1, int col_size1) : name(name1), col_size(col_size1) {
    next_row_id = 1;
  }

  string name;
  int next_row_id;
  int col_size;
  unordered_map<int, vector<string>> entries;  
};

class SQL {
public:
  SQL(vector<string>& names, vector<int>& columns) {
    for (size_t i = 0; i < names.size(); ++i) {
      string n = names[i];
      Table t(n, columns[i]);
      tables.insert({n, t});
    }
  }

  bool ins(string name, vector<string> row) {
    if (!tables.count(name) || static_cast<size_t>(tables[name].col_size) != row.size()) {
      return false;
    }
    int id_to_insert = tables[name].next_row_id;
    ++tables[name].next_row_id;
    auto& table = tables[name];
    table.entries.insert({id_to_insert, row});
    return true;
  }

  bool rmv(string name, int row_id) {
    if (tables.find(name) == tables.end()) {
      return false;
    }
    auto& table = tables[name];
    if (!table.entries.count(row_id)) {
      return false;
    }
    table.entries.erase(row_id);
    return true;
  }

  string sel(string table_name, int row_id, int col_id) {
    if (!tables.count(table_name)
      || row_id >= tables[table_name].next_row_id
      || col_id < 0
      || col_id > tables[table_name].col_size
      || !tables[table_name].entries.count(row_id)) {
      return "<null>";
    }
    return tables[table_name].entries[row_id][col_id-1];
  }

  vector<string> exp(string table_name) {
    vector<string> csv;
    if (tables.find(table_name) == tables.end()) {
      return csv;
    }
    for (const auto& row : tables[table_name].entries) {
      string entry = to_string(row.first) + ",";
      for (size_t i = 0; i < row.second.size(); ++i) {
        string val = row.second[i];
        entry += val;
        if (i < row.second.size() - 1) {
          entry += ",";
        }
      }
      csv.push_back(entry);
    }
    return csv;
  }

private:
  unordered_map<string, Table> tables;
};
*/