#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <unordered_map>
#include <chrono>

using namespace std;
/*
Design and implement a time-based key-value store in C++. Your store must
support:

1. set(key, value, timestamp). Store the given key→value pair, tagged with the
integer timestamp. Keys and values may contain arbitrary characters (e.g. ., !,
whitespace, even newlines).

2. get(key, timestamp) → string. Return the value associated with key at the
largest stored timestamp ≤ timestamp. If no such entry exists, return the empty
string ("").

3. serialize() → string. Produce a single string encoding your entire in-memory
store. You may not use any external serialization libraries. Your format must be
binary-safe (i.e. it must faithfully round-trip through any characters in
keys/values).

4. deserialize(data). Reconstruct the entire store from a string previously
produced by serialize().
*/
class TimeMap {
public:
  TimeMap() {}

  void set(string key, string val, int ts) {
    // check input validation
    // ensure ts is strictly increasing
    auto it = store_.find(key);
    if (it == store_.end()) {
      store_.insert({key, {{ts, val}}});
    } else {
      // it->second.rbegin(): reverse iterator pointing the last element.
      // assert(ts > it->second.rbegin()->first);
      if (ts <= it->second.rbegin()->first) {
        throw std::runtime_error("Timestamp must be strictly increasing.");
      }

      store_[key].insert({ts, val});
    }
  }

  string get(string key, int ts) {
    if (!store_.count(key)) {
      return "";
    }
    auto itr = store_[key].upper_bound(ts);
    if (itr == store_[key].begin()) {
      return "";
    }
    --itr;
    return itr->second;
  }

  // binary-safe serialization via length prefixes.
  string Serialize() {
    cout << "--- Serialize ---\n";
    std::ostringstream oss;
    // key size
    oss << store_.size() << ":";

    for (auto &[key, val_list] : store_) {
      oss << key.size() << ":" << key;
      // # of data points
      oss << val_list.size() << ":";
      for (auto &[ts, val] : val_list) {
        oss << ts << ":" << val.size() << ":" << val;
      }
    }

    return oss.str();
  }

  void Deserialize(const string &blob) {
    cout << "--- Deserialize ---\n";
    store_.clear();
    /*
    3:6:openai2:999:4:foo\nxyz1001:5:bar~~
    */
    std::istringstream iss(blob);
    string str;
    std::getline(iss, str, ':');
    size_t key_count = std::stoull(str);

    for (size_t key_idx = 0; key_idx < key_count; ++key_idx) {
      // parse key
      string token;
      std::getline(iss, token, ':');
      size_t key_len = ToSizeT(token);
      string key;
      key.resize(key_len);
      iss.read(key.data(), key_len);
      store_.insert({key, map<int, string>()});

      // # of values
      string entry_count;
      std::getline(iss, entry_count, ':');
      size_t entry_num = ToSizeT(entry_count);

      // parse values
      for (size_t i = 0; i < entry_num; ++i) {
        // parse "ts:val_size:val"
        string ts_token;
        std::getline(iss, ts_token, ':');
        int ts = std::stoi(ts_token);

        string val_len_tk;
        std::getline(iss, val_len_tk, ':');
        size_t val_len = ToSizeT(val_len_tk);
        string value;
        value.resize(val_len);
        iss.read(value.data(), val_len);
        store_[key].insert({ts, value});
      }
    }
  }

  // binary-safe serialization via length prefixes.
  string serialize_escape() {
    cout << "--- Serialize ---\n";
    std::ostringstream oss;
    // key size
    oss << store_.size() << "\n";

    for (auto &[key, val_list] : store_) {
      oss << key.size() << ":" << EscapeString(key) << "\n";
      // # of data points
      oss << val_list.size() << "\n";
      for (auto &[ts, val] : val_list) {
        oss << ts << ":" << val.size() << ":" << EscapeString(val) << "\n";
      }
    }

    return oss.str();
  }

