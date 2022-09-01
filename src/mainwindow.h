#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "pointercolor.h"
#include <QMainWindow>
#include <QTimerEvent>
#include <QPixmap>
#include <QString>


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
    int mousePointx;
    int mousePointy;
    int timerId;
    bool isPaused;
    Ui::MainWindow *ui;

protected:
    void timerEvent(QTimerEvent *event);
    void handleCopyHex();
    void handleCopyRgb();
    void handlePause();
    void bootStrap();
    QVariantMap * getColorNameMap();
    QPixmap screenshot;
    QVariantMap * colorNames;

    PointerColor * current;

};
#endif // MAINWINDOW_H
