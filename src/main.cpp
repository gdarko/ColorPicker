// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2022 Darko Gjorgjijoski <dg@darkog.com>

#include "mainwindow.h"

#include <QApplication>
#include <Qt>
#include <QScreen>
#include <cstdlib>
#include <cstring>

int main(int argc, char *argv[])
{
    // On Wayland sessions, force XWayland backend for reliable global
    // cursor position and per-pixel screen capture access.
    const char* sessionType = getenv("XDG_SESSION_TYPE");
    const char* waylandDisplay = getenv("WAYLAND_DISPLAY");
    if ((sessionType && strcmp(sessionType, "wayland") == 0) ||
        (waylandDisplay && strstr(waylandDisplay, "wayland") != nullptr)) {
        setenv("QT_QPA_PLATFORM", "xcb", 1);
    }

    QApplication a(argc, argv);

    QScreen *screen = a.primaryScreen();

    MainWindow w;
    w.setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    w.setScreen(screen);

    w.show();

    return a.exec();
}
