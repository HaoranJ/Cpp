#include <iostream>
#include <vector>
#include <string>
#include <variant>
#include <unordered_map>
#include <stdexcept>
#include <sstream>

using namespace std;

// Problem: Type System Modeling with Generics and Structural Matching
//
// Implement two classes: Node and Function.
//
// class Node:
// - Represents a type, which can be:
//   - A primitive type (e.g., "int", "str")
//   - A composite type (e.g., tuple: ("int", "str"))
// - Can be constructed from:
//   - A single string value
//   - A list of Node objects (possibly nested)
// - Method:
//   - ToString(): Returns the string representation of the Node
//     - Example: Node("int") => "int"
//     - Example: Node({"int", "str"}) => "(int, str)"
//
// class Function:
// - Represents a function signature with input parameters and return type
// - Constructor takes:
//   - A vector of input Node types
//   - A single output Node type
// - Method:
//   - ToString(): Returns string representation of the function type
//     - Example: Function(["str", "int"], ["int", "str"]) => "((str, int)) -> (int, str)"
//
// Implement:
// Node GetReturnType(const Function& func, const std::vector<Node>& invoke_args):
// - Computes the return type of invoking a Function with given argument Nodes
// - Validates that the invoke_args structurally match the function's input types
// - Supports generic type variables like T, U
//   - Example: Function(["T"], ["T", "int"]), called with ["str"]
//     => Return: ["str", "int"]
// - Matching is recursive:
//   - Generic variables can match to primitive or composite types
//   - Generics must match consistently throughout the type signature
// - If argument types do not match input types, throw an exception
//
// Notes:
// - Node can be deeply nested
// - The type system supports structural typing with generic substitution
class Node {
public:
  // Node() = default;
  Node (const string& str) : value_(str) {}
  Node (const vector<Node>& list) : value_(list) {}

  string ToString() {
    ostringstream oss;
    // base case
    if (std::holds_alternative<string>(value_)) {
      oss << std::get<string>(value_);
    } else {
      oss << "(";
      auto node_list = std::get<vector<Node>>(value_);
      for (size_t i = 0; i < node_list.size(); ++i) {
        Node n = node_list[i];
        oss << n.ToString();
        if (i != node_list.size() - 1) {
          oss << ",";
        }
      }
      oss << ")";
    }
    return oss.str();
  }

  vector<Node>& AsList() {
    return std::get<vector<Node>>(value_);
  }

  bool IsBase() {
    return std::holds_alternative<string>(value_);
  }

  bool IsGeneric() {
    return IsBase() 
      && std::get<string>(value_) != "int" 
      && std::get<string>(value_) != "str" 
      && std::get<string>(value_) != "bool" 
      && std::get<string>(value_) != "double"
      && std::get<string>(value_) != "float"
      && std::get<string>(value_) != "void";
  }

  // 2nd const -> can't modify *this
  bool operator==(const Node& other) const {
    return value_ == other.value_;
  }

  std::variant<string, vector<Node>> value_;
};

class Function {
public:
  Function(const vector<Node>& args, const Node& ret) : args_(args), ret_(ret) {}

  string ToString() {
    Node input(args_);
    return input.ToString() + "->" + ret_.ToString();
  }

  // omit the getter functions.
  vector<Node> args_;
  Node ret_;
};

/*
template<typename K, typename V>
void PrintPair(K a, V b) {
  cout << a << ": " << b;
}

int main() {
  // partial template specialization is NOT allowed
  // PrintPair("Alice", V);
  PrintPair("Alice", 10);
}

Variadic Templates
template<typename... Args>
void PrintTuple(Args... args) {
  (std::cout << ... << args) << std::endl;
}

int main() {
  PrintTuple("Alice", 10, 8.8);
}
*/
bool IsMatch(Node& nf, Node& na, unordered_map<string, Node>& generic_map) {
  // cout << "Compare " << nf.ToString() << " and " << na.ToString() << "\n";
  if (nf.IsGeneric()) {
    auto itr = generic_map.find(nf.ToString());
    if (itr == generic_map.end()) {
      // T can be a nested Node too. 
      generic_map.insert({nf.ToString(), na});
      return true;
    }
    return itr->second == na;
  } 
  if (nf.IsBase()) {
    return nf == na;
  }
  // nf is a list. We need to go recursively to extract generic types instead of
  // just comparing using == operator
  if (!na.IsBase() && nf.AsList().size() == na.AsList().size()) {
    cout << "Deeply nested\n";
    for (size_t i = 0; i < nf.AsList().size(); ++i) {
      if (!IsMatch(nf.AsList()[i], na.AsList()[i], generic_map)) {
        return false;
      }
    }
    return true;
  }
  return false;
}

Node ReplaceGeneric(Node& ret, unordered_map<string, Node>& generic_map) {
  if (ret.IsBase()) {
    if (ret.IsGeneric()) {
      return generic_map.at(ret.ToString());
    } else {
      return ret;
    }
  }
  vector<Node> new_list;
  auto& node_list = std::get<vector<Node>>(ret.value_);
  for (size_t i = 0; i < node_list.size(); ++i) {
    new_list.push_back(ReplaceGeneric(node_list[i], generic_map));
  }
  return Node(new_list);
}

