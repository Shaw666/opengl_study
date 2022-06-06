#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "decodeAV.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    std::list<decodeAV*> m_listdecodeAV;
};
#endif // MAINWINDOW_H