  void deserialize_unescape(const string &blob) {
    cout << "--- Deserialize ---\n";
    store_.clear();
    /*
    3
    6:openai
    2
    999:4:foo!
    1001:5:bar~~
    */
    std::istringstream iss(blob);
    string str;
    std::getline(iss, str);
    size_t key_count = std::stoull(str);

    for (size_t key_idx = 0; key_idx < key_count; ++key_idx) {
      // parse key
      string line;
      std::getline(iss, line);
      size_t colon_idx = line.find(":");
      if (colon_idx == std::string::npos) {
        throw std::runtime_error("Bad key header");
      }
      size_t key_size = ToSizeT(line.substr(0, colon_idx + 1));
      if (key_size <= 0) {
        throw std::runtime_error("Invalid key size");
      }
      string key = UnescapeString(line.substr(colon_idx + 1));
      if (key_size != key.length()) {
        throw std::runtime_error("Key length mismatch");
      }
      store_.insert({key, map<int, string>()});

      // # of values
      string value_count_str;
      std::getline(iss, value_count_str);
      size_t value_count = ToSizeT(value_count_str);
      if (value_count <= 0) {
        throw std::runtime_error("Invalid value count");
      }

      // parse values
      for (size_t i = 0; i < value_count; ++i) {
        // parse "ts:val_size:val"
        string v_line;
        std::getline(iss, v_line);
        colon_idx = v_line.find(":");
        size_t ts = ToSizeT(v_line.substr(0, colon_idx + 1));
        size_t second_colon_idx = v_line.find(":", colon_idx + 1);
        if (second_colon_idx == std::string::npos) {
          throw std::runtime_error("Bad value format");
        }
        size_t val_size = ToSizeT(
            v_line.substr(colon_idx + 1, second_colon_idx - colon_idx - 1));
        string val = UnescapeString(v_line.substr(second_colon_idx + 1));
        if (val_size != val.length()) {
          throw std::runtime_error("Value length mismatch");
        }
        store_[key].insert({ts, val});
      }
    }
  }

  void deserialize_my_read_token(const std::string &data) {
    cout << "--- Deserialize ---\n";
    store_.clear();
    size_t pos = 0, n = data.size();

    auto read_token = [&](void) -> std::string {
      auto colon = data.find(':', pos);
      if (colon == std::string::npos)
        throw std::runtime_error("Invalid format");
      std::string tok = data.substr(pos, colon - pos);
      pos = colon + 1;
      return tok;
    };

    // number of keys
    size_t numKeys = std::stoull(read_token());
    for (size_t ki = 0; ki < numKeys; ++ki) {
      // key
      size_t keyLen = std::stoull(read_token());
      if (pos + keyLen > n)
        throw std::runtime_error("Invalid key length");
      std::string key = data.substr(pos, keyLen);
      pos += keyLen;

      // entries count
      size_t numEnt = std::stoull(read_token());
      auto &map = store_[key];
      map.clear();

      // each entry
      for (size_t ei = 0; ei < numEnt; ++ei) {
        long long ts = std::stoll(read_token());
        size_t valLen = std::stoull(read_token());
        if (pos + valLen > n)
          throw std::runtime_error("Invalid value length");
        std::string val = data.substr(pos, valLen);
        pos += valLen;
        map.insert({ts, val});
      }
    }
  }

private:
  unordered_map<string, map<int, string>> store_;

  size_t ToSizeT(const string &str) {
    size_t ret = -1;
    try {
      ret = std::stoull(str);
    } catch (...) {
    }
    return ret;
  }

  // Helper function to escape a single string
  std::string EscapeString(const std::string &str) {
    std::ostringstream oss;
    for (char c : str) {
      // Convert each character to its hex representation
      oss << std::setw(2) << std::setfill('0') << std::hex
          << static_cast<unsigned int>(static_cast<unsigned char>(c));
    }
    return oss.str();
  }

