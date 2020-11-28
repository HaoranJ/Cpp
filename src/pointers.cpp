#include <iostream>
#include <unordered_map>
#include <vector>
#include <sstream>
using namespace std;

void AssignByValueOrReference() {
  vector<vector<int> > open(3, vector<int>(1));
  vector<vector<int> > valCopy = open;
  vector<vector<int> > *refCopy = &open;
  vector<int> node(2,2);
  vector<int> node3(3,3);
  valCopy.push_back(node);
  refCopy->push_back(node3);
  /*
  References
  & at left is reference
  & at right is address-of 
  References are generally easier and safer than pointers. 
  As a decent rule of thumb, references should be used in place of pointers when possible.

  However, if the other object is not yet available when the first object is created, then the first object will need to use a pointer, not a reference, since a reference cannot be null. The reference could only be initialized once the other object is created.
  */
  int i = 1;
  int& j = i; // j is another reference to i

  /*
  Ref-sign(&) in front of function
  https://stackoverflow.com/questions/23776784/use-of-operator-before-a-function-name-in-c/23777436
  */
}