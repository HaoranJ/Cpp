#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>
using namespace std;

/**
 * Interview Question: Resumable Iterators (45-minute implementation)
 *
 * This task is focused on test-driven development and abstraction design.
 * The goal is to implement an iterator interface that supports saving and restoring its position via a `State` object.
 *
 * Interface Requirements:
 *   - An abstract base class `ResumableIterator<T>` should be defined with two core methods:
 *
 *     virtual State GetState() const = 0;
 *     virtual void SetState(const State& state) = 0;
 *
 *   - The `State` class (or struct) must encapsulate all information necessary to resume iteration from the saved position.
 *
 * ----------------------------------------------------------------------
 *
 * Part 1: Define the Interface and Unit Tests
 *   - Design the abstract base class `ResumableIterator<T>` and the `State` type.
 *   - Write unit tests in `main()` to validate:
 *       * Saving state after each call to `next()`.
 *       * Restoring iterator to previous state using `SetState()`.
 *       * Resuming iteration from restored state yields correct values.
 *       * Behavior at the end of iteration (e.g., next() throws or returns sentinel).
 *   - The iterator should assume it is iterating over a finite sequence (e.g., `vector<int>`).
 *   - The test code must cover all edge cases including:
 *       * Saving state at beginning, middle, and end.
 *       * Restoring to a stopped/finished iterator.
 *
 * ----------------------------------------------------------------------
 *
 * Part 2: Implement `IntListResumableIterator`
 *   - A concrete subclass of `ResumableIterator<int>` that iterates over a `std::vector<int>`.
 *   - The `State` for this class can contain the current index or an iterator.
 *
 * ----------------------------------------------------------------------
 *
 * Part 3: Implement `ResumableMultiFileIterator`
 *   - This class uses multiple `ResumableIterator<json>` instances to iterate over a list of JSON files.
 *   - It composes or wraps the base `ResumableIterator` abstraction.
 *   - It must skip or handle empty files gracefully.
 *   - When a file is exhausted, it moves to the next available file.
 *
 * ----------------------------------------------------------------------
 *
 * Part 4: Optimization Discussion
 *   - What can be done to improve `ResumableMultiFileIterator`?
 *     Consider:
 *       * Memory usage (e.g., lazy loading files)
 *       * Skipping or compacting state representations
 *       * Thread safety (if needed)
 *       * Partial loading or caching of file contents
 *     No code required, just a brief discussion.
 */


template<typename T>
class ResumableIterator {
public:
  struct State {
    typename vector<T>::iterator itr;
  };
  // Define default virtual destructor if this abstract class is designed to be
  // inherited and used polymorphically. It's to make sure to correctly call the
  // destructor of derived class. 
  virtual ~ResumableIterator() = default;
  virtual T Next() = 0;
  virtual bool HasNext() = 0;
  virtual State GetState() = 0;
  virtual void SetState(const State& state) = 0;
};

class IntListResumableIterator : public  ResumableIterator<int> {
public:
  IntListResumableIterator() = default;
  explicit IntListResumableIterator(const vector<int>& list) : list_(list) {
    st_.itr = list_.begin();
  }

  int Next() override {
    if (st_.itr == list_.end()) {
      throw std::runtime_error("Run out of range.");
    }
    int last_next = *(st_.itr);
    st_.itr++;
    return last_next;
  }

  bool HasNext() override {
    return st_.itr != list_.end();
  }

  State GetState() override {
    return st_;
  }

  void SetState(const State& state) override {
    st_ = state;
  }

  State st_;
  vector<int> list_;
};

bool IsSame(vector<int> actual, vector<int> exp) {
  if (actual.size() != exp.size()) { return false; }
  for (size_t i = 0; i < exp.size(); ++i) {
    if (actual[i] != exp[i]) { return false; }
  }
  return true;
}

// To execute C++, please define "int main()"
int main() {
  vector<int> list = {0,1,2,3,4};
  IntListResumableIterator itr(list);
  vector<IntListResumableIterator::State> states;
  vector<int> actual;
  cout << "\n ========= Iterate through a list =========\n";
  while (itr.HasNext()) {
    actual.push_back(itr.Next());
    states.push_back(itr.GetState());
  }
  cout << IsSame(actual, list) << "\n";
  for (size_t i = 0; i < states.size(); ++i) {
    itr.SetState(states[i]);
    vector<int> exp;
    actual.clear();
    int offset = 1;
    while (itr.HasNext()) {
      actual.push_back(itr.Next());
      exp.push_back(i + offset);
      ++offset;
    }
    cout << IsSame(actual, exp) << "\n";
  }

  cout << "\n ========= Basic resuming =========\n";
  IntListResumableIterator itr1(list);
  cout << itr1.Next() << " [EXPECTED: 0]\n";
  cout << itr1.Next() << " [EXPECTED: 1]\n";
  cout << itr1.Next() << " [EXPECTED: 2]\n";
  auto state = itr1.GetState();
  cout << itr1.Next() << " [EXPECTED: 3]\n";
  cout << itr1.Next() << " [EXPECTED: 4]\n";
  itr1.SetState(state);
  cout << itr1.Next() << " [EXPECTED: 3]\n";
  cout << itr1.Next() << " [EXPECTED: 4]\n";
  auto end_state = itr1.GetState();
  itr1.SetState(state);
  itr1.SetState(end_state);
  cout << "\n ========= Iterator at the end =========\n";
  try {
    cout << itr1.Next() << "\n";
  } catch (std::exception& e) {
    cerr << e.what() << " [EXPECTED: 'Run out of range.' ERROR]\n";
  }

  cout << "\n ========= Empty list =========\n";
  IntListResumableIterator empty_list((vector<int>()));
  cout << empty_list.HasNext() << " [EXPECTED: 0]\n";
  try {
    cout << empty_list.Next() << "\n";
  } catch (std::exception& e) {
    cerr << e.what() << " [EXPECTED: 'Run out of range.' ERROR]\n";
  }
  return 0;
}