  // Helper function to unescape a single string
  std::string UnescapeString(const std::string &escaped) {
    std::string result;
    for (size_t i = 0; i < escaped.length(); i += 2) {
      if (i + 1 >= escaped.length())
        break; // Safety check

      std::string hexPair = escaped.substr(i, 2);
      try {
        char c = static_cast<char>(std::stoi(hexPair, nullptr, 16));
        result += c;
      } catch (const std::exception &e) {
        // Handle invalid hex
        std::cerr << "Error parsing hex: " << hexPair << std::endl;
      }
    }
    return result;
  }
};

int64_t GenerateRandomTimestamps(int64_t max_ts) {
  std::random_device rd;
  std::mt19937_64 gen(rd());
  // std::time_t now = std::chrono::system_clock::to_time_t(
  //     std::chrono::system_clock::now());

  std::uniform_int_distribution<std::time_t> dist(0, max_ts);
  return dist(gen);
}

// To execute C++, please define "int main()"
int main() {
  TimeMap timeMap;
  timeMap.set("CPU", "80%", 1);
  timeMap.set("CPU", "70%", 99);
  timeMap.set("CPU", ":::::", 105);
  // timeMap.set("CPU", "99%", 96); // ERROR: ts is decreasing
  timeMap.set("Build", "ab:hhhh!", 100);
  timeMap.set("Build", "cd~", 200);
  timeMap.set("Build", "yz\nnew line", 400);
  timeMap.set("Build", "jk\tafter tab", 800);
  cout << "---- Expected: ''\n";
  cout << timeMap.get("Build", 0) << "\n";
  cout << "---- Expected: 'ab:hhhh!'\n";
  cout << timeMap.get("Build", 199) << "\n";
  cout << "---- Expected: 'yz\nnew line'\n";
  cout << timeMap.get("Build", 499) << "\n";
  cout << "---- Expected: 'jk\tafter tab'\n";
  cout << timeMap.get("Build", 99999) << "\n";

  string blob = timeMap.Serialize();
  // 1) Write to a file
  {
    std::ofstream ofs("data.txt"); // open for writing (truncates)
    if (!ofs) {
      std::cerr << "Open failed\n";
      return 1;
    }
    ofs << blob;
    // ofs.close();  // optional, destructor will close
  }
  cout << blob << "\n";

  // Read from the file.
  {
    std::ifstream ifs("data.txt", std::ios::binary);
    std::istreambuf_iterator<char> it(ifs);
    std::istreambuf_iterator<char> end;
    std::string blob(it, end);
    timeMap.Deserialize(blob);
  }

  cout << "---- Expected: ''\n";
  cout << timeMap.get("CPU", -5) << std::endl;
  cout << "---- Expected: '80%'\n";
  cout << timeMap.get("CPU", 5) << std::endl;
  cout << "---- Expected: ':::::'\n";
  cout << timeMap.get("CPU", 107) << std::endl;
  cout << "---- Expected: ''\n";
  cout << timeMap.get("Build", 0) << "\n";
  cout << "---- Expected: 'ab:hhhh!'\n";
  cout << timeMap.get("Build", 199) << "\n";
  cout << "---- Expected: 'yz\nnew line'\n";
  cout << timeMap.get("Build", 400) << "\n";
  cout << "---- Expected: 'jk\tafter tab'\n";
  cout << timeMap.get("Build", 99999) << "\n";

  // randomly generate timestamps
  for (int i = 0; i < 5; ++i) {
    int ts = GenerateRandomTimestamps(1000);
    cout << "ts = " << ts << "\n";
    try {
      std::ostringstream oss;
      oss << i << "GB";
      timeMap.set("Memory", oss.str(), ts);
    } catch (...) {
      cout << "Set failed." << "\n";
    }

    cout << "Get: " << timeMap.get("Memory", ts) << "\n";
  }
  return 0;
}
