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
}


void MainWindow::timerEvent(QTimerEvent *event)
{

    QPoint cursor = QCursor::pos();

    if(this->isMinimized()) {
        qInfo() << "Prevented detection when minimized.";
        return;
    }

    if(this->frameGeometry().contains(cursor)) {
        qInfo() << "Prevented detection over the main window.";
        return;
    }

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
    QString colorName = "Not found";
    currentColor = rgbColors->name();
    qInfo() << "Finding color for: " + currentColor.toLower();
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
    QShortcut *shortcut = new QShortcut(QKeySequence("F5"), this);
    QObject::connect(shortcut,&QShortcut::activated,this,&MainWindow::handleCopy);

    colorNames = this->getColorNameMap();
}


void MainWindow::handleCopy()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(currentColor, QClipboard::Clipboard);

    qInfo() << "Color Copied.";

#if defined(Q_OS_LINUX)
    QThread::msleep(1); //workaround for copied text not being available...
#endif
}

QVariantMap * MainWindow::getColorNameMap()
{
    QVariantMap *map = new QVariantMap();

    QFile *file_obj = new QFile(QString::fromStdString("res/colors.json"));
    if (!file_obj->open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open color map.";
        return map;
    }

    QTextStream file_text(file_obj);
    QString json_string;
    json_string = file_text.readAll();
    file_obj->close();

    auto json_doc = QJsonDocument::fromJson(json_string.toUtf8());

    if (json_doc.isNull()) {
        qDebug() << "Failed to create JSON doc.";
        return map;
    }
    if (!json_doc.isObject()) {
        qDebug() << "JSON is not an object.";
        return map;
    }

    QJsonObject json_obj = json_doc.object();

    if (json_obj.isEmpty()) {
        qDebug() << "JSON object is empty.";
        return map;
    }

    *map = json_obj.toVariantMap();
    return map;
}


