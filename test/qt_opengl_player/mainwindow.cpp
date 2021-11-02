#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_pDecodeAV = new DecodeAV();
    if (m_pDecodeAV->init((void*)ui->widget_4->winId()) == 0) {
        m_pDecodeAV->start();
    }

}

MainWindow::~MainWindow()
{
    m_pDecodeAV->requestInterruption();
    m_pDecodeAV->wait();
    delete m_pDecodeAV;
    delete ui;
}
