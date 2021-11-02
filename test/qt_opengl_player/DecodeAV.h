#pragma once

#include <QThread>
#define __STDC_CONSTANT_MACROS
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h"
#include <libavutil/imgutils.h>
};
#include "Direct3D9Render.h"
#include "D3D9TextureRender.h"

class DecodeAV : public QThread
{
private:
public:
    DecodeAV();
    ~DecodeAV();
    int init(void* winId);

protected:
    void run();

private:
    AVFormatContext *m_pFormatCtx;
    int i, videoindex;
    AVCodecContext *m_pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameYUV;
    uint8_t *out_buffer;
    AVPacket *packet;
    struct SwsContext *img_convert_ctx;

    // Direct3D9Render* m_pDirectXRender;
    D3D9TextureRender* m_pDirectXRender;
    void* m_pWinId;
};

