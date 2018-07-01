#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "Lib/Universe.h"
#include "Lib/Vector2.h"

struct CallbackHandler {
    Vector2D pos = Vector2D(1e6, 1e6);
    int increase = 0; // left mouse = 1, none = 0, right mouse = -1
    double radius = 50;
    bool leftDown = false, rightDown = false;

    char key = 't';

    static void mouseCallback(int event, int _x, int _y, int _flags, void *userdata);
    void setKey(int _key);
    void drawPointer(cv::Mat &img);
};

void modifyUniverse(Universe &universe, const CallbackHandler &handler, double dT);

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
