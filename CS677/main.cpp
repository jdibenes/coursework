
#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[]) {
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setSwapInterval(0);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
