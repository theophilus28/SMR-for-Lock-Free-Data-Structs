#pragma once
#include <atomic>
#include <cstdint>
#include <limits>

struct alignas(8) Node {
    int key;
    std::atomic<std::uintptr_t> next{0};
    explicit Node(int k) : key(k) {}
};

namespace tagged {
constexpr std::uintptr_t MARK = 1;
inline Node* ptr(std::uintptr_t v) { return reinterpret_cast<Node*>(v & ~MARK); }
inline std::uintptr_t raw(Node* p) { return reinterpret_cast<std::uintptr_t>(p); }
inline bool marked(std::uintptr_t v) { return (v & MARK) != 0; }
}
