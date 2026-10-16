#include <iostream>
#include "../src/stackType.h"
using namespace std;

int main() {
    stackType<int> s;

    s.push(10);
    s.push(20);
    s.push(30);

    cout << "Top: " << s.top() << endl;
    cout << "Size: " << s.size() << endl;

    s.pop();

    cout << "Top after pop: " << s.top() << endl;

    stackType<int> copy(s);

    cout << "Copied stack top: " << copy.top() << endl;

    stackType<int> assigned;
    assigned = s;

    cout << "Assigned stack top: " << assigned.top() << endl;

    if (s.top() == 20 && s.size() == 2 &&
        copy.top() == 20 && assigned.top() == 20)
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;

    return 0;
}
