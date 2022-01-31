#ifndef POINTERCOLOR_H
#define POINTERCOLOR_H

#include <QRgb>
#include <QColor>
#include <QString>
#include <QCursor>
#include <QVariantMap>


class PointerColor
{
public:
   PointerColor(QPoint &cursor, QRgb &rgb, QVariantMap const *colors);
   ~PointerColor();
    QRgb rgb;
    QPoint cursor;
    QColor * color;
    QString cursorX;
     QVariantMap const * colors;
    QString cursorY;
    QString colorRed;
    QString colorGreen;
    QString colorBlue;
    QString colorHex;
    QString colorRGB;
    QString colorName;

};

#endif // POINTERCOLOR_H
