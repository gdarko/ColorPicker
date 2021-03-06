#include "pointercolor.h"
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
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(QSize(500, 340));
    timerId = startTimer(50);
    this->bootStrap();
}

MainWindow::~MainWindow()
{
    killTimer(timerId);
    delete ui;
    if(colorNames) {
        delete this->colorNames;
    }
    if(current) {
        delete this->current;
    }
}


void MainWindow::timerEvent(QTimerEvent *event)
{

    QPoint cursor = QCursor::pos();

    if(this->isPaused) {
        qInfo() << "Prevented detection: Paused.";
        return;
    }

    if(this->isMinimized()) {
        qInfo() << "Prevented detection: Minimized.";
        return;
    }

    if(this->frameGeometry().contains(cursor)) {
        qInfo() << "Prevented detection: Cusor in main window.";
        return;
    }

    if(mousePointx && mousePointy ) {
        if(mousePointx == cursor.x() && mousePointy == cursor.y()) {
            qInfo() << "Prevented detection: Cursor idle.";
            return;
        }
    }

    mousePointx = cursor.x();
    mousePointy = cursor.y();

    screenshot = this->screen()->grabWindow(0);

    QRgb rgbValue = screenshot.toImage().pixel(mousePointx, mousePointy);

    this->current = new PointerColor(cursor, rgbValue, this->colorNames);

    ui->posX->document()->setPlainText(this->current->cursorX);
    ui->posY->document()->setPlainText(this->current->cursorY);
    ui->numR->document()->setPlainText(this->current->colorRed);
    ui->numG->document()->setPlainText(this->current->colorGreen);
    ui->numB->document()->setPlainText(this->current->colorBlue);
    ui->hexCode->document()->setPlainText(this->current->colorHex);
    ui->colorName->document()->setPlainText(this->current->colorName);

    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, rgbValue);
    ui->colorBox->setPalette(pal);
    ui->colorBox->setAutoFillBackground(true);
    ui->colorBox->show();

}


void MainWindow::bootStrap()
{
    this->isPaused = false;

    QShortcut *shortcutF5 = new QShortcut(QKeySequence("F5"), this);
    QObject::connect(shortcutF5,&QShortcut::activated,this,&MainWindow::handleCopyHex);

    QShortcut *shortcutF6 = new QShortcut(QKeySequence("F6"), this);
    QObject::connect(shortcutF6,&QShortcut::activated,this,&MainWindow::handleCopyRgb);

    QShortcut *shortcutF7 = new QShortcut(QKeySequence("F7"), this);
    QObject::connect(shortcutF7,&QShortcut::activated,this,&MainWindow::handlePause);

    colorNames = this->getColorNameMap();
}


void MainWindow::handleCopyHex()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(this->current->colorHex, QClipboard::Clipboard);

    qInfo() << "Clipboard: Color HexCode Copied.";

#if defined(Q_OS_LINUX)
    QThread::msleep(1); //workaround for copied text not being available...
#endif
}

void MainWindow::handleCopyRgb()
{
    // TODO: Implement.
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(this->current->colorRGB, QClipboard::Clipboard);

    qInfo() << "Clipboard: Color RGB Code Copied.";

#if defined(Q_OS_LINUX)
    QThread::msleep(1); //workaround for copied text not being available...
#endif
}

void MainWindow::handlePause()
{
    this->isPaused = !this->isPaused;
    qInfo() << QString("State:%1").arg( this->isPaused ? QString("Paused") : QString("Detecting"));

}

QVariantMap * MainWindow::getColorNameMap()
{
    QVariantMap *map = new QVariantMap();

    QFile *file_obj = new QFile(QString::fromStdString("res/colors.json"));
    if (!file_obj->open(QIODevice::ReadOnly)) {
        qDebug() << "Color Map: Failed to open json.";
        return map;
    }

    QTextStream file_text(file_obj);
    QString json_string;
    json_string = file_text.readAll();
    file_obj->close();

    auto json_doc = QJsonDocument::fromJson(json_string.toUtf8());

    if (json_doc.isNull()) {
        qDebug() << "Color Map: Failed to create JSON doc.";
        return map;
    }
    if (!json_doc.isObject()) {
        qDebug() << "Color Map: JSON is not an object.";
        return map;
    }

    QJsonObject json_obj = json_doc.object();

    if (json_obj.isEmpty()) {
        qDebug() << "Color Map: JSON object is empty.";
        return map;
    }

    *map = json_obj.toVariantMap();
    return map;
}


