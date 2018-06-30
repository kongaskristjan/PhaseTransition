#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "Lib/Universe.h"

struct CallbackHandler {
    int x, y, flags;
    bool leftDown = false, rightDown = false;

    static void mouseCallback(int event, int _x, int _y, int _flags, void *userdata);
};


class Display {
public:
    Display(size_t _sizeX, size_t _sizeY, const std::string &_caption);
    const CallbackHandler & update(const Universe &universe, double waitSeconds);

private:
    size_t sizeX, sizeY;
    std::string caption;
    CallbackHandler handler;
};

#endif
