#include "TextDocument.h"



//------------------------------------------------------------------------------
//  TextDocument::TextDocument()
//------------------------------------------------------------------------------
TextDocument::TextDocument() : QFile()
{
    _ocrDocument = new QTextDocument(this);
    _currentDocument = new QTextDocument(this);
    _saved = true;
}

//------------------------------------------------------------------------------
//  TextDocument::TextDocument()
//------------------------------------------------------------------------------
TextDocument::TextDocument(QString filename) : QFile(filename)
{
    _ocrDocument = new QTextDocument(this);
    _currentDocument = new QTextDocument(this);
    _saved = true;
}

//------------------------------------------------------------------------------
//  TextDocument::TextDocument()
//------------------------------------------------------------------------------
TextDocument::TextDocument(QFile* pFile) : QFile(pFile)
{
    _ocrDocument = new QTextDocument(this);
    _currentDocument = new QTextDocument(this);
    _saved = true;
}

//------------------------------------------------------------------------------
//  TextDocument::~TextDocument()
//------------------------------------------------------------------------------
TextDocument::~TextDocument()
{
}

//------------------------------------------------------------------------------
//  TextDocument::getOcrDocument()
//------------------------------------------------------------------------------
QTextDocument* TextDocument::getOcrDocument()
{
    return _ocrDocument;
}

//------------------------------------------------------------------------------
//  TextDocument::getOcrText()
//------------------------------------------------------------------------------
QString TextDocument::getOcrText()
{
    if (_ocrDocument != 0)
        return _ocrDocument->toHtml();
    else
        return QString();
}

//------------------------------------------------------------------------------
//  TextDocument::getCurrentDocument()
//------------------------------------------------------------------------------
QTextDocument* TextDocument::getCurrentDocument()
{
    return _currentDocument;
}

//------------------------------------------------------------------------------
//  TextDocument::getCurrentText()
//------------------------------------------------------------------------------
QString TextDocument::getCurrentText()
{
    if (_currentDocument != 0)
        return _currentDocument->toHtml();
    else
        return QString();
}

//------------------------------------------------------------------------------
//  TextDocument::isSaved()
//------------------------------------------------------------------------------
bool TextDocument::isSaved()
{
    return _saved;
}


//------------------------------------------------------------------------------
//  TextDocument::loadContent()
//------------------------------------------------------------------------------
void TextDocument::loadContent()
{
    if (open(QIODevice::ReadOnly))
    {
        _currentDocument->setHtml((QTextStream(this).readAll()));
        close();
    }
}


//------------------------------------------------------------------------------
//  TextDocument::setCurrentDocument()
//------------------------------------------------------------------------------
void TextDocument::setCurrentDocument(QTextDocument* document)
{
    _currentDocument = document;
}

//------------------------------------------------------------------------------
//  TextDocument::setCurrentText()
//------------------------------------------------------------------------------
void TextDocument::setCurrentText(QString str)
{
    _currentDocument->setHtml(str);
}

//------------------------------------------------------------------------------
//  TextDocument::setOcrDocument()
//------------------------------------------------------------------------------
void TextDocument::setOcrDocument(QTextDocument* document)
{
    _ocrDocument = document;
}

//------------------------------------------------------------------------------
//  TextDocument::setOcrText()
//------------------------------------------------------------------------------
void TextDocument::setOcrText(QString str)
{
    _ocrDocument->setHtml(str);
}

//------------------------------------------------------------------------------
//  TextDocument::setSaved()
//------------------------------------------------------------------------------
void TextDocument::setSaved(bool saved)
{
    _saved = saved;
}


