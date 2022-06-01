#pragma once
#include <QThread>
#include <QFile>
#include "XOpenGlWidget.h"
#include <memory>

class decodeAV : public QThread
{
public:
    decodeAV(const QString& fileName, XOpenGlWidget* v);
    ~decodeAV();

    void setRender(XOpenGlWidget* v);
    XOpenGlWidget* m_pXOpenGlWidget;
    XOpenGlWidget* render();
protected:
    void run() override;

private:
    QFile m_pFile;
    std::unique_ptr<char> unipData;
};
