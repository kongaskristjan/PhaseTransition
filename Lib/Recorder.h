#ifndef __RECORDER_H__
#define __RECORDER_H__

#include <string>
#include <iostream>
#include <SDL_surface.h>

#ifdef FFMPEG_ENABLED

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

class AVRecorder {
public:
    AVRecorder(std::string outputPath, int sizeX, int sizeY, int frameRate);
    ~AVRecorder();
    void encode(SDL_Surface *);
    bool isRecording() const;

private:
    AVCodecContext *context = nullptr;
    AVFrame *frame = nullptr;
    AVPacket *packet = nullptr;
    FILE *file;
    int64_t timestep = 0;

    bool doRecord = false;
};

typedef AVRecorder Recorder;

#else

class StubRecorder {
public:
    inline StubRecorder(std::string outputPath, int sizeX, int sizeY, int frameRate) {
        if(!outputPath.empty()) {
            std::cerr << "Warning: This binary was not compiled with FFMPEG recording support" << std::endl;
            std::cerr << std::endl;
        }
    }
    inline void encode(SDL_Surface *) {}
    inline bool recording() const { return false; }
};

typedef StubRecorder Recorder;

#endif

#endif
