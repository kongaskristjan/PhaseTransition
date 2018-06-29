
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "Display.h"

Display::Display(size_t _sizeX, size_t _sizeY, const std::string &_caption): sizeX(_sizeX), sizeY(_sizeY), caption(_caption) {
    cv::namedWindow(caption, cv::WINDOW_AUTOSIZE);
}

void Display::update(const Universe &universe, double waitSeconds) {
    auto img = cv::Mat(cv::Size(sizeX, sizeY), CV_8UC3, cv::Scalar(0, 0, 0));
    for(size_t i = 0; i < universe.size(); ++i) {
        auto [type, state] = universe.getParticle(i);
        cv::circle(img, cv::Point2i(state.pos.x, state.pos.y), 3, cv::Scalar(255, 255, 255), -1);
    }
    cv::imshow(caption, img);
    cv::waitKey(1000 * waitSeconds);
}
