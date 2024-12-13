#include <thread>
#include <iostream>
#include <vector>
#include <unistd.h>
using namespace std;

void doSomething(int id) {
    cout << id << "\n";
    sleep(1000);
}

/**
 * Spawns n threads
 */
void spawnThreads(int n)
{
    std::vector<thread> threads(n);
    // spawn n threads:
    for (int i = 0; i < n; i++) {
        threads[i] = thread(doSomething, i + 1);
    }

    for (auto& th : threads) {
        th.join();
    }
}

int main()
{
    spawnThreads(10);
}
