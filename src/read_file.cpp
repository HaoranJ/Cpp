// reading a text file
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

int ReadFile () {
  string line;
  ifstream myfile ("example.txt");
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
      cout << line << '\n';
    }
    myfile.close();
  }

  else cout << "Unable to open file"; 

  string b("1,2,3");
C
  std::istringstream my_stream(b);
  char c;
  int n;
  while (my_stream >> n >> c && c == ',') {
      cout << n << c << endl;
  }

  return 0;
}