#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "Lib/Universe.h"
#include "Lib/Vector2.h"
#include <opencv2/videoio.hpp>

enum class MouseAction { heat, push, create, spray };

struct CallbackHandler {
    CallbackHandler(int _totalParticleTypes);
    static void mouseCallback(int event, int _x, int _y, int _flags, void *userdata);
    void setActionFromKey(int _key);

    Vector2D pos = Vector2D(1e6, 1e6);
    int sign = 0; // left mouse = 1, none = 0, right mouse = -1
    double radius = 50;
    bool leftDown = false, rightDown = false;

    MouseAction action = MouseAction::create;
    int particleTypeIdx = 0;
private:
    const int totalParticleTypes = 1;
};

class UniverseModifier {
public:
    static void modify(Universe &universe, const CallbackHandler &handler, double dT);

private:
    static void modifyExisting(Universe &universe, const CallbackHandler &handler, double dT);
    static void addNew(Universe &universe, const CallbackHandler &handler, double dT);
};

class Display {
public:
    Display(Universe &universe, const std::string &_caption, const std::string &recordingPath="");
    const CallbackHandler & update();

private:
    cv::Mat drawParticles() const;
    void drawPointer(cv::Mat &img) const;
    void drawStats(cv::Mat &img) const;
    void drawText(cv::Mat &img, const std::string &text, const cv::Point &loc, const cv::Scalar &color=cv::Scalar(255, 255, 255)) const;
    std::tuple<int, double, double> computeStats() const;

    Universe &universe;
    std::string caption;
    CallbackHandler handler;
    cv::VideoWriter recorder;
};

std::string to_string(double x, int precision);

#endif
