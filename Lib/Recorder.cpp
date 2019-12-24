
#ifdef FFMPEG_ENABLED

#include <iostream>
#include <cassert>
#include "Recorder.h"

#if __cplusplus >= 201703L
#include <filesystem>
#endif

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

AVRecorder::AVRecorder(std::string outputPath, int sizeX, int sizeY, int frameRate) {
    if(outputPath.empty()) return;

#if __cplusplus >= 201703L
    auto path = std::filesystem::path(recordingPath);
    std::filesystem::create_directories(path.parent_path());
#endif

    std::cerr << outputPath.c_str() << std::endl;
    file = fopen(outputPath.c_str(), "wb");
    if(! file) {
        std::cerr << "Warning: Couldn't create " << outputPath << std::endl;
        std::cerr << "Note: Perhaps corresponding directory doesn't exist?" << std::endl;
        std::cerr << std::endl;
        return;
    }
    doRecord = true;

    AVCodec *codec = avcodec_find_encoder_by_name("libx264rgb");
    context = avcodec_alloc_context3(codec);
    packet = av_packet_alloc();
    std::cerr << file << " " << codec << " " << context << " " << packet << std::endl;

    context->bit_rate = 30000000;
    context->width = sizeX;
    context->height = sizeY;
    context->time_base = (AVRational) {1, frameRate};
    context->framerate = (AVRational) {frameRate, 1};

    context->gop_size = 10;
    context->max_b_frames = 1;
    context->pix_fmt = AV_PIX_FMT_RGB24;

    avcodec_open2(context, codec, NULL);

    frame = av_frame_alloc();
    frame->format = context->pix_fmt;
    frame->width = context->width;
    frame->height = context->height;
    std::cerr << "getting buffer: " << av_frame_get_buffer(frame, 32) << std::endl;
}

AVRecorder::~AVRecorder() {
    std::cerr << "Closing" << std::endl;
    if(! doRecord) return;

    avcodec_send_frame(context, nullptr);
    int ret;
    while (ret >= 0) {
        ret = avcodec_receive_packet(context, packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }

        //printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        fwrite(packet->data, 1, packet->size, file);
        av_packet_unref(packet);
    }


    fclose(file);

    avcodec_free_context(&context);
    av_frame_free(&frame);
    av_packet_free(&packet);
}

void AVRecorder::encode(SDL_Surface *surface) {
    assert(doRecord);

    std::cerr << "make frame writable: " << av_frame_make_writable(frame) << std::endl;

    SDL_LockSurface(surface);
    for(int y = 0; y < frame->height; ++y) {
        for(int x = 0; x < frame->width; ++x) {
            SDL_Color rgb;
            Uint32 data = *((Uint32 *) surface->pixels + y * frame->width + x);//getpixel(gSurface, 200, 200);
            SDL_GetRGB(data, surface->format, &rgb.r, &rgb.g, &rgb.b);

            frame->data[0][y * frame->linesize[0] + 3 * x] = rgb.r;
            frame->data[0][y * frame->linesize[0] + 3 * x + 1] = rgb.g;
            frame->data[0][y * frame->linesize[0] + 3 * x + 2] = rgb.b;
        }
    }
    SDL_UnlockSurface(surface);

    frame->pts = timestep;
    std::cerr << frame->pts << std::endl;
    std::cerr << "sending frame: " << avcodec_send_frame(context, frame) << std::endl;

    int errorCode = 0;
    while(errorCode >= 0) {
        errorCode = avcodec_receive_packet(context, packet);
        std::cerr << "errorCode: " << errorCode << std::endl;
        if (errorCode == AVERROR(EAGAIN) || errorCode == AVERROR_EOF)
            return;
        else if (errorCode < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }

        fwrite(packet->data, 1, packet->size, file);
        std::cerr << packet->pts << std::endl;
        printf("Write packet %3"PRId64" (size=%5d)\n", packet->pts, packet->size);
        av_packet_unref(packet);
    }
    std::cerr << "end cycle" << std::endl;

    ++timestep;
}

bool AVRecorder::isRecording() const {
    return doRecord;
}

#endif
