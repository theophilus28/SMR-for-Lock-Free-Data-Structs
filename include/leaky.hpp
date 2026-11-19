#pragma once
#include "reclaimer.hpp"

class LeakyReclaimer final : public Reclaimer {
public:
    void register_thread(std::size_t) override {}
    void unregister_thread(std::size_t) override {}
    void enter(std::size_t) override {}
    void exit(std::size_t) override {}
    std::uintptr_t protect(std::size_t, std::size_t,
                           const std::atomic<std::uintptr_t>& link) override {
        return link.load(std::memory_order_seq_cst);
    }
    void clear(std::size_t, std::size_t) override {}
    void retire(std::size_t, Node*) override {} // Intentionally never frees nodes.
};
