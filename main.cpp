#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <Qt>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QScreen *screen = a.primaryScreen();

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ColorPicker_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);

    w.setScreen(screen);

    w.show();
    return a.exec();
}
