
#include "AtomicCounter.h"

AtomicCounter::AtomicCounter(std::size_t total): idx(0), total_(total) {
}

std::size_t AtomicCounter::next() {
    return idx.fetch_add(1, std::memory_order_relaxed);
}

std::size_t AtomicCounter::total() const {
    return total_;
}
