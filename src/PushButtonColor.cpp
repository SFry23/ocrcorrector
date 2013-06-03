#include "PushButtonColor.h"



//------------------------------------------------------------------------------
//  PushButtonColor::PushButtonColor()
//------------------------------------------------------------------------------
PushButtonColor::PushButtonColor(QWidget* parent) : QPushButton(parent)
{
    connect(this, SIGNAL(clicked()), this, SLOT(reemitClicked()));
}

//------------------------------------------------------------------------------
//  PushButtonColor::PushButtonColor()
//------------------------------------------------------------------------------
PushButtonColor::PushButtonColor(const QColor& color, QWidget* parent) : QPushButton(parent)
{
    _color = color;
    connect(this, SIGNAL(clicked()), this, SLOT(reemitClicked()));
}

//------------------------------------------------------------------------------
//  PushButtonColor::colorize()
//------------------------------------------------------------------------------
void PushButtonColor::colorize(const QColor color)
{
    setColor(color);
    QString rgbColor = QString("rgb(%1, %2, %3)").arg(_color.red()).arg(_color.green()).arg(_color.blue());
    setStyleSheet(QString("border: none; background-color: %1;").arg(rgbColor));
}

//------------------------------------------------------------------------------
//  PushButtonColor::getColor()
//------------------------------------------------------------------------------
QColor PushButtonColor::getColor()
{
    return _color;
}

//------------------------------------------------------------------------------
//  PushButtonColor::getId()
//------------------------------------------------------------------------------
int PushButtonColor::getId()
{
    return _id;
}

//------------------------------------------------------------------------------
//  PushButtonColor::reemitClicked()
//------------------------------------------------------------------------------
void PushButtonColor::reemitClicked()
{
    emit clicked(_color);
}

//------------------------------------------------------------------------------
//  PushButtonColor::setColor()
//------------------------------------------------------------------------------
void PushButtonColor::setColor(const QColor color)
{
    if (color.isValid())
        _color = color;
}

//------------------------------------------------------------------------------
//  PushButtonColor::setId()
//------------------------------------------------------------------------------
void PushButtonColor::setId(const int id)
{
    _id = id;
}

