#include <algorithm> // For std::sort and std::find
#include <bitset>    // For bit manipulation
#include <cassert>   // For assertions
#include <chrono>    // For timing
#include <cmath>     // For mathematical functions
#include <cstdint>
#include <ctime> // For time functions
#include <deque>
#include <exception>
#include <fstream>
#include <functional>       // For std::function like std::greater
#include <initializer_list> // For handling initializer lists
#include <iomanip>
#include <iostream>
#include <iostream> // For std::cout and std::cin
#include <limits>   // For numeric limits
#include <map> // For std::map and multimap that allows duplicate values in sorted order
#include <mutex> // For mutexes
#include <optional>
#include <queue> // For std::priority_queue
#include <random>
#include <set>
#include <shared_mutex> // std::shared_mutex
#include <sstream>
#include <stdexcept>
#include <string> // For std::string
#include <string_view>
#include <sys/types.h>
#include <system_error>
#include <thread> // For multithreading
#include <unordered_map>
#include <unordered_set>
#include <utility> // For std::pair
#include <variant>
#include <vector> // For std::vector
using namespace std;

/**
 * Interview Question:
 *
 * Implement a simplified version of the `cd` (change directory) command.
 *
 * Part 1: Basic Path Navigation
 *    - Function signature: `string cd(const string& current_dir, const string&
 * new_dir);`
 *    - Supports the following path navigations:
 *      1. "." -> Current directory.
 *      2. ".." -> Parent directory.
 *      3. Absolute paths (start with '/') -> Directly navigate to that path.
 *      4. Relative paths -> Navigate relative to `current_dir`.
 *      5. If navigation goes beyond root (e.g., "/.."), return "null".
 *      6. Assume `new_dir` is always valid as long as it's under "/" (root
 * dir).
 *
 * Part 2: Home Directory Navigation
 *    - Adds support for `~` to represent the user's home directory.
 *    - Assume the home directory is `/home/user`.
 *
 * Part 3: Soft Link Support
 *    - Adds support for soft links, represented as an `unordered_map<string,
 * string>`.
 *    - If a path matches a key in the map, it should resolve to the value of
 * the map.
 *    - If there are multiple matches, the longest match is prioritized.
 *      - The longest match is determined by the maximum depth of the path.
 *      - For example:
 *         - If the map contains:
 *            `/foo/bar` -> `/abc` (they are ABSOLUTE paths!)
 *            `/foo/bar/baz` -> `/xyz`
 *         - And you call `cd("/foo/bar", "baz")`, the longest match is:
 *            `/foo/bar/baz` instead of `/foo/bar`, resulting in the final path
 * being `/xyz`.
 *    - If there is a cyclic dependency, throw an exception.
 *
 * Examples:
 *    cd("/home/", ".") -> "/home/"
 *    cd("/home/", "./ada") -> "/home/ada"
 *    cd("/foo/bar", "baz") -> "/foo/bar/baz"
 *    cd("/foo/../", "./baz") -> "/baz"
 *    cd("/", "foo/bar/../../baz") -> "/baz"
 *    cd("/", "..") -> "null"
 *    cd("/foo/bar", "baz", {/foo/bar: /abc}) -> "/abc/baz"
 *    cd("/foo/bar", "baz", {/foo/bar: /abc, /abc: /bcd, /bcd/baz: /xyz}) ->
 * "/xyz" cd("/foo/bar", "baz", {/foo/bar: /abc, /foo/bar/baz: /xyz}) -> "/xyz"
 */

