
#include <cmath>
#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <chrono>
#include <cassert>
#include <SDL_ttf.h>
#include "Display.h"
#include "Globals.h"
#include "StatsMap.h"

#ifdef SDL2_IMAGE_ENABLED
#include <SDL_image.h>
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
        int scroll = 0;
        if(event.wheel.y > 0) scroll = 1;
        if(event.wheel.y < 0) scroll = -1;
        radius *= pow(1.2, -scroll);
        radius = std::max(radius, 10.);
        radius = std::min(radius, 200.);
    }
    sign = (int) leftDown - (int) rightDown;
}

void CallbackHandler::keyboardCallback(const SDL_Event &event) {
    int key = event.key.keysym.sym;
    if(key == 'h') action = MouseAction::heat;
    if(key == 'p') action = MouseAction::push;
    if(key == 'c') action = MouseAction::create;
    if(key == 's') action = MouseAction::spray;
    if(key == 't') isDrawingTemperature = ! isDrawingTemperature;

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


Display::Display(Universe &_universe, const std::string &_windowCaption, const std::string &_displayedCaption,
        const std::string &_directoryPath, const std::string &_recordingPath):
    universe(_universe), displayedCaption(_displayedCaption), directoryPath(_directoryPath),
        handler(_universe.getParticleTypes().size()), recordingPath(_recordingPath) {
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
    surface = SDL_GetWindowSurface(window);
    defaultPointer = SDL_LoadBMP((directoryPath + "Sprites/DefaultPointer.bmp").c_str());
    increasePointer = SDL_LoadBMP((directoryPath + "Sprites/IncreasePointer.bmp").c_str());
    decreasePointer = SDL_LoadBMP((directoryPath + "Sprites/DecreasePointer.bmp").c_str());

    TTF_Init();
    font = TTF_OpenFont((directoryPath + "Fonts/DroidSans.ttf").c_str(), 24);

    isRecording = false;
    if(! recordingPath.empty()) {
#if SDL2_IMAGE_ENABLED
        isRecording = true;
        system((std::string("mkdir -p ") + recordingPath).c_str());
#else
        std::cerr << "Warning: trying to record, but this build does not support recording" << std::endl;
#endif
    }
}

Display::~Display() {
    SDL_FreeSurface(defaultPointer);
    SDL_FreeSurface(increasePointer);
    SDL_FreeSurface(decreasePointer);
    TTF_CloseFont(font);

    SDL_DestroyWindow(window);
    SDL_Quit();
}

const CallbackHandler & Display::update() {
    SDL_FillRect(surface, nullptr, 0x000000);
    if(handler.isDrawingTemperature) drawTemperature();
    drawParticles();
    drawDisplayedCaption();
    drawStats();
    drawPointer();
    if(isRecording) recordAndDrawRecordingText();
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

void Display::drawTemperature() {
    Image<Stats> statsImage = computeStatsImage(universe, 20, 10);
    SDL_Surface *smallMap = SDL_CreateRGBSurface(0, statsImage.sizeX(), statsImage.sizeY(), 24, 0, 0, 0, 0);
    auto* pixels = (unsigned char*)smallMap->pixels;
    for(int y = 0; y < statsImage.sizeY(); ++y) {
        for(int x = 0; x < statsImage.sizeX(); ++x) {
            Stats stats = statsImage.get({x, y});
            float strength = std::min(1.0, 10 * stats.density);
            float color = std::min(1.0, stats.temp);
            //std::cout << strength << " " << color << std::endl;
            //std::cout << 3 * (y * surface->w + x) + 0 << std::endl;
            pixels[3 * (y * smallMap->w + x) + 0] = 255;//(unsigned char) (255. * strength * color); // red
            pixels[3 * (y * smallMap->w + x) + 1] = 0;//(unsigned char) (255. * strength * (1 - color)); // green
            pixels[3 * (y * smallMap->w + x) + 2] = 0;//(unsigned char) (255. * strength * 0.5); // blue
        }
    }
    SDL_BlitScaled(smallMap, nullptr, surface, nullptr);
    SDL_FreeSurface(smallMap);
}

void Display::drawDisplayedCaption() {
    drawText(displayedCaption, 30, 60);
}

void Display::drawPointer() {
    SDL_Surface *pointer = defaultPointer;
    if(handler.sign > 0) pointer = increasePointer;
    if(handler.sign < 0) pointer = decreasePointer;

    const int numPointers = 12;
    for(int i = 0; i < numPointers; ++i) {
        double radians = (2. * M_PI / numPointers) * i;
        double dx = handler.radius * sin(radians), dy = handler.radius * cos(radians);
        drawSpriteFromCenter(pointer, (int) (handler.pos.x + dx), (int) (handler.pos.y + dy));
    }

    std::string modeText = "";
    if(handler.action == MouseAction::heat) modeText = "Heat mode";
    if(handler.action == MouseAction::push) modeText = "Push mode";
    if(handler.action == MouseAction::create) modeText = "Create mode";
    if(handler.action == MouseAction::spray) modeText = "Spray mode";
    drawText(modeText, 30, 120);

    int typeIdx = handler.particleTypeIdx;
    auto type = universe.getParticleTypes()[typeIdx];
    std::string typeText = std::to_string(typeIdx + 1) + ": " + type.getName();
    drawText(typeText, 30, 150);
}

void Display::drawStats() {
    int n;
    double velocity, temp;
    std::tie(n, velocity, temp) = computeStats();

    const int prec = 2;
    drawText("n = " + std::to_string(n), 30, 200);
    drawText("velocity = " + to_string(velocity, prec), 30, 230);
    drawText("temp = " + to_string(temp, prec), 30, 260);
}

void Display::drawText(const std::string &text, int x, int y) {
    if(text == "") return;

    SDL_Color color = {255, 255, 255};
    SDL_Surface* message = TTF_RenderText_Blended(font, text.c_str(), color);

    SDL_Rect outputRect;
    outputRect.x = x;
    outputRect.y = y;
    outputRect.w = message->w;
    outputRect.h = message->h;

    SDL_BlitSurface(message, nullptr, surface, & outputRect);
    SDL_FreeSurface(message);
}

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

void Display::drawParticles() {
    for(auto it = universe.begin(); it != universe.end(); ++it) {
        auto *particle = (SDL_Surface *) it->type->getSpriteSurface();
        assert(particle != nullptr);
        drawSpriteFromCenter(particle, it->pos.x, it->pos.y);
    }
}

void Display::recordAndDrawRecordingText() {
    if(! isRecording) return;

#ifdef SDL2_IMAGE_ENABLED
    IMG_SaveJPG(surface, (recordingPath + std::to_string(timestamp++) + ".jpg").c_str(), 95);
#endif

    auto now = std::chrono::system_clock::now();
    auto millisFromEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    if (millisFromEpoch % 1000 < 500)
        drawText("Recording...", 30, 30);
}

void Display::drawSpriteFromCenter(SDL_Surface *sprite, int x, int y) {
    assert(sprite != nullptr);
    SDL_Rect inputRect{0, 0, sprite->w, sprite->h};
    SDL_Rect outputRect{x - sprite->w / 2, y - sprite->h / 2, sprite->w, sprite->h};
    SDL_BlitSurface(sprite, & inputRect, surface, & outputRect);
}

std::string to_string(double x, int precision) {
    std::stringstream ss;
    ss << std::setprecision(precision) << std::fixed << x;
    return ss.str();
}
