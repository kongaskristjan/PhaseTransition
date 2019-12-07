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
    // spacer1 and spacer2 force non-atomic variables to different cache lines than atomic variables
    // This significantly improves performance of using address-wise nearby variables
    char spacer1[256];
    std::atomic<std::size_t> idx;
    char spacer2[256];
    std::size_t total_;
};

#endif