class FileSystem {
public:
  FileSystem() = default;
  // soft link map. Assume key and value of each pair is good path - start with
  // '/' and last dir name ending without '/', like "/foo/bar/openai".
  // `const &` can bind to temporary values (rvalue), non-const & can't (because
  // any modification for `softlinks` map will lost after the function if we
  // pass a temporary rvalue.)
  string cd(const string &pwd, const string &in,
            const unordered_map<string, string> &softlinks = {}) {
    // /a/b/c, ../d/e = /a/b/d/e
    if (in.empty()) {
      return pwd;
    }
    vector<string> out_list;
    vector<string> dirs;
    if (in.at(0) == '/') {
      // start from root
      dirs = ExtractDirs(in.substr(1));
    } else if (in.at(0) == '~') {
      // user home: "~/proj/openai"
      out_list.push_back("home");
      out_list.push_back("user");
      if (in.size() > 2) {
        dirs = ExtractDirs(in.substr(2));
      }
    } else {
      // start with "." or ".." or a dir name like "foo".
      vector<string> pwd_dirs = ExtractDirs(pwd.substr(1)); // skip first "/"
      out_list.insert(out_list.end(), pwd_dirs.begin(), pwd_dirs.end());
      // cout << "in = " << in << "\n";
      dirs = ExtractDirs(in);
    }
    // cout << "out_list size = " << out_list.size() << "\n";
    // cout << "dirs size = " << dirs.size() << "\n";
    for (auto &d : dirs) {
      if (d.empty()) {
        continue;
      }
      if (d == ".") {
        continue;
      }
      if (d == "..") {
        if (out_list.empty()) {
          return "null";
        }
        out_list.pop_back();
      } else {
        out_list.push_back(d);
      }
    }

    string out;
    if (!softlinks.empty()) {
      // {/, foo, bar}
      // !! If /folder is linked with /foo/folder_link, then it's not directed.
      // Instead, these two folders can access each other's child dirs and files.
      // I tried in Unix terminal.
      unordered_set<string> visited;
      out = Recur(out_list, softlinks, visited);
    } else {
      out = CombineDirs(out_list);
    }
    return out;
  }

  // DFS + backtracking
  // time = O(n*k), n is the segment number of initial path (out_list.size()).
  // k is the number of softlinks.
  string Recur(vector<string> &out_list,
               const unordered_map<string, string> &softlinks,
               unordered_set<string> &visited) {
    // cout << "Recur out_list size = " << out_list.size() << "\n";
    bool found_link = false;
    string out;
    //   0,   1  , 2
    //   i         j
    // {foo, bar, xyz}
    // time = O(n)
    for (size_t dir_num = out_list.size(); dir_num > 0; --dir_num) {
      // softlinks only contains absolute paths, so we have to iterate
      // different path lengths from the root.
      size_t lo = 0, hi = lo + dir_num - 1;
      // [inclusive, exclusive)
      vector<string> sub_list(out_list.begin(), out_list.begin() + hi + 1);
      string sub_path = CombineDirs(sub_list);
      // cout << "Recur sub_path = " << sub_path << "\n";
      auto itr = softlinks.find(sub_path);
      // find the link: /foo/bar/xyz -> /openai
      if (itr != softlinks.end()) {
        if (visited.count(itr->second)) {
          throw std::runtime_error("Cyclic Link");
        }
        visited.insert(itr->first);
        found_link = true;
        vector<string> new_list;
        vector<string> linked =
            ExtractDirs(itr->second.substr(1)); // "/xyz/openai"
        new_list.insert(new_list.end(), linked.begin(), linked.end());
        new_list.insert(new_list.end(), out_list.begin() + hi + 1,
                        out_list.end());
        // At most n times Recur (without cyclic links).
        out = Recur(new_list, softlinks, visited);
        visited.erase(itr->first); // erase step
        break;
      }
    }
    if (!found_link) {
      // base case
      out = CombineDirs(out_list);
    }
    // cout << "out = " << out << "\n";
    return out;
  }

  string CombineDirs(vector<string> &out_list) {
    if (out_list.empty()) {
      return "/";
    }
    string out;
    for (size_t i = 0; i < out_list.size(); ++i) {
      string s = out_list[i];
      out += "/";
      out += s;
    }
    return out;
  }

