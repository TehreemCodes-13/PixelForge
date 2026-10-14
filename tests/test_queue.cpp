
#include <iostream>
#include "../src/queueType.h"
using namespace std;

int main() {
    queueType<int> q(3);

    q.addQueue(10);
    q.addQueue(20);
    q.addQueue(30);

    cout << "Front: " << q.front() << endl;
    cout << "Back: " << q.back() << endl;
    cout << "Size: " << q.size() << endl;

    q.deleteQueue();
    q.addQueue(40);

    cout << "Front after wraparound: " << q.front() << endl;
    cout << "Back after wraparound: " << q.back() << endl;

    q.deleteQueue();
    cout << "New front: " << q.front() << endl;

    cout << (q.size() == 2 ? "PASS" : "FAIL") << endl;
    return 0;
}
