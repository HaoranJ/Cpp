#include <chrono>
#include <cstddef>
#include <future>
#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

/**
 * Interview Question:
 *
 * A tree represents a cluster with many nodes, where each node is a machine
 * with a unique ID. Communication only exists between parent and child nodes.
 * Only the root node can receive messages from outside the cluster.
 *
 * Two existing functions are provided:
 * 1. SendMessage(to_node_id, message):
 *    - This is already implemented and allows sending a message to a specific
 * node ID.
 *
 * 2. ReceiveMessage(from_node_id, message):
 *    - Every node executes this function upon receiving a message.
 *
 * ----------------------------------------------------------------------
 *
 * Part 1: Counting Total Nodes in the Cluster
 *    - Implement the Node class and the ReceiveMessage() function.
 *    - Send a request to the root node to trigger the count.
 *    - The root node will execute `ReceiveMessage(null, message)`.
 *    - Each node responds with its count to its parent.
 *    - The final count is aggregated at the root node.
 *
 * ----------------------------------------------------------------------
 *
 * Part 2: Handling Network Failures (Idempotence)
 *    - Ensure that network failures do not cause double counting of nodes.
 *    - Implement idempotency logic so that if a node receives the same message
 * twice, it does not count twice.
 *    - Consider using a message ID or a tracking mechanism to avoid
 * duplication.
 *
 * ----------------------------------------------------------------------
 *
 * Part 3: Retrieving the Topology
 *    - Implement a way to query the entire topology of the cluster.
 *    - The format should be serialized as follows:
 *      "1[2[4,5],3[6,7]]"
 *    - This means:
 *      - Node 1 is the root
 *      - Node 2 and Node 3 are its children
 *      - Node 4 and Node 5 are children of Node 2
 *      - Node 6 and Node 7 are children of Node 3
 *    - The serialization must be efficient and clear.
 */

class Node;
// SendMessage implementation to simulate message passing
string SendMessage(int from, int to_node_id, const string &message,
                   string req_id = "");

// Global registry to store node mappings
unordered_map<int, Node *> node_registry;

class Node {
public:
  Node() = default;
  Node(int id) : id_(id) {
    node_registry.insert({id_, this});
    cached_node_count_ = 0;
    req_counter_ = 0;
  }

  string ReceiveMsg(int from, const string &msg, string req_id = "") {
    // check input validation
    string ret;
    if (msg == "COUNT") {
      if (!req_id.empty() && reqs_done_.count(req_id)) {
        return "IDEMPOTENT";
      }
      if (id_ == 0 && req_id.empty()) {
        // root
        std::lock_guard<std::mutex> lock(mtx_);
        req_counter_++;
        req_id += "COUNT" + std::to_string(req_counter_);
      }

      int sum = 1;
      vector<future<int>> futures;
      for (size_t i = 0; i < kids_.size(); ++i) {
        // Simulate network retry
        // if (k->id_ % 2 == 1) {
        //   string kid_sum = SendMessage(id_, k->id_, "COUNT", req_id);
        //   sum += (kid_sum.empty() || kid_sum == "IDEMPOTENT")
        //              ? 0
        //              : std::stoi(kid_sum);
        // }
        auto& k = kids_[i];
        futures.emplace_back(std::async(std::launch::async, [this, &k, &msg, &req_id]() {
          string res = SendMessage(this->id_, k->id_, msg, req_id);
          return (res.empty() || res == "IDEMPOTENT") ? 0 : std::stoi(res);
        }));

        for (auto& f : futures) {
          if (f.valid()) {
            sum += f.get();
          }
        }
      }
      cached_node_count_ = sum;
      ret += std::to_string(sum);
      if (!req_id.empty()) {
        reqs_done_.insert(req_id);
      }
    } else if (msg == "TOPO") {
      ret += std::to_string(id_);
      if (kids_.size() > 0) {
        ret += "[";
        for (size_t i = 0; i < kids_.size(); ++i) {
          auto kid = kids_[i];
          string kid_topo = SendMessage(id_, kid->id_, "TOPO");
          ret += kid_topo;
          if (i != kids_.size() - 1) {
            ret += ",";
          }
        }
        ret += "]";
      }
    }
    return ret;
  }

  // omit getter and setter functions.
  vector<Node *> kids_;
  int id_;
  int cached_node_count_;
  std::mutex mtx_;
  int req_counter_; // mutex for thread safe.
  unordered_set<string> reqs_done_;
};

string SendMessage(int from, int to_node_id, const string &message,
                   string req_id) {
  // Retry (exponential backoff) if the request fails.
  if (node_registry.find(to_node_id) != node_registry.end()) {
    Node *target_node = node_registry[to_node_id];
    return target_node->ReceiveMsg(from, message, req_id);
  }
  return "";
}

int main() {
  /* Idempotent
     1
   /   \
  2     3
 / \
4   5
  1 send COUNT msg to 2, if it receives no response within the timeout,
  1 retries. Then it's possible for 1 to get two responses from 2, then
  1 shouldn't double count.
  */
  Node root(0);
  cout << "\n COUNT basic \n";
  cout << SendMessage(-1, 0, "COUNT") << " [EXPECTED: 1]\n";
  cout << SendMessage(-1, 0, "TOPO") << " [EXPECTED: 0]\n";
  Node r1(1), r2(2), r3(3), r4(4), r5(5), r6(6), r7(7);
  root.kids_.push_back(&r1);
  root.kids_.push_back(&r2);
  // cout << SendMessage(-1, 0, "COUNT") << " [EXPECTED: 3]\n";
  r1.kids_ = {&r3, &r4};
  r2.kids_ = {&r5, &r6};
  r5.kids_ = {&r7};
  cout << SendMessage(-1, 0, "COUNT") << " [EXPECTED: 8]\n";
  cout << SendMessage(-1, 0, "TOPO") << " [EXPECTED: 0[1[3,4],2[5[7],6]]]\n";
  // cout << "\n TOPO basic \n";
  // cout << "\n Idempotent \n";
  cout << SendMessage(-1, 0, "COUNT") << " [EXPECTED: 8]\n";

  return 0;
}
