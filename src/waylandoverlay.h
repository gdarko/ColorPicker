// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2022 Darko Gjorgjijoski <dg@darkog.com>

#ifndef WAYLANDOVERLAY_H
#define WAYLANDOVERLAY_H

#include <QWidget>
#include <QPixmap>
#include <QImage>
#include <QColor>
#include <QPoint>

class WaylandOverlay : public QWidget {
    Q_OBJECT

public:
    explicit WaylandOverlay(const QPixmap& screenshot, QWidget* parent = nullptr);

signals:
    void colorPicked(const QColor& color, const QPoint& pos);
    void cancelled();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    QPixmap m_screenshot;
    QImage m_image;
    QPoint m_cursorPos;
    QColor m_currentColor;
};

#endif // WAYLANDOVERLAY_H
