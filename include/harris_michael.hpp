#pragma once
#include "node.hpp"
#include "reclaimer.hpp"
#include <cstddef>

class HarrisMichaelList {
public:
    explicit HarrisMichaelList(Reclaimer& reclaimer);
    ~HarrisMichaelList();
    HarrisMichaelList(const HarrisMichaelList&) = delete;
    HarrisMichaelList& operator=(const HarrisMichaelList&) = delete;

    bool insert(int key, std::size_t tid);
    bool remove(int key, std::size_t tid);
    bool contains(int key, std::size_t tid);

private:
    struct Window { Node* pred; Node* curr; };
    Window find(int key, std::size_t tid);
    Reclaimer& reclaimer_;
    Node* head_;
    Node* tail_;
};
