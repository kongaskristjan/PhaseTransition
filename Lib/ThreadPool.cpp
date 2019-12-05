
#include <thread>
#include "Lib/ThreadPool.h"

ThreadPool threadPool(std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 1);
