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
        delete colorNames;
    }
}


void MainWindow::timerEvent(QTimerEvent *event)
{

    QPoint cursor = QCursor::pos();

    if(this->isFrozen) {
        qInfo() << "Prevented detection: Frozen.";
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

    QString mousePointXStr = QString::fromStdString(std::to_string(mousePointx));
    QString mousePointYStr = QString::fromStdString(std::to_string(mousePointy));

    ui->posX->document()->setPlainText(mousePointXStr);
    ui->posY->document()->setPlainText(mousePointYStr);

    QRgb rgbValue = screenshot.toImage().pixel(mousePointx, mousePointy);

    QColor * rgbColors = new QColor(rgbValue);

    QString cR = QString::fromStdString(std::to_string(rgbColors->red()));
    QString cG = QString::fromStdString(std::to_string(rgbColors->green()));
    QString cB = QString::fromStdString(std::to_string(rgbColors->blue()));
    QString colorName = "Not found";
    currentColor = rgbColors->name();
    if(colorNames->count() > 0) {
        colorName = colorNames->contains(currentColor.toLower()) ? colorNames->find(currentColor.toLower()).value().toString() : colorName;
    }

    ui->numR->document()->setPlainText(cR);
    ui->numG->document()->setPlainText(cG);
    ui->numB->document()->setPlainText(cB);
    ui->colorName->document()->setPlainText(colorName);
    ui->hexCode->document()->setPlainText(currentColor);


    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, rgbValue);
    ui->colorBox->setPalette(pal);
    ui->colorBox->setAutoFillBackground(true);
    ui->colorBox->show();

}


void MainWindow::bootStrap()
{
    this->isFrozen = false;

    QShortcut *shortcutF5 = new QShortcut(QKeySequence("F5"), this);
    QObject::connect(shortcutF5,&QShortcut::activated,this,&MainWindow::handleCopyHex);

    QShortcut *shortcutF6 = new QShortcut(QKeySequence("F6"), this);
    QObject::connect(shortcutF6,&QShortcut::activated,this,&MainWindow::handleCopyRgb);

    QShortcut *shortcutF7 = new QShortcut(QKeySequence("F7"), this);
    QObject::connect(shortcutF7,&QShortcut::activated,this,&MainWindow::handleFreeze);

    colorNames = this->getColorNameMap();
}


void MainWindow::handleCopyHex()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(currentColor, QClipboard::Clipboard);

    qInfo() << "Clipboard: Color HexCode Copied.";

#if defined(Q_OS_LINUX)
    QThread::msleep(1); //workaround for copied text not being available...
#endif
}

void MainWindow::handleCopyRgb()
{
    // TODO: Implement.
    QClipboard* clipboard = QApplication::clipboard();
    // clipboard->setText(currentColor, QClipboard::Clipboard);

    qInfo() << "Clipboard: Color RGB Code Copied.";

#if defined(Q_OS_LINUX)
    QThread::msleep(1); //workaround for copied text not being available...
#endif
}

void MainWindow::handleFreeze()
{
    this->isFrozen = !this->isFrozen;
    qInfo() << QString("State:%1").arg( this->isFrozen ? QString("Frozen") : QString("Detecting"));

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


