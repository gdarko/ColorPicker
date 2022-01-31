#include "pointercolor.h"

#include <QRgb>
#include <QColor>
#include <QString>
#include <QPoint>
#include <QVariantMap>


PointerColor::PointerColor(QPoint &cursor, QRgb &rgb, QVariantMap const *colors)
{

    this->cursor = cursor;
    this->rgb = rgb;
    this->colors = colors;

    this->color = new QColor(rgb);

    this->cursorX = QString::fromStdString(std::to_string(this->cursor.x()));
    this->cursorY = QString::fromStdString(std::to_string(this->cursor.y()));

    this->colorHex = this->color->name().toLower();
    this->colorRed = QString::fromStdString(std::to_string(this->color->red()));
    this->colorGreen = QString::fromStdString(std::to_string(this->color->green()));
    this->colorBlue = QString::fromStdString(std::to_string(this->color->blue()));
    this->colorRGB = QString("rgb(%1,%2,%3)").arg(this->colorRed, this->colorGreen, this->colorBlue);

    this->colorName = "Not found";
    if(this->colors->count() > 0) {
        this->colorName = this->colors->contains(this->colorHex) ? this->colors->find(this->colorHex).value().toString() : this->colorName;
    }


}

PointerColor::~PointerColor()
{
    delete(this->colors);
}
