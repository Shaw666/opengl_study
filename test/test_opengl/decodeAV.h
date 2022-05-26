#pragma once
#include <QThread>
#include "depends.h"
#include <QFile>
#include "XVideoWidget.h"
#include "QtAVWidgets/QtAVWidgets.h"
using namespace QtAV;
class decodeAV : public QThread
{
private:
    /* data */
public:
    decodeAV(QString fileName);
    ~decodeAV();

    void setRender(VideoRenderer* v);
    VideoRenderer* m_pXVideoWidget;
    VideoRenderer* render();
protected:
    void run() override;

private:
    QFile *m_pFile;
    AVFrame* m_pFrame;

};
