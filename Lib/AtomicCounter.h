#ifndef ATOMICCOUNTER_H
#define ATOMICCOUNTER_H

#include <atomic>

/* AtomicCounter enforces safe parallel usage of an atomic counter. Example:
 *
 * for(int idx = atomicCounter.next(); idx < atomicCounter.total(); idx = atomicCounter.next()) { ... } */

class AtomicCounter {
public:
    AtomicCounter(std::size_t total);
    std::size_t next();
    std::size_t total() const;

private:
    std::atomic<std::size_t> idx;
    std::size_t total_;
};

#endif
