#include "harris_michael.hpp"
#include <limits>
#include <stdexcept>

HarrisMichaelList::HarrisMichaelList(Reclaimer& r) : reclaimer_(r) {
    tail_ = new Node(std::numeric_limits<int>::max());
    head_ = new Node(std::numeric_limits<int>::min());
    head_->next.store(tagged::raw(tail_));
}

// Only call after all worker threads have stopped. This intentionally does not
// free previously unlinked nodes, consistent with the leaky baseline.
HarrisMichaelList::~HarrisMichaelList() {
    Node* n = head_;
    while (n) {
        Node* next = tagged::ptr(n->next.load());
        delete n;
        n = next;
    }
}

HarrisMichaelList::Window HarrisMichaelList::find(int key, std::size_t tid) {
retry:
    Node* pred = head_;
    Node* curr = tagged::ptr(reclaimer_.protect(tid, 0, pred->next));
    for (;;) {
        auto link = reclaimer_.protect(tid, 1, curr->next);
        Node* succ = tagged::ptr(link);
        if (tagged::marked(link)) {
            auto expected = tagged::raw(curr);
            if (!pred->next.compare_exchange_strong(expected, tagged::raw(succ)))
                goto retry;
            reclaimer_.retire(tid, curr);
            curr = succ;
        } else {
            if (curr->key >= key) return {pred, curr};
            pred = curr;
            curr = succ;
        }
    }
}

bool HarrisMichaelList::insert(int key, std::size_t tid) {
    if (key == std::numeric_limits<int>::min() || key == std::numeric_limits<int>::max())
        throw std::invalid_argument("sentinel key");
    reclaimer_.enter(tid);
    Node* fresh = new Node(key);
    for (;;) {
        auto [pred, curr] = find(key, tid);
        if (curr->key == key) {
            delete fresh;
            reclaimer_.exit(tid);
            return false;
        }
        fresh->next.store(tagged::raw(curr));
        auto expected = tagged::raw(curr);
        if (pred->next.compare_exchange_strong(expected, tagged::raw(fresh))) {
            reclaimer_.exit(tid);
            return true;
        }
    }
}

bool HarrisMichaelList::remove(int key, std::size_t tid) {
    if (key == std::numeric_limits<int>::min() || key == std::numeric_limits<int>::max())
        return false;
    reclaimer_.enter(tid);
    for (;;) {
        auto [pred, curr] = find(key, tid);
        if (curr->key != key) {
            reclaimer_.exit(tid);
            return false;
        }
        auto link = curr->next.load();
        if (tagged::marked(link)) continue;
        if (!curr->next.compare_exchange_strong(link, link | tagged::MARK)) continue;
        auto expected = tagged::raw(curr);
        if (pred->next.compare_exchange_strong(expected, link & ~tagged::MARK))
            reclaimer_.retire(tid, curr);
        reclaimer_.exit(tid);
        return true;
    }
}

bool HarrisMichaelList::contains(int key, std::size_t tid) {
    if (key == std::numeric_limits<int>::min() || key == std::numeric_limits<int>::max())
        return false;
    reclaimer_.enter(tid);
    // Use find so the same traversal protocol will be used for reclamation.
    auto [pred, curr] = find(key, tid);
    (void)pred;
    bool found = curr->key == key && !tagged::marked(curr->next.load());
    reclaimer_.exit(tid);
    return found;
}
