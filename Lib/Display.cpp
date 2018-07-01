
#include <cmath>
#include <algorithm>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "Display.h"

void CallbackHandler::mouseCallback(int event, int x, int y, int flags, void *userdata) {
    CallbackHandler &thisHandler = * (CallbackHandler *) userdata;
    if(x != -1 || y != -1) thisHandler.pos = Vector2D(x, y);

    if(event == CV_EVENT_LBUTTONDOWN) thisHandler.leftDown = true;
    if(event == CV_EVENT_LBUTTONUP) thisHandler.leftDown = false;
    if(event == CV_EVENT_RBUTTONDOWN) thisHandler.rightDown = true;
    if(event == CV_EVENT_RBUTTONUP) thisHandler.rightDown = false;
    if(event == CV_EVENT_MOUSEWHEEL) {
        thisHandler.radius *= pow(1.2, cv::getMouseWheelDelta(flags));
        thisHandler.radius = std::max(thisHandler.radius, 10.);
        thisHandler.radius = std::min(thisHandler.radius, 200.);
    } 
    thisHandler.increase = (int) thisHandler.leftDown - (int) thisHandler.rightDown;
}

void CallbackHandler::setKey(int _key) {
    if(_key != -1) key = _key;
}

void CallbackHandler::drawPointer(cv::Mat &img) {
    auto color = cv::Scalar(255, 255, 255);
    if(increase > 0) color = cv::Scalar(0, 0, 255);
    if(increase < 0) color = cv::Scalar(255, 0, 0);
    
    cv::circle(img, cv::Point(pos.x, pos.y), radius, color, 1);
}

void modifyUniverse(Universe &universe, const CallbackHandler &handler, double dT) {
}

Display::Display(size_t _sizeX, size_t _sizeY, const std::string &_caption): sizeX(_sizeX), sizeY(_sizeY), caption(_caption) {
    cv::namedWindow(caption, cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback(caption, CallbackHandler::mouseCallback, & handler);
}

const CallbackHandler & Display::update(const Universe &universe, double waitSeconds) {
    auto img = cv::Mat(cv::Size(sizeX, sizeY), CV_8UC3, cv::Scalar(0, 0, 0));
    for(size_t i = 0; i < universe.size(); ++i) {
        auto [type, state] = universe.getConstParticle(i);
        auto radius = 0.6 * type.getRadius();
        cv::circle(img, cv::Point2i(state.pos.x, state.pos.y), radius, type.getColor(), -1);
    }
    handler.drawPointer(img);

    cv::imshow(caption, img);
    handler.setKey(cv::waitKey(1000 * waitSeconds));
    return handler;
}
