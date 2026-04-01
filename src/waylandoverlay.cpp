// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2022 Darko Gjorgjijoski <dg@darkog.com>

#include "waylandoverlay.h"

#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QScreen>
#include <QGuiApplication>

WaylandOverlay::WaylandOverlay(const QPixmap& screenshot, QWidget* parent)
    : QWidget(parent, Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint)
    , m_screenshot(screenshot)
    , m_image(screenshot.toImage())
{
    setMouseTracking(true);
    setCursor(Qt::CrossCursor);
    setFocusPolicy(Qt::StrongFocus);

    // Cover the full virtual desktop
    QRect fullGeometry;
    for (QScreen* screen : QGuiApplication::screens()) {
        fullGeometry = fullGeometry.united(screen->geometry());
    }
    setGeometry(fullGeometry);
}

void WaylandOverlay::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter painter(this);

    // Draw the screenshot as background
    painter.drawPixmap(rect(), m_screenshot);

    if (!m_currentColor.isValid())
        return;

    // Position the info panel near the cursor, offset to bottom-right
    int panelX = m_cursorPos.x() + 20;
    int panelY = m_cursorPos.y() + 20;
    int panelW = 140;
    int panelH = 50;

    // Flip to other side if it would go off-screen
    if (panelX + panelW > width())
        panelX = m_cursorPos.x() - panelW - 20;
    if (panelY + panelH > height())
        panelY = m_cursorPos.y() - panelH - 20;

    QRect panelRect(panelX, panelY, panelW, panelH);

    // Panel background
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 180));
    painter.drawRoundedRect(panelRect, 6, 6);

    // Color swatch
    QRect swatchRect(panelX + 8, panelY + 8, 34, 34);
    painter.setPen(QPen(Qt::white, 1));
    painter.setBrush(m_currentColor);
    painter.drawRect(swatchRect);

    // Hex label
    painter.setPen(Qt::white);
    painter.setFont(QFont(QStringLiteral("monospace"), 12, QFont::Bold));
    painter.drawText(QRect(panelX + 50, panelY + 4, panelW - 58, 22),
                     Qt::AlignLeft | Qt::AlignVCenter,
                     m_currentColor.name().toUpper());

    // RGB label
    QString rgb = QString("R:%1 G:%2 B:%3")
                      .arg(m_currentColor.red())
                      .arg(m_currentColor.green())
                      .arg(m_currentColor.blue());
    painter.setFont(QFont(QStringLiteral("monospace"), 9));
    painter.drawText(QRect(panelX + 50, panelY + 26, panelW - 58, 18),
                     Qt::AlignLeft | Qt::AlignVCenter, rgb);

    // Crosshair at cursor
    painter.setPen(QPen(Qt::white, 1));
    painter.drawLine(m_cursorPos.x() - 10, m_cursorPos.y(),
                     m_cursorPos.x() + 10, m_cursorPos.y());
    painter.drawLine(m_cursorPos.x(), m_cursorPos.y() - 10,
                     m_cursorPos.x(), m_cursorPos.y() + 10);
}

void WaylandOverlay::mouseMoveEvent(QMouseEvent* event)
{
    m_cursorPos = event->pos();

    // Scale by device pixel ratio: event->pos() returns logical coordinates
    // but QImage pixel access needs physical pixel coordinates when DPR > 1
    qreal dpr = m_image.devicePixelRatio();
    int imgX = qRound(m_cursorPos.x() * dpr);
    int imgY = qRound(m_cursorPos.y() * dpr);

    if (imgX >= 0 && imgX < m_image.width() &&
        imgY >= 0 && imgY < m_image.height()) {
        m_currentColor = m_image.pixelColor(imgX, imgY);
    }

    update();
}

void WaylandOverlay::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_currentColor.isValid()) {
        emit colorPicked(m_currentColor, m_cursorPos);
        close();
    } else if (event->button() == Qt::RightButton) {
        emit cancelled();
        close();
    }
}

void WaylandOverlay::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        emit cancelled();
        close();
    }
}
