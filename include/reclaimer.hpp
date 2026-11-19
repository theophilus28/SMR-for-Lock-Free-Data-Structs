#pragma once
#include <cstddef>
#include <cstdint>
#include <atomic>

struct Node;

// Thread contexts will later hold EBR epoch state or hazard-pointer slots.
class Reclaimer {
public:
    virtual ~Reclaimer() = default;
    virtual void register_thread(std::size_t thread_id) = 0;
    virtual void unregister_thread(std::size_t thread_id) = 0;
    virtual void enter(std::size_t thread_id) = 0;
    virtual void exit(std::size_t thread_id) = 0;
    // Protect and validate a tagged link; caller must validate traversal adjacency.
    virtual std::uintptr_t protect(std::size_t thread_id, std::size_t slot,
                                   const std::atomic<std::uintptr_t>& link) = 0;
    virtual void clear(std::size_t thread_id, std::size_t slot) = 0;
    virtual void retire(std::size_t thread_id, Node* node) = 0;
};
