// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Yuriy Puchkov <yuriy.puchkov@namecheap.com>, 2022 Darko Gjorgjijoski
// Adopted from Flameshoot to support Qt6

#ifndef QGUIAPPCURRENTSCREEN_H
#define QGUIAPPCURRENTSCREEN_H

#include <QPoint>

class QScreen;

class QGuiAppCurrentScreen
{
public:
    explicit QGuiAppCurrentScreen();
    QScreen* currentScreen();
    QScreen* currentScreen(const QPoint& pos);

private:
    QScreen* screenAt(const QPoint& pos);

    // class members
private:
    QScreen* m_currentScreen;
};

#endif // FLAMESHOT_QGUIAPPCURRENTSCREEN_H