  vector<string> ExtractDirs(const string &in) {
    istringstream iss(in);
    vector<string> ret;
    string token;
    while (std::getline(iss, token, '/')) {
      ret.push_back(token);
    }
    return ret;
  }
};

int main() {
  FileSystem fs;
  cout << "\n====== Basic =======\n";
  cout << fs.cd("/foo/bar", "") << " [EXPECTED: /foo/bar]\n";
  cout << fs.cd("/foo/bar", "../xyz") << " [EXPECTED: /foo/xyz]\n";
  cout << fs.cd("/foo/bar", "./xyz") << " [EXPECTED: /foo/bar/xyz]\n";
  cout << fs.cd("/foo/bar", "/xyz/../../proj") << " [EXPECTED: null]\n";
  cout << fs.cd("/foo/bar", "../../../xyz") << " [EXPECTED: null]\n";
  cout << fs.cd("/foo/bar", "/xyz/proj") << " [EXPECTED: /xyz/proj]\n";
  cout << fs.cd("/foo/bar", "../") << " [EXPECTED: /foo]\n";
  cout << fs.cd("/foo/bar", ".") << " [EXPECTED: /foo/bar]\n";
  cout << fs.cd("/foo/bar", "../xyz/././..") << " [EXPECTED: /foo]\n";
  cout << fs.cd("/foo/bar", "/") << " [EXPECTED: /]\n";
  cout << fs.cd("/foo/bar", "../xyz////proj") << " [EXPECTED: /foo/xyz/proj]\n";

  cout << "\n====== User home =======\n";
  cout << fs.cd("/foo/bar", "~") << " [EXPECTED: /home/user]\n";
  cout << fs.cd("/foo/bar", "~/xyz") << " [EXPECTED: /home/user/xyz]\n";
  cout << fs.cd("/foo/bar", "~/proj/openai/./..///")
       << " [EXPECTED: /home/user/proj]\n";

  cout << "\n====== Soft links =======\n";
  unordered_map<string, string> empty_map;
  unordered_map<string, string> map = {{"/foo/bar/xyz", "/openai"}};
  cout << fs.cd("/foo/bar", "./xyz", {{"/foo/bar/xyz", "/openai"}})
       << " [EXPECTED: /openai]\n";
  map = {{"/foo/bar/xyz", "/openai/server"},
         {"/foo/bar", "/wrong_dir"},
         {"/openai/server", "/final"}};
  cout << fs.cd("/foo/bar", "./xyz", map) << " [EXPECTED: /final]\n";
  map = {{"/foo/bar/xyz", "/openai/server/node/index"},
         {"/home/foo", "/correct_dir"},
         {"/correct_dir/bar/xyz", "/final"},
         {"/openai/server", "/wrong"}};
  cout << fs.cd("/home/foo/bar", "./xyz", map)
       << " [EXPECTED: /final]\n";

  cout << "\n====== Soft links: detect cyclic deps =======\n";
  try {
    map = {{"/home/foo/bar", "/openai"},
           {"/openai", "/test"},
           {"/test", "/home/foo/bar"},
           {"/openai/server", "/final"}};
    cout << fs.cd("/home/foo/bar", "./xyz", map) << "\n";
  } catch (const std::exception &e) {
    cerr << e.what() << " [EXPECTED: Cyclic Link]\n";
  }

  map = {{"/home/foo", "/openai"},
         {"/home/foo/bar", "/gen"}}; // longest softlink avoids cyclic dep.
  cout << fs.cd("/home/foo/bar", "./xyz", map) << " [EXPECTED: /gen/xyz]\n";

  cout << "\n====== Soft links: time complexity worst case =======\n";
  map = {{"/a", "/b"},
         {"/b", "/c"},
         {"/c", "/d"},
         {"/d", "/e"},
         {"/e", "/z"}}; 
  cout << fs.cd("/a/foo", "./xyz", map) << " [EXPECTED: /z/foo/xyz]\n";

  return 0;
}
