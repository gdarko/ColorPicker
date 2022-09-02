// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Yuriy Puchkov <yuriy.puchkov@namecheap.com>, 2022 Darko Gjorgjijoski
// Adopted from Flameshoot to support Qt6

#pragma once

#include <QString>

class DesktopInfo
{
public:
    DesktopInfo();

    enum WM
    {
        GNOME,
        KDE,
        OTHER,
        SWAY
    };

    bool waylandDetected();
    WM windowManager();

private:
    QString XDG_CURRENT_DESKTOP;
    QString XDG_SESSION_TYPE;
    QString WAYLAND_DISPLAY;
    QString KDE_FULL_SESSION;
    QString GNOME_DESKTOP_SESSION_ID;
    QString GDMSESSION;
    QString DESKTOP_SESSION;
};
