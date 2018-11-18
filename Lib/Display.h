#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "Lib/Universe.h"
#include "Lib/Vector2.h"

enum class MouseAction { heat, push, create, spray };

struct CallbackHandler {
    Vector2D pos = Vector2D(1e6, 1e6);
    int sign = 0; // left mouse = 1, none = 0, right mouse = -1
    double radius = 50;
    bool leftDown = false, rightDown = false;

    MouseAction action = MouseAction::create;

    static void mouseCallback(int event, int _x, int _y, int _flags, void *userdata);
    void setActionFromKey(int _key);
};

class UniverseModifier {
public:
    static void modify(Universe &universe, const CallbackHandler &handler, double dT, int type);

private:
    static void modifyExisting(Universe &universe, const CallbackHandler &handler, double dT);
    static void addNew(Universe &universe, const CallbackHandler &handler, double dT, int type);
};

class Display {
public:
    Display(size_t _sizeX, size_t _sizeY, const std::string &_caption);
    const CallbackHandler & update(Universe &universe);

private:
    cv::Mat drawParticles(Universe &universe) const;
    void drawPointer(cv::Mat &img) const;
    void drawStats(cv::Mat &img, Universe &universe) const;
    std::tuple<int, double, double> computeStats(Universe &universe) const;
    void displayText(cv::Mat &img, const std::string &text, const cv::Point &loc) const;

    size_t sizeX, sizeY;
    std::string caption;
    CallbackHandler handler;
    const cv::Scalar textColor{ 255, 255, 255 };
};

std::string to_string(double x, int precision);

#endif
