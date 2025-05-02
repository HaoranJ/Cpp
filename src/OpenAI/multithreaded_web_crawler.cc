#include <deque>
#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <vector>
using namespace std;

class HtmlParser {
public:
  // blocking HTTP call
  vector<string> getUrls(string url);
};

string GetHostName(const string &url) {
  auto start = url.find("://") + 3;
  if (start == std::string::npos) {
    start = 0;
  }
  // Any character of "/?:" as spliter.
  auto end = url.find_first_of("/?:", start);
  // Use "/?:" as the spliter.
  // auto end = url.find("/?:", start);
  return url.substr(start, end - start);
}

bool SameHostName(const string &url, const string &hn) {
  return GetHostName(url) == hn;
}

class Crawler {
public:
  vector<string> crawl(string startUrl, HtmlParser htmlParser) {
    string hostname = GetHostName(startUrl);
    vector<string> ret;
    std::mutex mtx;
    unordered_set<string> visited;
    // BFS
    deque<string> que = {startUrl};
    visited.insert(startUrl);
    ret.push_back(startUrl);
    while (que.size() > 0) {
      size_t level_size = que.size();
      vector<thread> threads;
      for (size_t i = 0; i < level_size; ++i) {
        string url = que.front();
        cout << "--- url = " << url << "\n";
        que.pop_front();
        threads.emplace_back([url, &mtx, &ret, &visited, &que, &htmlParser, hostname]() {
          auto linked_urls = htmlParser.getUrls(url);
          for (const auto &u : linked_urls) {
            cout << "child = " << u << "\n";
            // If the page doesn't have the same hostname, we won't crawl.
            if (!SameHostName(u, hostname)) {
              continue;
            }
            if (!visited.count(u)) {
              // std::condition_variable comes with std::unique_lock
              std::lock_guard<std::mutex> lock(mtx); // simple scoped lock
              que.push_back(u);
              visited.insert(u);
              ret.push_back(u);
            }
          }
        });
      }
      // Blocks all the threads
      for (auto& t : threads) { t.join(); }
    }
    return ret;
  }
};

// To execute C++, please define "int main()"
int main() {
  string url = "http://news.yahoo.com";
  cout << GetHostName(url) << "\n";
  cout << SameHostName(url, "news.yahoo.com");
  return 0;
}
