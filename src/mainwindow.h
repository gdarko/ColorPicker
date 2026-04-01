// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2022 Darko Gjorgjijoski <dg@darkog.com>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "pointercolor.h"
#include <QMainWindow>
#include <QTimerEvent>
#include <QPixmap>
#include <QImage>
#include <QString>
#include <QVariantMap>

class WaylandOverlay;


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
    bool isUnixWayland;
    bool m_waylandHasFallbackTools = false;
    bool m_waylandInitialCaptureFailed = false;
    int m_waylandRefreshCounter = 0;
    QPoint m_lastRefreshCursorPos;
    static const int WAYLAND_REFRESH_TICKS = 37; // ~3 seconds (37 * 80ms)
    QImage m_cachedImage;
    bool m_screenshotDirty = true;
    Ui::MainWindow *ui;

private slots:
    void handleExitApp();
    void handleLaunchDialogAbout();
    void handleLaunchHelpLink();
    void displayWaylandInfoDialog();

protected:
    void timerEvent(QTimerEvent *event);
    void handleCopyHex();
    void handleCopyRgb();
    void handlePause();
    void handleGrab();
    void bootStrap();
    void waylandAutoRefresh();
    void showWaylandOverlay();
    void onWaylandColorPicked(const QColor& color, const QPoint& pos);
    QVariantMap * getColorNameMap();
    QPixmap screenshot;
    QVariantMap * colorNames;
    WaylandOverlay* m_waylandOverlay = nullptr;

    PointerColor * current;
};
#endif // MAINWINDOW_H
