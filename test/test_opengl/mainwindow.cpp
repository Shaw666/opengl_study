#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "XVideoWidget.h"
#include "QtAVWidgets/QtAVWidgets.h"


constexpr auto fileName = "/home/xhp/testdata/out_1920.yuv";
//constexpr auto fileName = "/home/xhp/test/QT_ffmpeg/src/myout240x128.yuv";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QtAV::Widgets::registerRenderers();
    QString vid = "opengl";
    int r = 1, c =1;
    //    for (int i=0; i<2; i++) {
    //        for (int j=0; j<2; j++) {

    ////            XVideoWidget* pXVideoWidget = new XVideoWidget(ui->centralwidget);
    ////            decodeAV* pDecodeAV = new decodeAV(fileName, pXVideoWidget);
    ////            m_listdecodeAV.append(pDecodeAV);
    //            ui->gridLayout->addWidget(pXVideoWidget, i, j);
    //        }

    VideoRendererId v = VideoRendererId_Widget;
    if (vid == QLatin1String("gl"))
        v = VideoRendererId_GLWidget2;
    else if (vid == QLatin1String("opengl"))
        v = VideoRendererId_OpenGLWidget;
    else if (vid == QLatin1String("d2d"))
        v = VideoRendererId_Direct2D;
    else if (vid == QLatin1String("gdi"))
        v = VideoRendererId_GDI;
    else if (vid == QLatin1String("x11"))
        v = VideoRendererId_X11;
    else if (vid == QLatin1String("xv"))
        v = VideoRendererId_XV;
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            decodeAV* pDecodeAV = new decodeAV(fileName);
            m_listdecodeAV.append(pDecodeAV);
            VideoRenderer* renderer = VideoRenderer::create(v);
            renderer->widget()->setWindowFlags(renderer->widget()->windowFlags()| Qt::FramelessWindowHint);
            renderer->widget()->setAttribute(Qt::WA_DeleteOnClose);
            pDecodeAV->setRender(renderer);
//            renderer->widget()->resize(w, h);
//            renderer->widget()->move(j*w, i*h);
//            AVPlayer *player = new AVPlayer;
//            player->setRenderer(renderer);
//            connect(player, SIGNAL(started()), SLOT(changeClockType()));
//            players.append(player);
//            if (view)
//                ((QGridLayout*)view->layout())->addWidget(renderer->widget(), i, j);
             ui->gridLayout_2->addWidget(renderer->widget(), i, j);
        }
    }
    m_pOSDFilter = new OSDFilter();
    bool ret = m_pOSDFilter->installTo(m_listdecodeAV.first()->render());

    //    AVCodecContext *avctx = demuxer.videoCodecContext();
        statistics.video = Statistics::Common();
        statistics.video_only = Statistics::VideoOnly();
    //    if (!avctx)
    //        return;
        statistics.video.available = true;
    //    initCommonStatistics(s, &statistics.video, avctx);
    //    if (vdec) {
    //        statistics.video.decoder = vdec->name();
    //        statistics.video.decoder_detail = vdec->description();
    //    }
        statistics.video_only.coded_height = 1920;
        statistics.video_only.coded_width = 1080;
        statistics.video_only.gop_size = 50;
    //    statistics.video_only.pix_fmt = QLatin1String(av_get_pix_fmt_name(avctx->pix_fmt));
        statistics.video_only.height = 1920;
        statistics.video_only.width = 1080;
        statistics.video_only.rotate = 0;
    //#if AV_MODULE_CHECK(LIBAVFORMAT, 55, 18, 0, 39, 100)
    //    quint8 *sd = av_stream_get_side_data(demuxer.formatContext()->streams[s], AV_PKT_DATA_DISPLAYMATRIX, NULL);
    //    if (sd) {
    //        double r = av_display_rotation_get((qint32*)sd);
    //        if (!qIsNaN(r))
    //            statistics.video_only.rotate = ((int)r + 360) % 360;
    //    }
    //#endif
        m_listdecodeAV.first()->render()->setTestStatistics(&statistics);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
//    m_listdecodeAV.first()->start();
    for (auto var : m_listdecodeAV) {
//        var->m_pXVideoWidget->Init(VWidth, VHeight);
        var->start();
    }
}


void MainWindow::on_pushButton_2_clicked()
{

//    m_pOSDFilter->context()->drawPlainText(QPointF(10,10), "test");
}

OSDFilter::OSDFilter(QObject *parent)
    : VideoFilter(parent)
{}

void OSDFilter::process(Statistics *statistics, VideoFrame *frame)
{
    Q_UNUSED(frame);
    //qDebug("ctx=%p tid=%p main tid=%p", ctx, QThread::currentThread(), qApp->thread());
    context()->drawPlainText(context()->rect, Qt::AlignCenter, "text(statistics)");
}


