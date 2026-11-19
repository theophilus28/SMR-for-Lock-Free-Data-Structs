#include "harris_michael.hpp"
#include "leaky.hpp"
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

int main(int argc, char** argv) {
    const int nthreads = argc > 1 ? std::atoi(argv[1]) : 4;
    const int ops = argc > 2 ? std::atoi(argv[2]) : 100000;
    if (nthreads < 1 || ops < 1) return 1;
    LeakyReclaimer r;
    HarrisMichaelList list(r);
    r.register_thread(nthreads);
    for (int k = 1; k <= 512; ++k) list.insert(k, nthreads);
    r.unregister_thread(nthreads);
    std::atomic<bool> go{false};
    std::vector<std::thread> workers;
    auto start = std::chrono::steady_clock::now();
    for (int t = 0; t < nthreads; ++t) {
        workers.emplace_back([&, t] {
            r.register_thread(t);
            std::mt19937 rng(12345u + t);
            while (!go.load(std::memory_order_acquire)) std::this_thread::yield();
            for (int i = 0; i < ops; ++i) {
                int key = 1 + static_cast<int>(rng() % 1024);
                int choice = static_cast<int>(rng() % 100);
                if (choice < 90) list.contains(key, t);
                else if (choice < 95) list.insert(key, t);
                else list.remove(key, t);
            }
            r.unregister_thread(t);
        });
    }
    start = std::chrono::steady_clock::now();
    go.store(true, std::memory_order_release);
    for (auto& w : workers) w.join();
    auto end = std::chrono::steady_clock::now();
    double seconds = std::chrono::duration<double>(end - start).count();
    std::cout << "scheme=leaky threads=" << nthreads << " ops=" << nthreads * ops
              << " seconds=" << seconds << " throughput_ops_sec=" << (nthreads * ops / seconds) << '\n';
}
