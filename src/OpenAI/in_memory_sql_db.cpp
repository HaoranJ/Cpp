#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <optional>

/*
In Memory DB 题，一共四个part，支持以下功能
1. 建立table (not necessarily relational DB, so we don't need to create table)，支持add／query row
2. where clause
3. order by
4. inverted index (不需要写code，但是不局限于inverted index, 
最好各种index是怎么构建的都熟悉一下)

第一问要query全表，选几个col返回（projection）第二问支持where，第三问支持order_by，第四问问怎么建index（不用写），整体API大概这样：
```
db = DB()
db.insert("users", {"id": "1", "name": "Ada", "birthday": "1815-12-10"})
...
assert(db.query("users", ["id"], conditions=[("name", "=", "Charles")], order_by=(["birthday"], False)) == [{"id": "3"}, {"id": "2"}])
```

class InMemoryDB:
  def __init__(self):
    self.tables = {}

  def create_table(self, table_name, columns):
    self.tables[table_name] = {"columns": columns, "rows": []}

  def add_row(self, table_name, row):
    if table_name not in self.tables:
      raise Exception("Table {} does not exist".format(table_name))
    table = self.tables[table_name]
    if set(row.keys()) != set(table["columns"]):
      raise Exception("Row keys {} do not match table columns {}".format(row.keys(), table["columns"]))
    table["rows"].append(row)

  def query(self, table_name, where_conditions=None, logic="AND", order_by=None):
    if table_name not in self.tables:
      raise Exception("Table {} does not exist".format(table_name))
    rows = self.tables[table_name]["rows"]

    # 过滤：如果有条件，则对每一行进行条件判断
    if where_conditions:
      def match_condition(row, condition):
        col, op, val = condition
        if col not in row:
          return False
        cell_val = row[col]
        if op == "=":
          return cell_val == val
        elif op == ">":
          return cell_val > val
        elif op == "<":
          return cell_val < val
        elif op == ">=":
          return cell_val >= val
        elif op == "<=":
          return cell_val <= val
        else:
          raise Exception("Unsupported operator: {}".format(op))
      
      if logic.upper() == "AND":
        filtered = [r for r in rows if all(match_condition(r, cond) for cond in where_conditions)]
      elif logic.upper() == "OR":
        filtered = [r for r in rows if any(match_condition(r, cond) for cond in where_conditions)]
      else:
        raise Exception("Unsupported logic operator: {}".format(logic))
    else:
      filtered = rows.copy()

    # 排序支持：如果提供 order_by 参数，则对结果按指定列排序
    if order_by:
      # 为了支持多列排序，从后向前依次调用 sort (排序稳定)
      for col, ord in reversed(order_by):
        filtered.sort(key=lambda r: r[col], reverse=(ord.lower() == "decrease"))
    
    return filtered
总体思路没啥问题，insert也需要自己实现，and 和 or不需要，可以假设所有条件之间都是and
*/

#include <algorithm>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <variant>
#include <vector>

using namespace std;
// using Val = std::variant<string, int>;

// void PrintVal(const Val &v) {
//   std::visit([](auto &&arg) { std::cout << arg; }, v);
// }

// void PrintRows(const vector<vector<Val>> rows) {
//   for (auto &r : rows) {
//     for (auto &cell : r) {
//       PrintVal(cell);
//       std::cout << ",";
//     }
//     std::cout << "\n";
//   }
// }

void PrintRows(const vector<vector<string>> rows) {
  for (auto &r : rows) {
    for (auto &cell : r) {
      std::cout << cell << ",";
    }
    std::cout << "\n";
  }
}

bool is_integer(const string &str) {
  try {
    std::stoi(str);
    return true;
  } catch (...) {
    return false;
  }
}

enum class ColType { STR, INT };

struct Col {
  string name;
  ColType type;
  size_t idx = 0;
};

