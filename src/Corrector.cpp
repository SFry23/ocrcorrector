#include "Corrector.h"




// Constants
const QStringList Corrector::_SIMPLE_PUNCTUATION = QStringList() << "." << "…" << ";" << "," << ")" << ":";
const QStringList Corrector::_DOUBLE_PUNCTUATION = QStringList() << "?" << "!" << ":" << "»";
const QStringList Corrector::_APOSTROPHE_PREFIXES = QStringList()
        << "c" << "ç" << "d" << "j" << "l" << "m" << "n" << "qu" << "s" << "t" \
        << "quoiqu" << "lorsqu" << "jusqu" << "puisqu";


//------------------------------------------------------------------------------
//  Corrector::Corrector()
//------------------------------------------------------------------------------
Corrector::Corrector(QSharedPointer<Dictionary> dic, QSharedPointer<Dictionary> names)
{
    _dicNouns = dic;
    _dicNames = names;

    _highlightStyle = 0;

    _colors.append(QColor(Qt::red));
    _colors.append(QColor(Qt::green));
    _colors.append(QColor(Qt::cyan));
}

//------------------------------------------------------------------------------
//  Corrector::~Corrector()
//------------------------------------------------------------------------------
Corrector::~Corrector()
{
}

//------------------------------------------------------------------------------
//  Corrector::autoReplace()
//------------------------------------------------------------------------------
void Corrector::autoReplace(QTextDocument* document)
{
    // Substitutions of simple strings
    QMap<QString, QString> simpleRules;

    simpleRules.insert("â€” ", "— ");
    simpleRules.insert("â€”", "— ");
    simpleRules.insert("*", "’");
    simpleRules.insert("â€” ", "— ");
    simpleRules.insert("A ", "À ");

    // Substitutions with regular expressions
    QMap<QString, QString> regexRules;

    regexRules.insert("[I1]['’]", "l’");
    regexRules.insert("([\\.!\\?]) [l1][Il1] ", "\\1 Il ");
    regexRules.insert("([\\.!\\?]) I[I1] ", "\\1 Il ");
    regexRules.insert("([\\.!\\?]) H(s?) ", "\\1 Il\\2 ");
    regexRules.insert("([eE])He(s?) ", "\\1lle\\2 ");
    regexRules.insert("['']", "’");
    regexRules.insert("- ", "");
    regexRules.insert("['’]\\?", "?");
    regexRules.insert("\\.['’]", ".");


    QMap<QString, QString>::const_iterator it = simpleRules.constBegin();
    while (it != simpleRules.constEnd())
    {
        replaceAll(document, it.key(), it.value());
        ++it;
    }

    it = regexRules.constBegin();
    while (it != regexRules.constEnd())
    {
        replaceAll(document, QRegExp(it.key()), it.value());
        ++it;
    }
}

//------------------------------------------------------------------------------
//  Corrector::correct()
//------------------------------------------------------------------------------
void Corrector::correct(QTextDocument* document)
{
    _removeImages(document);
    _removePageNumber(document);

    if (document != 0 and _dicNouns != 0)
    {
        autoReplace(document);

        QTextCursor cursor = QTextCursor(document);

        do
        {
            cursor.select(QTextCursor::WordUnderCursor);
            QString word = cursor.selectedText();

            if (_dicNouns->search(word.toLower()) != -1)
                continue;
            else if (_dicNames != 0 and _dicNames->search(word) != -1)
                continue;
            else if (_isValidWithApostrophe(word))
                continue;
            else if (word == "—" or _DOUBLE_PUNCTUATION.contains(word))
                continue;
            else if (_correctWord(cursor))
                continue;
            else
            {
                // Highlight errors
                _highlight(cursor, _colors[0]);
            }
        }
        while (cursor.movePosition(QTextCursor::NextWord));
    }
}

//------------------------------------------------------------------------------
//  Corrector::getColors()
//------------------------------------------------------------------------------
QList<QColor> Corrector::getColors()
{
    return _colors;
}

//------------------------------------------------------------------------------
//  Corrector::getHighlightStyle()
//------------------------------------------------------------------------------
int Corrector::getHighlightStyle()
{
    return _highlightStyle;
}

//------------------------------------------------------------------------------
//  Corrector::replaceAll()
//------------------------------------------------------------------------------
void Corrector::replaceAll(QTextDocument* document, QString before, QString after)
{
    QTextCursor cursor = document->find(before, 0, QTextDocument::FindCaseSensitively);
    while (not cursor.isNull())
    {
        cursor.insertText(after);

        // Apply format
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, after.size());
        cursor.select(QTextCursor::WordUnderCursor);
        _highlight(cursor, _colors[2]);

        // Find next occurrence
        cursor = document->find(before, cursor.position(), QTextDocument::FindCaseSensitively);
    }
}

