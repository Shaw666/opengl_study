#include "decodeAV.h"

decodeAV::decodeAV(QString fileName)
    :QThread()
{

    m_pFile = new QFile(fileName);
    bool ret = m_pFile->open(QIODevice::ReadOnly);

    m_pFrame = new AVFrame();
    m_pFrame->width = VWidth;
    m_pFrame->height = VHeight;
    m_pFrame->data[0] = new uint8_t[VWidth * VHeight];
    m_pFrame->data[1] = new uint8_t[VWidth * VHeight / 4];
    m_pFrame->data[2] = new uint8_t[VWidth * VHeight / 4];

       memset(m_pFrame->data[1], VWidth * VHeight / 4, 0);
       memset(m_pFrame->data[2], VWidth * VHeight / 4, 0);
}

decodeAV::~decodeAV()
{
    m_pFile->deleteLater();
    delete m_pFrame;
}

void decodeAV::setRender(VideoRenderer *v)
{
    m_pXVideoWidget = v;
}

VideoRenderer *decodeAV::render()
{
    return m_pXVideoWidget;
}

void saveAVFrame_YUV_ToFile(AVFrame *pFrame, QFile *m_pFile)
{
    int t_frameWidth = pFrame->width;
    int t_frameHeight = pFrame->height;
    int t_yPerRowBytes = pFrame->linesize[0];
    int t_uPerRowBytes = pFrame->linesize[1];
    int t_vPerRowBytes = pFrame->linesize[2];
    qDebug() << "saveAVFrame_YUV_ToFile" << t_frameWidth << t_frameHeight << "||" << t_yPerRowBytes << t_uPerRowBytes << t_vPerRowBytes;
    //m_pFile->write((char *)pFrame->data[0],t_frameWidth * t_frameHeight);
    //m_pFile->write((char *)pFrame->data[1],(t_frameWidth/2) * t_frameHeight / 2);
    //m_pFile->write((char *)pFrame->data[2],(t_frameWidth/2) * t_frameHeight / 2);

    for (int i = 0; i < t_frameHeight; i++)
    {
        m_pFile->write((char *)(pFrame->data[0] + i * t_yPerRowBytes), t_frameWidth);
    }

    for (int i = 0; i < t_frameHeight / 2; i++)
    {
        m_pFile->write((char *)(pFrame->data[1] + i * t_uPerRowBytes), t_frameWidth / 2);
    }

    for (int i = 0; i < t_frameHeight / 2; i++)
    {
        m_pFile->write((char *)(pFrame->data[2] + i * t_vPerRowBytes), t_frameWidth / 2);
    }

    m_pFile->flush();
}

int getAVFrame_YUV_FromFile(AVFrame *pFrame, int width, int height, QFile *m_pFile)
{

    pFrame->linesize[0] = width;
    pFrame->linesize[1] = 960;
    pFrame->linesize[2] = 960;
    qint64 len1 = m_pFile->read((char *)pFrame->data[0], width * height);
    qint64 len2 = m_pFile->read((char *)pFrame->data[1], width * height / 4);//width * height / 4;
    qint64 len3 = m_pFile->read((char *)pFrame->data[2], width * height / 4);//width * height / 4;//
    qDebug() << "read yuv:" << len1 << len2 << len3;
    if(len1 == 0 || len2 == 0 ||len3 == 0 ){
        return 0;
    }
    return (len1 + len2 + len3);
}

qreal getDAR(AVFrame *f)
{
    // lavf 54.5.100 av_guess_sample_aspect_ratio: stream.sar > frame.sar
    qreal dar = 0;
    if (f->height > 0)
        dar = (qreal)f->width/(qreal)f->height;
    // prefer sar from AVFrame if sar != 1/1
    //    if (f->sample_aspect_ratio.num > 1)
    //        dar *= av_q2d(f->sample_aspect_ratio);
    //    else if (codec_ctx && codec_ctx->sample_aspect_ratio.num > 1) // skip 1/1
    //        dar *= av_q2d(codec_ctx->sample_aspect_ratio);
    return dar;
}

void decodeAV::run()
{
    while (1)
    {
        if (isInterruptionRequested())
            break;

        if(getAVFrame_YUV_FromFile(m_pFrame, VWidth, VHeight, m_pFile) <= 0)
            break;
        VideoFrame frame(m_pFrame->width, m_pFrame->height, VideoFormat((int)AV_PIX_FMT_YUV420P));
        frame.setDisplayAspectRatio(getDAR(m_pFrame));
        frame.setBits(m_pFrame->data);
        frame.setBytesPerLine(m_pFrame->linesize);
        // in s. TODO: what about AVFrame.pts? av_frame_get_best_effort_timestamp? move to VideoFrame::from(AVFrame*)
        //        frame.setTimestamp((double)m_pFrame->pkt_pts/1000.0);
        //        frame.setMetaData(QStringLiteral("avbuf"), QVariant::fromValue(AVFrameBuffersRef(new AVFrameBuffers(m_pFrame))));
        //        updateColorDetails(&frame);
        //        if (frame.format().hasPalette()) {
        //            frame.setMetaData(QStringLiteral("pallete"), QByteArray((const char*)m_pFrame->data[1], 256*4));
        //        }

        m_pXVideoWidget->receive(frame);
        //        m_pXVideoWidget->Repaint(m_pFrame);
        msleep(40);
    }
}
