#include "pointercolor.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMouseEvent>

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
#include <QWindow>
#include <QApplication>
#include <QScreen>
#include <QDesktopServices>
#include <QUrl>

#include "dialogabout.h"
#include "dialogstartupinfo.h"
#include "screengrabber.h"

#if defined(Q_OS_LINUX)
#include <QThread>
#endif

#define HELP_URL "https://github.com/gdarko/ColorPicker"


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
    this->isUnixWayland = ScreenGrabber().isWayland();

    QShortcut *shortcutCpyHex = new QShortcut(QKeySequence("Ctrl+C"), this);
    QObject::connect(shortcutCpyHex,&QShortcut::activated,this,&MainWindow::handleCopyHex);

    QShortcut *shortcutCpyRGB = new QShortcut(QKeySequence("Ctrl+X"), this);
    QObject::connect(shortcutCpyRGB,&QShortcut::activated,this,&MainWindow::handleCopyRgb);

    QShortcut *shortcutPause = new QShortcut(QKeySequence("P"), this);
    QObject::connect(shortcutPause,&QShortcut::activated,this,&MainWindow::handlePause);

    QShortcut *shortcutGrab = new QShortcut(QKeySequence("Ctrl+G"), this);
    QObject::connect(shortcutGrab,&QShortcut::activated,this,&MainWindow::handleGrab);

    QObject::connect(ui->btnExit, SIGNAL(clicked()), this, SLOT(handleExitApp()));
    QObject::connect(ui->btnAbout, SIGNAL(clicked()), this, SLOT(handleLaunchDialogAbout()));
    QObject::connect(ui->btnHelp, SIGNAL(clicked()), this, SLOT(handleLaunchHelpLink()));

    colorNames = this->getColorNameMap();

    if(this->isUnixWayland) {
        const QString waylandInfo = "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9.75pt;\">Press &quot;</span><span style=\" font-size:9.75pt; color:#2c974b;\">Ctrl + G</span><span style=\" font-size:9.75pt;\">&quot; to grab the screen (wayland)</span></p>";
        this->ui->textBrowser->append(waylandInfo);
        this->displayWaylandInfoDialog();
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

    if(this->isUnixWayland){
        if(!screenshot) {
            qInfo() << "Use the 'Ctrl+G' shortcut to grab the current screen";
            return;
        }
    } else {
        screenshot =  QPixmap(ScreenGrabber().grabScreen(screen, ok));
    }

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

void MainWindow::handleGrab() {
    bool ok = true;
    QScreen* screen;

    QPoint globalCursorPos = QCursor::pos();
    screen = QGuiApplication::screenAt(globalCursorPos);
    screenshot =  QPixmap(ScreenGrabber().grabScreen(screen, ok));
}

void MainWindow::handleExitApp()
{
    QApplication::exit();
}

void MainWindow::handleLaunchDialogAbout() {
    DialogAbout*  about = new DialogAbout();
    about->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    about->show();
    about->exec();
}

void MainWindow::handleLaunchHelpLink() {
    QDesktopServices::openUrl(QUrl(HELP_URL));
}

void MainWindow::displayWaylandInfoDialog() {
    DialogStartupInfo * dialog = new DialogStartupInfo();
    dialog->setDialogTitle(QString("Attention!"));
    dialog->setDialogDescription(QString("We detected that your system uses Wayland desktop server which is more restrictive and this alters the way how it works.\n\nWhen using Wayland, you need to manually grab the screen using CTRL+G shortcut while focused on the ColorPicker window before you find/copy/detect the actual color.\n\nOn the other environments our app will grab the screen automatically and you only need to find/copy the color code."));
    dialog->show();
    dialog->exec();
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
