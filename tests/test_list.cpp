#include "harris_michael.hpp"
#include "leaky.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

int main() {
    LeakyReclaimer r;
    HarrisMichaelList list(r);
    r.register_thread(0);
    assert(list.insert(10, 0));
    assert(!list.insert(10, 0));
    assert(list.contains(10, 0));
    assert(list.remove(10, 0));
    assert(!list.contains(10, 0));
    r.unregister_thread(0);

    constexpr int N = 4, PER = 1000;
    std::vector<std::thread> threads;
    for (int t = 0; t < N; ++t) {
        threads.emplace_back([&, t] {
            r.register_thread(t);
            for (int i = 0; i < PER; ++i) assert(list.insert(100 + t * PER + i, t));
            r.unregister_thread(t);
        });
    }
    for (auto& t : threads) t.join();
    r.register_thread(0);
    for (int i = 100; i < 100 + N * PER; ++i) assert(list.contains(i, 0));
    r.unregister_thread(0);
    std::cout << "All starter tests passed\n";
}
