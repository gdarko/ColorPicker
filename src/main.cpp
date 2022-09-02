#include "mainwindow.h"

#include <QApplication>
#include <Qt>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QScreen *screen = a.primaryScreen();

    MainWindow w;
    w.setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    w.setScreen(screen);

    w.show();

    return a.exec();
}
