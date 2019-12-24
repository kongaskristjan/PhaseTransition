#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "Lib/Universe.h"
#include "Lib/Vector2.h"
#include "Recorder.h"
#include <SDL2/SDL.h>
#include <SDL_ttf.h>

enum class MouseAction { heat, push, create, spray };

struct CallbackHandler {
    CallbackHandler(int _totalParticleTypes);
    void mouseCallback(const SDL_Event &event);
    void keyboardCallback(const SDL_Event &event);

    Vector2D pos = Vector2D(1e6, 1e6);
    int sign = 0; // left mouse = 1, none = 0, right mouse = -1
    double radius = 50;
    bool leftDown = false, rightDown = false;
    bool quit = false;

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
    Display(Universe &universe, const std::string &_windowCaption, const std::string &_displayedCaption,
            const std::string &_directoryPath, const std::string &recordingPath="");
    ~Display();
    const CallbackHandler & update();

private:
    void drawParticles();
    void drawDisplayedCaption();
    void drawPointer();
    void drawStats();
    void drawText(const std::string &text, int x, int y);
    void recordAndDrawRecordingText();
    void drawSpriteFromCenter(SDL_Surface *sprite, int x, int y);
    std::tuple<int, double, double> computeStats() const;

    Universe &universe;
    std::string windowCaption, displayedCaption;
    std::string directoryPath;
    CallbackHandler handler;
    Recorder recorder;

    SDL_Window *window = nullptr;
    SDL_Surface *surface = nullptr;
    SDL_Surface *defaultPointer = nullptr, *increasePointer = nullptr, *decreasePointer = nullptr;
    TTF_Font *font;
};

std::string to_string(double x, int precision);

#endif