struct Table {
  Table(const string &n, const vector<Col> &c) : name(n) {
    for (size_t i = 0; i < c.size(); ++i) {
      Col col = c[i];
      col.idx = i;
      cols.insert({col.name, std::move(col)});
    }
  }
  string name;
  unordered_map<string, Col> cols;
  vector<vector<string>> rows;
};

class Database {
public:
  Database() {}

  void CreateTable(const string &name, const vector<Col> &cols) {
    // check input validation
    if (db.count(name)) {
      throw std::runtime_error("Table already exists: " + name);
    }
    Table t(name, cols);
    db.insert({name, std::move(t)});
  }

  bool Insert(const string &name, const vector<string> &row) {
    auto itr = db.find(name);
    if (itr == db.end() || itr->second.cols.size() != row.size()) {
      return false;
    }
    auto &tb = itr->second;
    tb.rows.push_back(row);
    return true;
  }

  vector<vector<string>> Select(string name, vector<string> where_cols,
                                vector<string> operators,
                                vector<string> conditions,
                                vector<string> order_by_cols = {},
                                string logic = "AND") {
    vector<vector<string>> ans;
    auto itr = db.find(name);
    if (itr == db.end()) {
      return ans;
    }
    auto &tb = itr->second;
    // WHERE
    for (auto &r : tb.rows) {
      bool matching = true;
      for (size_t i = 0; i < where_cols.size(); ++i) {
        if (!tb.cols.count(where_cols[i])) {
          throw std::runtime_error("Wrong WHERE Col name.");
        }
        size_t col_idx = tb.cols.at(where_cols[i]).idx;
        string cell = r[col_idx];
        string op = operators[i];
        if (tb.cols.at(where_cols[i]).type == ColType::STR) {
          if (op != "=") {
            // throw exception
          }
          if (cell != conditions[i]) {
            matching = false;
            break;
          }
        } else {
          int num = std::stoi(cell);
          int cond_num = std::stoi(conditions[i]);
          if (op != ">") {
            throw std::runtime_error("Logic operator must be GREATER THAN");
          }
          if (num <= cond_num) {
            matching = false;
            break;
          }
        }
      }
      if (matching) {
        ans.push_back(r);
      }
    }

    // ORDER_BY
    vector<int> order_idx;
    for (auto &col : order_by_cols) {
      if (!tb.cols.count(col)) {
        throw std::runtime_error("No such column.");
      }
      order_idx.push_back(tb.cols.at(col).idx);
    }

    std::sort(ans.begin(), ans.end(), [&](auto &a, auto &b) {
      // Compare "Age" first, if not tied, directly return true for < and false
      // for > If tied, then compare the next column "Name" to break tie.
      for (auto idx : order_idx) {
        if (a[idx] < b[idx]) {
          return true;
        }
        if (a[idx] > b[idx]) {
          return false;
        }
      }
      return false; // equal
    });

    return ans;
  }

  vector<vector<string>> Join(string name0, string name1, string left_col,
                              string right_col) {
    vector<vector<string>> ans;
    auto it0 = db.find(name0);
    auto it1 = db.find(name1);
    if (it0 == db.end() || it1 == db.end()) {
      throw std::runtime_error("Invalid table names");
    }
    auto &tb0 = it0->second, tb1 = it1->second;
    size_t col_idx0 = tb0.cols[left_col].idx,
           col_idx1 = tb1.cols[right_col].idx;

    // Build right table cell value --> row indices hash map
    unordered_map<string, vector<size_t>> hash_idx;
    for (size_t i = 0; i < tb1.rows.size(); ++i) {
      string cell = tb1.rows[i][col_idx1];
      auto itr = hash_idx.find(cell);
      if (itr == hash_idx.end()) {
        hash_idx.insert({cell, {i}});
      } else {
        itr->second.push_back(i);
      }
    }

    for (auto &r0 : tb0.rows) {
      string cell = r0[col_idx0];
      if (!hash_idx.count(cell)) {
        continue;
      }
      for (size_t left_row_idx : hash_idx[cell]) {
        vector<string> joined_row;
        joined_row.insert(joined_row.end(), r0.begin(), r0.end());
        joined_row.insert(joined_row.end(), tb1.rows[left_row_idx].begin(), tb1.rows[left_row_idx].end());
        ans.push_back(std::move(joined_row));
      }
    }
    return ans;
  }

private:
  unordered_map<string, Table> db;
};

