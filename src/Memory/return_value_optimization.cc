#include <iostream>
 
// https://diego.assencio.com/?index=f57f25fd5a187c70fc7f34fcf5374773
// We shouldn't use std::move to return a local variable to avoid
// breaking return value optimization
class X
{
public:
    /* default constructor */
    X() { std::cout << "X::X()\n"; }
 
    /* copy constructor */
    X(const X&) { std::cout << "X::X(const X&)\n"; }
 
    /* move constructor */
    X(X&&) { std::cout << "X::X(X&&)\n"; }
};
 
X good()
{
    std::cout << "good()\n";
 
    X x;
    return x;
}
 
X bad()
{
    std::cout << "bad()\n";
 
    X x;
    return std::move(x);
}
 
int main()
{
    X x1 = good();
    X x2 = bad();
 
    return 0;
    // Output:
    // good()
    // X::X()
    // bad()
    // X::X()
    // X::X(X&&)
}