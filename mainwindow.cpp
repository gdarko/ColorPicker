#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMouseEvent>
#include <QTextEdit>

#include <QLocale>
#include <QDebug>
#include <QCursor>
#include <QPixmap>
#include <QColor>
#include <QBrush>
#include <QShortcut>
#include <QClipboard>
#include <QPalette>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setMouseTracking(true);

    this->setFixedSize(QSize(750, 400));

    timerId = startTimer(50);

    this->bootStrap();

}

MainWindow::~MainWindow()
{
    killTimer(timerId);
    delete ui;
}


void MainWindow::timerEvent(QTimerEvent *event)
{

    QPoint cursor = QCursor::pos();

    if(mousePointx && mousePointy ) {
        if(mousePointx == cursor.x() && mousePointy == cursor.y()) {
            qInfo() << "Nothing changed. Exiting.";
            return;
        }
    }

    mousePointx = cursor.x();
    mousePointy = cursor.y();



    screenshot = this->screen()->grabWindow(0);

    QString mousePointXStr = QString::fromStdString(std::to_string(mousePointx));
    QString mousePointYStr = QString::fromStdString(std::to_string(mousePointy));

    ui->posX->document()->setPlainText(mousePointXStr);
    ui->posY->document()->setPlainText(mousePointYStr);


    QRgb rgbValue = screenshot.toImage().pixel(mousePointx, mousePointy);

    QColor * rgbColors = new QColor(rgbValue);

    QString cR = QString::fromStdString(std::to_string(rgbColors->red()));
    QString cG = QString::fromStdString(std::to_string(rgbColors->green()));
    QString cB = QString::fromStdString(std::to_string(rgbColors->blue()));
    currentColor = rgbColors->name();

    ui->numR->document()->setPlainText(cR);
    ui->numG->document()->setPlainText(cG);
    ui->numB->document()->setPlainText(cB);
    ui->colorName->document()->setPlainText("Not found");
    ui->hexCode->document()->setPlainText(currentColor);


    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, rgbValue);
    ui->colorBox->setPalette(pal);
    ui->colorBox->setAutoFillBackground(true);
    ui->colorBox->show();

}


void MainWindow::bootStrap()
{
    QShortcut *shortcut = new QShortcut(QKeySequence("F5"), this);
    QObject::connect(shortcut,&QShortcut::activated,this,&MainWindow::handleCopy);
}


void MainWindow::handleCopy()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(currentColor, QClipboard::Clipboard);

    qInfo() << "Copied.";

#if defined(Q_OS_LINUX)
    QThread::msleep(1); //workaround for copied text not being available...
#endif
}

