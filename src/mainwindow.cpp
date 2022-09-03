

#include "pointercolor.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMouseEvent>
#include <QTextEdit>

#include <QDebug>
#include <QCursor>
#include <QPixmap>
#include <QColor>
#include <QShortcut>
#include <QClipboard>
#include <QPalette>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QWindow>
#include <QApplication>
#include <QScreen>
#include <QVariantMap>
#include <QDesktopServices>
#include <QUrl>

#include "dialogabout.h"
#include "screengrabber.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(QSize(500, 340));
    timerId = startTimer(80);
    this->bootStrap();
}

MainWindow::~MainWindow()
{
    killTimer(timerId);
    if(ui) {
         delete ui;
    }
    if(colorNames) {
        delete this->colorNames;
    }
    if(current) {
        delete this->current;
    }
}

void MainWindow::bootStrap()
{
    this->isPaused = false;

    QShortcut *shortcutCpyHex = new QShortcut(QKeySequence("Ctrl+C"), this);
    QObject::connect(shortcutCpyHex,&QShortcut::activated,this,&MainWindow::handleCopyHex);

    QShortcut *shortcutCpyRGB = new QShortcut(QKeySequence("Ctrl+X"), this);
    QObject::connect(shortcutCpyRGB,&QShortcut::activated,this,&MainWindow::handleCopyRgb);

    QShortcut *shortcutPause = new QShortcut(QKeySequence("P"), this);
    QObject::connect(shortcutPause,&QShortcut::activated,this,&MainWindow::handlePause);

    QObject::connect(ui->btnExit, SIGNAL(clicked()), this, SLOT(exitApp()));
    QObject::connect(ui->btnAbout, SIGNAL(clicked()), this, SLOT(launchDialogAbout()));
    QObject::connect(ui->btnHelp, SIGNAL(clicked()), this, SLOT(launchHelpLink()));


    colorNames = this->getColorNameMap();
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
        qInfo() << "Prevented detection: Cursor in main window.";
        return;
    }

    if(mousePointx && mousePointy ) {
        if(mousePointx == cursor.x() && mousePointy == cursor.y()) {
            //qInfo() << "Prevented detection: Cursor idle.";
            return;
        }
    }

    bool ok = true;
    QScreen* screen;

    QPoint globalCursorPos = QCursor::pos();
    screen = QGuiApplication::screenAt(globalCursorPos);
    screenshot =  QPixmap(ScreenGrabber().grabScreen(screen, ok));

    if(!ok) {
        qInfo() << "Unable to grab screen.";
        return;

    }

    qreal pixelRatio = screen->devicePixelRatio();

    mousePointx = cursor.x() * pixelRatio;
    mousePointy = cursor.y() * pixelRatio;

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

void MainWindow::exitApp()
{
    QApplication::exit();
}

void MainWindow::launchDialogAbout() {
    DialogAbout*  about = new DialogAbout();
    about->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    about->show();
    about->exec();
}

void MainWindow::launchHelpLink() {
    QDesktopServices::openUrl(QUrl("https://github.com/gdarko/ColorPicker"));
}

QVariantMap * MainWindow::getColorNameMap()
{
    QVariantMap *map = new QVariantMap();

    QFile *file_obj = new QFile(QString::fromStdString(":/colorpicker/data/colors.json"));
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