//------------------------------------------------------------------------------
//  Corrector::replaceAll()
//------------------------------------------------------------------------------
void Corrector::replaceAll(QTextDocument* document, QRegExp before, QString after)
{
    QTextCursor cursor = document->find(before);

    while (not cursor.isNull())
    {
        QString str = cursor.selectedText();
        QString newStr = str.replace(before, after);

        QTextCharFormat format = cursor.charFormat();

        if (_highlightStyle == 0)
        {
            format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
            format.setUnderlineColor(_colors[2]);
        }
        else if (_highlightStyle == 1)
        {
            format.setBackground(QBrush(_colors[2]));
        }

        cursor.insertText(newStr, format);

        // Replace text
        format = cursor.charFormat();

        // Find next occurrence
        cursor = document->find(before, cursor.position(), QTextDocument::FindCaseSensitively);
    }
}

//------------------------------------------------------------------------------
//  Corrector::setColors()
//------------------------------------------------------------------------------
void Corrector::setColors(QColor colorError, QColor colorCorrected, QColor colorFormatting)
{
    _colors[0] = colorError;
    _colors[1] = colorCorrected;
    _colors[2] = colorFormatting;
}

//------------------------------------------------------------------------------
//  Corrector::setHighlightStyle()
//------------------------------------------------------------------------------
void Corrector::setHighlightStyle(int highlightStyle)
{
    _highlightStyle = highlightStyle;
}




//------------------------------------------------------------------------------
//  Corrector::_correctWord()
//------------------------------------------------------------------------------
bool Corrector::_correctWord(QTextCursor& cursor)
{
    bool corrected = false;
    QString str = cursor.selectedText();

    if (not str.isNull())
    {
        if (not _isAlphaNum(str) and not _DOUBLE_PUNCTUATION.contains(str))
        {
            cursor.removeSelectedText();
        }
        // Phologramme -> l'hologramme
        else if (str.startsWith("P"))
        {
            QString correction = str.replace(0, 1, "l’");

            if (_isValidWithApostrophe(correction))
            {
                _highlight(cursor, _colors[1]);
                cursor.insertText(correction, cursor.charFormat());
                corrected = true;
            }
        }
    }

    return corrected;
}

//------------------------------------------------------------------------------
//  Corrector::_highlight()
//------------------------------------------------------------------------------
void Corrector::_highlight(QTextCursor& cursor, QColor color)
{
    QTextCharFormat format = cursor.charFormat();

    if (_highlightStyle == 0)
    {
        format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
        format.setUnderlineColor(color);
    }
    else if (_highlightStyle == 1)
    {
        format.setBackground(QBrush(color));
    }

    cursor.setCharFormat(format);
}

//------------------------------------------------------------------------------
//  Corrector::_isValidWithApostrophe()
//------------------------------------------------------------------------------
bool Corrector::_isAlphaNum(const QString& str)
{
    return str.contains(QRegExp("[a-zA-Z0-9]"));
}

//------------------------------------------------------------------------------
//  Corrector::_isValidWithApostrophe()
//------------------------------------------------------------------------------
bool Corrector::_isValidWithApostrophe(const QString& str)
{
    const QRegExp APOSTROPHE = QRegExp("['’]");

    bool valid = false;
    if (str.count(APOSTROPHE) == 1)
    {
        QStringList parts = str.split(APOSTROPHE);

        if (_APOSTROPHE_PREFIXES.contains(parts[0].toLower()))
        {
            if (_dicNouns->search(parts[1]) != -1 or (_dicNames != 0 and _dicNames->search(parts[1]) != -1))
            {
                valid = true;
            }
        }
    }

    return valid;
}

//------------------------------------------------------------------------------
//  Corrector::_removeImages()
//------------------------------------------------------------------------------
void Corrector::_removeImages(QTextDocument* document)
{
    if (document != 0)
    {
        QString content = document->toHtml();
        content.replace(QRegExp("<img [^>]* />"), "");
        document->setHtml(content);
    }
}

//------------------------------------------------------------------------------
//  Corrector::_removePageNumber()
//------------------------------------------------------------------------------
void Corrector::_removePageNumber(QTextDocument* document)
{
    if (document != 0)
    {
        QTextCursor cursor = document->find(QRegExp("^[ ]*[0-9]{1,4}[ ]*$"));

        if (not cursor.isNull())
            cursor.removeSelectedText();
    }
}

//------------------------------------------------------------------------------
//  Corrector::_stripPunctuation()
//------------------------------------------------------------------------------
QString Corrector::_stripPunctuation(QString str)
{
    while (_SIMPLE_PUNCTUATION.contains(str.right(1)))
        str.truncate(str.size() - 1);

    return str;
}


