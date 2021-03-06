#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>
#include <deque>
#include <climits>
#include <type_traits>
#include "ood.h"
#include "algorithm_utils.h"

using namespace std;

int numberOfSteps(int num)
{
  return num ? __builtin_popcount(num) * 2 - 1 + (32 - __builtin_clz(num) - __builtin_popcount(num)) : 0;
}

class Node
{
  public:
  int val;
  int *p;
  Node() {}
  Node(int x) {
    val = x;
    p = new int;
    *p = x;
  }
};

void f(Node n0) {
  std::cout << "n0 address = " << &n0 << " val address = " << &(n0.val) << " p address = " << n0.p << std::endl;
}

int findClosestLeaf(TreeNode *root, int k) {
  unordered_map<TreeNode *, vector<TreeNode*>> graph;
  TreeNode *target = nullptr;
  function<void(TreeNode *, TreeNode *)> buildGraph = [&](TreeNode *cur, TreeNode *parent) {
    if (!cur) { return; }
    if (cur->val == k) {
      target = cur;
    }
    if (parent) {
      graph[cur].emplace_back(parent);
      graph[parent].emplace_back(cur);
    }
    buildGraph(cur->left, cur);
    buildGraph(cur->right, cur);
  };
  buildGraph(root, nullptr);

  deque<TreeNode*> que;
  unordered_set<TreeNode*> visited;
  que.emplace_back(target);
  while (!que.empty()) {
    TreeNode *cur = que.front();
    que.pop_front();
    if (!cur->left && !cur->right) {
      return cur->val;
    }
    visited.emplace(cur);
    for (auto &kid : graph[cur]) {
      if (visited.find(kid) != visited.end()) { continue; }
      que.emplace_back(kid);
    }
  }
  return -1;
}

class BaseClass {
public:
   virtual void disp() {};
};
class DerivedClass: public BaseClass{
public:
   void disp() override {
      cout<<"Function of Child Class";
   }
};
class DDClass : public DerivedClass {
  public:
  void disp() override {
    cout<<"Function of Grand child Class";
  }
};

int main()
{
  // Pointer bacics
  /*
     * DO use the indirection operator (*) to access the data stored at the address in a pointer.
     * DO initialize all pointers either to a valid address or to null (0).
     * Each time you allocate memory using the new keyword, you must check to make sure the pointer is not null.
     * For every time in your program that you call new, there should be a call to delete. It is important to keep track
     * of which pointer owns an area of memory and to ensure that the memory is returned to the free store when you are done with it.
     * To be safe, after you delete a pointer, set it to null (0). This disarms the pointer.
     */
  char str1[] = "Udacity";
  int *ptr;
  ptr = new int;
  *ptr = 5;
  std::cout << *ptr << std::endl; // 5
  std::cout << ptr << std::endl;  // address
  delete ptr;
}