// To execute C++, please define "int main()"
int main() {
  Database db;
  db.CreateTable("Customers", {{.name = "Id", .type = ColType::STR},
                               {.name = "Name", .type = ColType::STR},
                               {.name = "Age", .type = ColType::INT}});
  db.Insert("Customers", {"1", "Zack", "30"});
  db.Insert("Customers", {"2", "Alice", "30"});
  db.Insert("Customers", {"100", "Wrong"});
  db.Insert("Customers", {"3", "Bob", "20"});
  db.Insert("Customers", {"4", "Carter", "40"});
  db.Insert("Customers", {"5", "Dave", "50"});
  db.Insert("Customers", {"6", "Eva", "34"});
  db.Insert("Customers", {"7", "Josh", "30"});

  db.CreateTable("Orders", {{.name = "Id", .type = ColType::STR},
                            {.name = "CustomerName", .type = ColType::STR},
                            {.name = "Amount", .type = ColType::INT}});
  db.Insert("Orders", {"1", "X", "3000"});
  db.Insert("Orders", {"2", "Alice", "100"});
  db.Insert("Orders", {"100", "Wrong"});
  db.Insert("Orders", {"3", "Alice", "250"});
  db.Insert("Orders", {"4", "Y", "400"});
  db.Insert("Orders", {"5", "Z", "500"});
  db.Insert("Orders", {"6", "H", "340"});
  db.Insert("Orders", {"7", "K", "1000"});

  cout << "\n";
  cout << "--- SELECT * FROM Customers ---\n";
  PrintRows(db.Select("Customers", {}, {}, {}));

  cout << "\n";
  cout << "--- SELECT * FROM Orders ---\n";
  PrintRows(db.Select("Orders", {}, {}, {}));

  cout << "\n";
  cout << "---SELECT * FROM Customers JOIN Orders On CustomerName ---\n";
  PrintRows(db.Join("Customers", "Orders", "Name", "CustomerName"));

  cout << "\n";
  cout << "--- SELECT * FROM Customers WHERE Name = 'Bob' ---\n";
  PrintRows(db.Select("Customers", {"Name"}, {"="}, {"Bob"}));

  cout << "\n";
  cout << "--- SELECT * FROM Customers WHERE Age > 35 ---\n";
  PrintRows(db.Select("Customers", {"Age"}, {">"}, {"35"}));

  cout << "\n";
  cout << "---SELECT * FROM Customers WHERE Age > 35 AND Name == 'Dave' ---\n";
  PrintRows(
      db.Select("Customers", {"Age", "Name"}, {">", "="}, {"35", "Dave"}));

  cout << "\n";
  cout << "---SELECT * FROM Customers WHERE Age > 25 ORDER BY Name ---\n";
  PrintRows(db.Select("Customers", {"Age"}, {">"}, {"25"}, {"Name"}));

  cout << "\n";
  cout << "---SELECT * FROM Customers WHERE Age > 0 ORDER BY Age, Name ---\n";
  PrintRows(db.Select("Customers", {"Age"}, {">"}, {"0"}, {"Age", "Name"}));

  // Corner cases
  // 1. Create existing table
  // 2. Insert wrong row
  // 3. Wrong WHERE Logic
  // 4. Wrong WHERE Column
  // 5. Wrong ORDER BY Column
  try {
    db.Select("Customers", {"wrong col name"}, {"="}, {"random"});
  } catch (const std::exception &e) {
    cout << "Expected error " << e.what() << "\n";
  }
  return 0;
}
