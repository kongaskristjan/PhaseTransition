
#include <cmath>
#include <algorithm>
#include <random>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "Display.h"
#include "Globals.h"

void CallbackHandler::mouseCallback(int event, int x, int y, int flags, void *userdata) {
    CallbackHandler &thisHandler = * (CallbackHandler *) userdata;
    if(x != -1 || y != -1) thisHandler.pos = Vector2D(x, y);

    switch(event) {
    case CV_EVENT_LBUTTONDOWN: thisHandler.leftDown = true; break;
    case CV_EVENT_LBUTTONUP: thisHandler.leftDown = false; break;
    case CV_EVENT_RBUTTONDOWN: thisHandler.rightDown = true; break;
    case CV_EVENT_RBUTTONUP: thisHandler.rightDown = false; break;
    case CV_EVENT_MOUSEWHEEL:
        thisHandler.radius *= pow(1.2, cv::getMouseWheelDelta(flags));
        thisHandler.radius = std::max(thisHandler.radius, 10.);
        thisHandler.radius = std::min(thisHandler.radius, 200.);
        break;
    }
    thisHandler.sign = (int) thisHandler.leftDown - (int) thisHandler.rightDown;
}

void CallbackHandler::setActionFromKey(int key) {
    switch(key) {
        case 'h': action = MouseAction::heat; break;
        case 'p': action = MouseAction::push; break;
        case 'c': action = MouseAction::create; break;
        case 's': action = MouseAction::spray; break;
    }
}

void CallbackHandler::drawPointer(cv::Mat &img) {
    auto circleColor = cv::Scalar(255, 255, 255);
    if(sign > 0) circleColor = cv::Scalar(0, 0, 255);
    if(sign < 0) circleColor = cv::Scalar(255, 0, 0);
    cv::circle(img, cv::Point(pos.x, pos.y), radius, circleColor, 1);

    std::string text = "";
    if(action == MouseAction::heat) text = "Heat mode";
    if(action == MouseAction::push) text = "Push mode";
    if(action == MouseAction::create) text = "Create mode";
    if(action == MouseAction::spray) text = "Spray mode";
    auto textColor = cv::Scalar(255, 255, 255);
    cv::putText(img, text, cv::Point(30, 60), cv::FONT_HERSHEY_PLAIN, 2., textColor, 2);
}


void UniverseModifier::modify(Universe &universe, const CallbackHandler &handler, double dT, int type) {
    if(! handler.sign) return; // No action from user

    modifyExisting(universe, handler, dT);
    addNew(universe, handler, dT, type);
}

void UniverseModifier::modifyExisting(Universe &universe, const CallbackHandler &handler, double dT) {
    const double heatingSpeed = 0.1;
    const double pushingSpeed = 0.5, pullingSpeed = 0.2;
    const double removeSpeed = 0.5;

    auto it = universe.begin();
    while(it != universe.end()) {
        bool skipIncrement = false;
        auto &state = *it;
        if((state.pos - handler.pos).magnitude() < handler.radius) {
            switch(handler.action) {
            case MouseAction::heat:
                state.v *= 1. + handler.sign * heatingSpeed * dT;
                break;
            case MouseAction::push:
                if(handler.sign > 0) {
                    state.v += (state.pos - handler.pos) * (pushingSpeed * dT / handler.radius);
                } else if (handler.sign < 0) {
                    state.v -= (state.pos - handler.pos) * (pullingSpeed * dT / handler.radius);
                    state.v *= 1. - heatingSpeed * dT;
                }
                break;
            case MouseAction::create:
                if(handler.sign > 0) { // pull and slow particles
                    state.v -= (state.pos - handler.pos) * (pullingSpeed * dT / handler.radius);
                    state.v *= 1. - heatingSpeed * dT;
                }
            case MouseAction::spray:
                if(handler.sign == -1) {
                    double prob = removeSpeed * dT;
                    if(std::bernoulli_distribution(prob)(randomGenerator)) {
                        it = universe.erase(it);
                        skipIncrement = true;
                    }
                }
                break;
            }
        }

        if(! skipIncrement) ++it;
    }
}

void UniverseModifier::addNew(Universe &universe, const CallbackHandler &handler, double dT, int type) {
    if(handler.sign <= 0) return;

    const double sprayParticleSpeedCoef = 0.08;

    auto phiDistr = std::uniform_real_distribution<>(0., 2 * M_PI);
    std::array<double, 2> x = {0, handler.radius};
    std::array<double, 2> y = {0, 1};
    auto rDistr = std::piecewise_linear_distribution<>(x.begin(), x.end(), y.begin());

    switch(handler.action) {
    case MouseAction::create: {
        double phi = phiDistr(randomGenerator);
        double r = rDistr(randomGenerator);
        auto pos = universe.clampInto(handler.pos + Vector2D(r * cos(phi), r * sin(phi)));
        auto state = ParticleState(pos);
        universe.addParticle(type, state);
        break;
    }
    case MouseAction::spray: {
        double phi = phiDistr(randomGenerator);
        double velocity = sprayParticleSpeedCoef * handler.radius;
        auto state = ParticleState(handler.pos, Vector2D(velocity * cos(phi), velocity * sin(phi)));
        universe.addParticle(type, state);
        break;
    }
    default: break;
    }
}


Display::Display(size_t _sizeX, size_t _sizeY, const std::string &_caption): sizeX(_sizeX), sizeY(_sizeY), caption(_caption) {
    cv::namedWindow(caption, cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback(caption, CallbackHandler::mouseCallback, & handler);
}

const CallbackHandler & Display::update(Universe &universe, double waitSeconds) {
    auto img = cv::Mat(cv::Size(sizeX, sizeY), CV_8UC3, cv::Scalar(0, 0, 0));
    for(auto it = universe.begin(); it != universe.end(); ++it) {
        double radius = 0.6 * it->type->getRadius();
        cv::circle(img, cv::Point2i(it->pos.x, it->pos.y), radius, it->type->getColor(), -1);
    }
    handler.drawPointer(img);

    cv::imshow(caption, img);
    handler.setActionFromKey(cv::waitKey(1000 * waitSeconds));
    return handler;
}
