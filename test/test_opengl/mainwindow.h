#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "decodeAV.h"
#include <QtAV/Filter.h>
#include <QtAV/FilterContext.h>
#include <QtAV/Statistics.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using namespace QtAV;

class OSDFilter : public VideoFilter
{
public:
    OSDFilter(QObject *parent = 0);
    bool isSupported(VideoFilterContext::Type ct) const {
        return ct == VideoFilterContext::QtPainter || ct == VideoFilterContext::X11;
    }
protected:
    void process(Statistics* statistics, VideoFrame* frame);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QList<decodeAV*> m_listdecodeAV;
    OSDFilter* m_pOSDFilter;
    Statistics statistics;
};




#endif // MAINWINDOW_H
