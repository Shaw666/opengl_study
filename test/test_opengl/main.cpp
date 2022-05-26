#include "mainwindow.h"
#include <QLoggingCategory>
#include <QApplication>

int main(int argc, char *argv[])
{
        QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
