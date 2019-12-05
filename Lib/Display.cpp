
#include <cmath>
#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>
#include "Display.h"
#include "Globals.h"

#if __cplusplus >= 201703L
#include <filesystem>
#endif

CallbackHandler::CallbackHandler(int _totalParticleTypes):
    totalParticleTypes(_totalParticleTypes) {
}

void CallbackHandler::mouseCallback(const SDL_Event &event) {
    int x, y;
    SDL_GetMouseState(& x, & y);
    pos = Vector2D(x, y);

    if(event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
        bool down = event.type == SDL_MOUSEBUTTONDOWN;
        if (event.button.button == SDL_BUTTON_LEFT) leftDown = down;
        if (event.button.button == SDL_BUTTON_RIGHT) rightDown = down;
    }
    if(event.type == SDL_MOUSEWHEEL) {
        radius *= pow(1.2, -event.wheel.y);
        radius = std::max(radius, 10.);
        radius = std::min(radius, 200.);
    }
    sign = (int) leftDown - (int) rightDown;
    std::cout << x << " " << y << "    " << sign << " " << radius << std::endl;
}

void CallbackHandler::keyboardCallback(const SDL_Event &event) {
    int key = event.key.keysym.sym;
    if(key == 'h') action = MouseAction::heat;
    if(key == 'p') action = MouseAction::push;
    if(key == 'c') action = MouseAction::create;
    if(key == 's') action = MouseAction::spray;

    if('1' <= key && key <= '9') {
        int newParticleType = key - '0' - 1;
        if(newParticleType < totalParticleTypes)
            particleTypeIdx = newParticleType;
    }
}


