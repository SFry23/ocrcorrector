#include "TextEdit.h"


//------------------------------------------------------------------------------
//  TextEdit::TextEdit()
//------------------------------------------------------------------------------
TextEdit::TextEdit(QWidget* parent) : QTextEdit(parent)
{
}

//------------------------------------------------------------------------------
//  TextEdit::removeBold()
//------------------------------------------------------------------------------
void TextEdit::removeBold()
{
    setHtml(toHtml().replace(" font-weight:600;", ""));
}

//------------------------------------------------------------------------------
//  TextEdit::removeHighlight()
//------------------------------------------------------------------------------
void TextEdit::removeHighlight()
{
    setHtml(toHtml().replace(QRegExp(" background-color[^\"]*"), ""));
}

//------------------------------------------------------------------------------
//  TextEdit::removeItalic()
//------------------------------------------------------------------------------
void TextEdit::removeItalic()
{
    setHtml(toHtml().replace(QRegExp(" font-style:[\\s]*italic;"), ""));
}

//------------------------------------------------------------------------------
//  TextEdit::removeUnderline()
//------------------------------------------------------------------------------
void TextEdit::removeUnderline()
{
    setHtml(toHtml().replace(QRegExp(" text-decoration:[\\s]*underline;"), ""));
}

//------------------------------------------------------------------------------
//  TextEdit::replaceMeta()
//------------------------------------------------------------------------------
void TextEdit::replaceMeta()
{
    setHtml(this->toHtml().replace(QString("<meta name=\"qrichtext\" content=\"1\" />"),
      QString("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">")));
}

