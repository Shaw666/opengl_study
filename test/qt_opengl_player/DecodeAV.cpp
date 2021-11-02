#include "DecodeAV.h"
#include <iostream>

using namespace std;

DecodeAV::DecodeAV()
{
    // m_pDirectXRender = new Direct3D9Render();
    m_pDirectXRender = new D3D9TextureRender();
}

DecodeAV::~DecodeAV()
{
    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    avcodec_close(m_pCodecCtx);
    avformat_close_input(&m_pFormatCtx);
    sws_freeContext(img_convert_ctx);
    delete m_pDirectXRender;
}

void printFFmpegError(int e)
{
    char err_buffer[1024] = {0};
    av_strerror(e, err_buffer, 1024);
    cout << err_buffer;
}

int DecodeAV::init(void *winId)
{
    m_pWinId = winId;
    int ret = 0;
    char inputFilePath[] = "test.mp4";
    m_pFormatCtx = avformat_alloc_context(); //初始化一个AVFormatContext
    ret = avformat_open_input(&m_pFormatCtx, inputFilePath, NULL, NULL);
    if (ret != 0)
    { //打开输入的视频文件
        cout << "Couldn't open input stream.";
        printFFmpegError(ret);
        return -1;
    }
    if (avformat_find_stream_info(m_pFormatCtx, NULL) < 0)
    { //获取视频文件信息
        printf("Couldn't find stream information.\n");
        return -1;
    }
    videoindex = -1;
    for (unsigned int i = 0; i < m_pFormatCtx->nb_streams; i++)
        if (m_pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoindex = i;
            break;
        }

    if (videoindex == -1)
    {
        printf("Didn't find a video stream.\n");
        return -1;
    }
    m_pDirectXRender->InitD3D((HWND)m_pWinId, m_pFormatCtx->streams[videoindex]->codecpar->width, m_pFormatCtx->streams[videoindex]->codecpar->height);

    m_pCodecCtx = avcodec_alloc_context3(nullptr);
    if (!m_pCodecCtx)
    {
        return -1;
    }
    avcodec_parameters_to_context(m_pCodecCtx, m_pFormatCtx->streams[videoindex]->codecpar);
    //m_pCodecCtx = m_pFormatCtx->streams[videoindex]->codec;
    pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id); //查找解码器
    if (pCodec == NULL)
    {
        printf("Codec not found.\n");
        return -1;
    }
    if (avcodec_open2(m_pCodecCtx, pCodec, NULL) < 0)
    { //打开解码器
        printf("Could not open codec.\n");
        return -1;
    }

    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();
    int avframe_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, m_pCodecCtx->width, m_pCodecCtx->height, 4);
    out_buffer = (uint8_t *)av_malloc(avframe_size);
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P, m_pCodecCtx->width, m_pCodecCtx->height, 4);
    packet = (AVPacket *)av_malloc(sizeof(AVPacket));

    //Output Info-----------------------------
    printf("--------------- File Information ----------------\n");
    av_dump_format(m_pFormatCtx, 0, inputFilePath, 0);
    printf("-------------------------------------------------\n");
    img_convert_ctx = sws_getContext(m_pCodecCtx->width, m_pCodecCtx->height, m_pCodecCtx->pix_fmt,
                                     m_pCodecCtx->width, m_pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
    if (!img_convert_ctx)
    {
        return -1;
    }
    return 0;
}

void DecodeAV::run()
{
    int data_size = 0;
    int decode_ret = 0, ret = 0;
    while (true)
    {
        if (isInterruptionRequested() || decode_ret != 0)
            break;
        while (av_read_frame(m_pFormatCtx, packet) >= 0 && decode_ret == 0)
        {
            if (packet->stream_index == videoindex)
            {
                decode_ret = avcodec_send_packet(m_pCodecCtx, packet);
                if (decode_ret < 0)
                {
                    fprintf(stderr, "Error sending a packet for decoding\n");
                    break;
                }
                while (decode_ret >= 0)
                {
                    decode_ret = avcodec_receive_frame(m_pCodecCtx, pFrame);
                    if (decode_ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    {
                        decode_ret = 0;
                        break;
                    }
                    else if (decode_ret < 0)
                    {
                        fprintf(stderr, "Error during decoding\n");
                        break;
                    }
                    else
                    {
                        int height = sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, m_pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
                        //cout << "decode " << height << endl;
                        m_pDirectXRender->drawVideo(pFrameYUV->data[0], m_pCodecCtx->width, height);
                    }
                }
            }
        }
    }
}