void UniverseModifier::modify(Universe &universe, const CallbackHandler &handler, double dT) {
    if(! handler.sign) return; // No action from user

    modifyExisting(universe, handler, dT);
    addNew(universe, handler, dT);
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

void UniverseModifier::addNew(Universe &universe, const CallbackHandler &handler, double dT) {
    if(handler.sign <= 0) return;

    const double creationRadiusCoef = 0.8;
    const double sprayParticleSpeedCoef = 0.08;
    const double newParticlesCoef = 0.1;

    auto phiDistr = std::uniform_real_distribution<>(0., 2 * M_PI);
    std::array<double, 2> x = {0, creationRadiusCoef * handler.radius};
    std::array<double, 2> y = {0, 1};
    auto rDistr = std::piecewise_linear_distribution<>(x.begin(), x.end(), y.begin());

    switch(handler.action) {
    case MouseAction::create: {
        double particleR = universe.getParticleTypes()[handler.particleTypeIdx].getRadius();
        int newParticles = newParticlesCoef * handler.radius / particleR + 1;
        for(int i = 0; i < newParticles; ++i) {
            double phi = phiDistr(randomGenerator);
            double r = rDistr(randomGenerator);
            auto pos = universe.clampInto(handler.pos + Vector2D(r * cos(phi), r * sin(phi)));
            auto state = ParticleState(pos);
            universe.addParticle(handler.particleTypeIdx, state);
        }
        break;
    }
    case MouseAction::spray: {
        double phi = phiDistr(randomGenerator);
        double velocity = sprayParticleSpeedCoef * handler.radius;
        auto state = ParticleState(handler.pos, Vector2D(velocity * cos(phi), velocity * sin(phi)));
        universe.addParticle(handler.particleTypeIdx, state);
        break;
    }
    default: break;
    }
}


Display::Display(Universe &_universe, const std::string &_windowCaption, const std::string &_displayedCaption, const std::string &recordingPath):
    universe(_universe), displayedCaption(_displayedCaption), handler(_universe.getParticleTypes().size()) {
    windowCaption = _windowCaption + " - " + _displayedCaption;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    window = SDL_CreateWindow(windowCaption.data(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              universe.getConfig().sizeX, universe.getConfig().sizeY, SDL_WINDOW_SHOWN);
    if(window == nullptr) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    if(! recordingPath.empty()) {
#if __cplusplus >= 201703L
        auto path = std::filesystem::path(recordingPath);
        std::filesystem::create_directories(path.parent_path());
#endif
        //recorder.open(recordingPath, CV_FOURCC('M','J','P','G'), 60, cv::Size(universe.getConfig().sizeX, universe.getConfig().sizeY));
    }
}

Display::~Display() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

const CallbackHandler & Display::update() {
    /*
    auto img = drawParticles();
    drawDisplayedCaption(img);
    drawPointer(img);
    drawStats(img);
    */
    /*
    if(recorder.isOpened()) {
        recorder.write(img);

        auto now = std::chrono::system_clock::now();
        auto millisFromEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        if(millisFromEpoch % 1000 < 500)
            drawText(img, "Recording...", cv::Point(30, 30));
    }
    */

    SDL_UpdateWindowSurface(window);
    SDL_Event event;
    while(SDL_PollEvent(& event)) {
        if(event.type == SDL_QUIT)
            handler.quit = true;
        if(event.type == SDL_KEYDOWN)
            handler.keyboardCallback(event);
        if(event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEWHEEL
                || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
            handler.mouseCallback(event);
    }
    return handler;
}
/*
void Display::drawDisplayedCaption(cv::Mat &img) const {
    drawText(img, displayedCaption, cv::Point(30, 60), cv::Scalar(255, 255, 255));
}

void Display::drawPointer(cv::Mat &img) const {
    auto circleColor = cv::Scalar(255, 255, 255);
    if(handler.sign > 0) circleColor = cv::Scalar(0, 0, 255);
    if(handler.sign < 0) circleColor = cv::Scalar(255, 0, 0);
    cv::circle(img, cv::Point(handler.pos.x, handler.pos.y), handler.radius, circleColor, 1);

    std::string modeText = "";
    if(handler.action == MouseAction::heat) modeText = "Heat mode";
    if(handler.action == MouseAction::push) modeText = "Push mode";
    if(handler.action == MouseAction::create) modeText = "Create mode";
    if(handler.action == MouseAction::spray) modeText = "Spray mode";
    drawText(img, modeText, cv::Point(30, 120));

    int typeIdx = handler.particleTypeIdx;
    auto type = universe.getParticleTypes()[typeIdx];
    std::string typeText = std::to_string(typeIdx + 1) + ": " + type.getName();
    drawText(img, typeText, cv::Point(30, 150), type.getColor());
}

void Display::drawStats(cv::Mat &img) const {
    int n;
    double velocity, temp;
    std::tie(n, velocity, temp) = computeStats();

    const int prec = 2;
    drawText(img, "n = " + std::to_string(n), cv::Point(30, 200));
    drawText(img, "velocity = " + to_string(velocity, prec), cv::Point(30, 230));
    drawText(img, "temp = " + to_string(temp, prec), cv::Point(30, 260));
}

void Display::drawText(cv::Mat &img, const std::string &text, const cv::Point &loc, const cv::Scalar &color) const {
    cv::putText(img, text, loc, cv::FONT_HERSHEY_PLAIN, 2., color, 2);
}
*/
std::tuple<int, double, double> Display::computeStats() const {
    int n = 0;
    double mass = 0;
    Vector2D momentum;

    // Compute velocity
    for(auto it = universe.begin(); it != universe.end(); ++it) {
        double pMass = it->type->getMass();
        Vector2D &pPos = it->pos, &pV = it->v;
        if((pPos - handler.pos).magnitude2() < handler.radius * handler.radius) {
            ++n;
            mass += pMass;
            momentum += pV * pMass;
        }
    }
    Vector2D velocity = momentum / mass;

    // Compute kinetic energy relative to average speed
    double energy = 0;
    for(auto it = universe.begin(); it != universe.end(); ++it) {
        double pMass = it->type->getMass();
        Vector2D &pPos = it->pos, &pV = it->v;
        if((pPos - handler.pos).magnitude2() < handler.radius * handler.radius)
            energy += (pV - velocity).magnitude2() * pMass / 2;
    }
    double temp = energy / n; // E = kT * (degrees of freedom = 2) / 2, k == 1 (natural units)
    return { n, velocity.magnitude(), temp };
}
/*
cv::Mat Display::drawParticles() const {
    auto img = cv::Mat(cv::Size(universe.getConfig().sizeX, universe.getConfig().sizeY), CV_8UC3, cv::Scalar(0, 0, 0));
    for(auto it = universe.begin(); it != universe.end(); ++it) {
        double radius = 0.6 * it->type->getRadius();
        cv::circle(img, cv::Point2i(it->pos.x, it->pos.y), radius, it->type->getColor(), -1);
    }
    return img;
}
*/

std::string to_string(double x, int precision) {
    std::stringstream ss;
    ss << std::setprecision(precision) << std::fixed << x;
    return ss.str();
}
