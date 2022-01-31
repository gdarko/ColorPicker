#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

    QString currentColor;

    Ui::MainWindow *ui;

protected:
    void timerEvent(QTimerEvent *event);
    void handleCopy();
    void bootStrap();
    QVariantMap * getColorNameMap();
    QPixmap screenshot;
    QVariantMap * colorNames;

};
#endif // MAINWINDOW_H
