#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QGridLayout>
#include "XOpenGlWidget.h"


int row = 1;
int col = 1;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QGridLayout *pVideoLayout = new QGridLayout(ui->contentWidget);
    for (size_t i = 0; i < row; i++)
    {
        for (size_t j = 0; j < col; j++)
        {
            XOpenGlWidget *player = new XOpenGlWidget(this);
            pVideoLayout->addWidget(player, i, j);
            m_listdecodeAV.push_back(new decodeAV("../../../test_1920x1080420p.yuv", player));
        }
    }

    for (auto &&var : m_listdecodeAV)
    {
        var->start();
    }
}

MainWindow::~MainWindow()
{
    for (auto&& var : m_listdecodeAV)
    {
        var->requestInterruption();
        var->wait();
    }
    delete ui;
}
