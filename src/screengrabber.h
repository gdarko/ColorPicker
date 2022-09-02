// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors, 2022 Darko Gjorgjijoski
// Adopted from Flameshoot to support Qt6

#pragma once

#include "desktopinfo.h"
#include <QObject>
#include <QScreen>

class ScreenGrabber : public QObject
{
    Q_OBJECT
public:
    explicit ScreenGrabber(QObject* parent = nullptr);
    QPixmap grabEntireDesktop(bool& ok);
    QRect screenGeometry(QScreen* screen);
    QPixmap grabScreen(QScreen* screenNumber, bool& ok);
    void freeDesktopPortal(bool& ok, QPixmap& res);
    QRect desktopGeometry();

private:
    DesktopInfo m_info;
};