Node GetReturnType(Function& func, vector<Node>& args) {
  // check if args matches func input
  if (func.args_.size() != args.size()) { 
    return Node("invalid"); 
  }
  unordered_map<string, Node> generic_map;
  // Avoid generic_map[key] which inserts a Node if the mapping doesn't exists,
  // so it needs default constructor of Node.
  // return generic_map[key];
  for (size_t i = 0; i < args.size(); ++i) {
    Node& nf = func.args_[i];
    Node& na = args[i];
    if (!IsMatch(nf, na, generic_map)) {
      return Node("invalid");
    }
  }
  return ReplaceGeneric(func.ret_, generic_map);
}

template<typename T>
T Get(T t) {
  cout << "return: " << t << "\n";
  return t;
}

int main() {
  // Get(444);
  Node n_int("int"), n_str("str"), n_bool("bool"), T("T"), U("U");
  cout << n_int.ToString() << "[EXPECTED: int]\n";
  Node nested_node0({n_int, {{U, n_str}}});
  cout << nested_node0.ToString() << "[EXPECTED: (int, (U, str))]\n";
  Node nested_node({T, nested_node0});
  cout << nested_node.ToString() << "[EXPECTED: (T, (int, (U, str)))]\n";

  cout << "\n=== Basic match ===\n";
  vector<Node> args = {n_str, n_int};
  Function f0(args, n_bool);
  cout << f0.ToString() << "\n";
  cout << "ReturnType = " << GetReturnType(f0, args).ToString() << " (expected: bool) \n";
  vector<Node> wrong_args = {n_int, n_int};
  cout << "ReturnType = " << GetReturnType(f0, wrong_args).ToString() << " (expected: invalid) \n";

  cout << "\n=== function input is base, args is nested ===\n";
  vector<Node> input = {n_int};
  Function f3(input, n_bool);
  Node nested0({n_str, {{n_str, n_str}}});
  vector<Node> arg_list = {nested0};
  cout << "ReturnType = " << GetReturnType(f3, arg_list).ToString() << " [expected: invalid] \n";

  cout << "\n=== function input is nested, args is base ===\n";
  Function f4(arg_list, n_bool);
  vector<Node> base_list = {n_int};
  cout << "ReturnType = " << GetReturnType(f4, base_list).ToString() << " [expected: invalid] \n";

  cout << "\n=== Empty args ===\n";
  vector<Node> empty_args;
  cout << "ReturnType = " << GetReturnType(f0, empty_args).ToString() << " (expected: invalid) \n";

  cout << "\n=== Function input and args are both empty ===\n";
  Function f1(empty_args, n_bool);
  cout << "ReturnType = " << GetReturnType(f1, empty_args).ToString() << " (expected: bool) \n";

  cout << "\n=== Different param ordering between input and args ===\n";
  vector<Node> diff_order_args = {n_int, n_str};
  Function f2(args, n_bool);
  cout << "ReturnType = " << GetReturnType(f2, diff_order_args).ToString() << " (expected: invalid) \n";

  cout << "\n=== Generic: basic ===\n";
  vector<Node> g_args = {T, U};
  Function g(g_args, T);
  cout << "ReturnType = " << GetReturnType(g, args).ToString() << " (expected: str) \n";

  cout << "\n=== Multiple instances of the same generic ===\n";
  Function gm({T, T}, Node({T, T}));
  cout << gm.ToString() << "[EXPECTED: (T, T)->(T,T)]\n";
  vector<Node> args_m = {Node("int"), Node("int")};
  cout << "ReturnType = " << GetReturnType(gm, args_m).ToString() << " [EXPECTED: (int, int)] \n";
  
  cout << "\n";
  cout << "=== Generic: generic type mapped to a nested Node ===\n";
  vector<Node> nested_args = {nested0, n_int};
  Function g1(g_args, T);
  cout << "ReturnType = " << GetReturnType(g1, nested_args).ToString() << " (expected: (str, (str, str))) \n";
  
  cout << "\n";
  cout << "=== Generic: function output is a nested Node with generic ===\n";
  vector<Node> g_args2 = {T, U, n_int};
  Node nested_out({n_str, {{U, {{T, n_int}}}}});
  Function g2(g_args2, nested_out);
  vector<Node> my_args2 = {n_bool, n_str, n_int};
  cout << "ReturnType = " << GetReturnType(g2, my_args2).ToString() << " [expected: (str,(str,(bool,int)))] \n";

  cout << "\n";
  cout << "=== Generic: deeply nested ===\n";
  vector<Node> g_args3 = {{g_args2}, n_str};
  Function g3(g_args3, U);
  vector<Node> my_args3 = {{my_args2}, n_str};
  cout << "ReturnType = " << GetReturnType(g3, my_args3).ToString() << " [expected: str]\n";

  // Variadic Arguments (Partial Matching)
  std::cout << "\n=== Variadic Arguments with Partial Matching ===\n";
  Function f6({T, Node({T, n_str})}, T);
  Node n_float("float");
  vector<Node> args6 = {n_float, Node({n_float, n_str})};
  Node ret4 = GetReturnType(f6, args6);
  std::cout << ret4.ToString() << " (expected: float)\n";

  // Deeply Nested Generic Matching
  std::cout << "\n=== Deeply Nested Generic Matching ===\n";
  Node V("V");
  Function f7({T, Node({U, V})}, Node({U, V}));
  cout << f7.ToString() << "\n";
  vector<Node> args7 = {n_int, Node({n_str, n_float})};
  Node ret5 = GetReturnType(f7, args7);
  std::cout << ret5.ToString() << " (expected: (str, float))\n";

  return 0;
}
