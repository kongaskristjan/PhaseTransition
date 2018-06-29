#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "Lib/Universe.h"

class Display {
public:
    Display(size_t _sizeX, size_t _sizeY, const std::string &_caption);
    void update(const Universe &universe, double waitSeconds);

private:
    size_t sizeX, sizeY;
    std::string caption;
};

#endif
