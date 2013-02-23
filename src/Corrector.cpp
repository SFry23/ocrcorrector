#include "Corrector.h"


//------------------------------------------------------------------------------
//  Corrector::Corrector()
//------------------------------------------------------------------------------
Corrector::Corrector(QSharedPointer<Dictionary> dicNouns,
                     QSharedPointer<Dictionary> dicNames)
{
    _dicNouns = dicNouns;
    _dicNames = dicNames;

    _init();
}


//------------------------------------------------------------------------------
//  Corrector::_init()
//------------------------------------------------------------------------------
void Corrector::_init()
{
    _highlightStyle = 0;

    _colors.append(QColor(Qt::red));
    _colors.append(QColor(Qt::green));
    _colors.append(QColor(Qt::cyan));

    // Constants
    _simplePonctuation = QStringList() << "." << "…" << ";" << "," << ")" << ":";
    _doublePonctuation = QStringList() << "?" << "!" << ":" << "»";
    _apostrophePrefixes = QStringList() << "c" << "ç" << "d" << "j" << "l" << "m" << "n" \
                        << "qu" << "s" << "t" << "quoiqu" << "lorsqu" \
                        << "jusqu" << "puisqu";
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
void Corrector::autoReplace(QTextDocument* document, bool highlight)
{
    // Substitutions of simple strings
    QMap<QString, QString> simpleRules;

    simpleRules.insert("â€” ", "— ");
    simpleRules.insert("â€”", "— ");
    simpleRules.insert("*", "’");
    simpleRules.insert("â€” ", "— ");
    simpleRules.insert("A ", "À ");
    simpleRules.insert("oeil", "œil");

    // Substitutions with regular expressions
    QMap<QString, QString> regexRules;

    regexRules.insert("[I1]['’]", "l’");
    regexRules.insert("([\\.!\\?]) [l1][Il1] ", "\\1 Il ");
    regexRules.insert("([\\.!\\?]) I[I1] ", "\\1 Il ");
    regexRules.insert("([\\.!\\?]) H(s?) ", "\\1 Il\\2 ");
    regexRules.insert("^H(s?) ", "Il\\1 ");
    regexRules.insert("([eE])He(s?) ", "\\1lle\\2 ");
    regexRules.insert("['']", "’");
    regexRules.insert("- ", "");
    regexRules.insert("['’]\\?", "?");
    regexRules.insert("\\.['’]", ".");
    regexRules.insert("(\\w)—(\\w)", "\\1-\\2");
    regexRules.insert("([!\\?\\.:]\\s?)— ", "\\1\n— ");
    regexRules.insert("([\\w])([!\\?])", "\\1 \\2");


    QMap<QString, QString>::const_iterator it = simpleRules.constBegin();
    while (it != simpleRules.constEnd())
    {
        replaceAll(document, it.key(), it.value(), highlight);
        ++it;
    }

    it = regexRules.constBegin();
    while (it != regexRules.constEnd())
    {
        replaceAll(document, QRegExp(it.key()), it.value(), highlight);
        ++it;
    }
}

//------------------------------------------------------------------------------
//  Corrector::correct()
//------------------------------------------------------------------------------
void Corrector::correct(QTextDocument* document, bool highlight)
{
    _removeImages(document);
    _removePageNumber(document);
    _removePunctuationInsideWords(document);

    autoReplace(document, highlight);

    if (document != 0 and _dicNouns != 0)
    {
        QTextCursor cursor = QTextCursor(document);

        do
        {
            cursor.select(QTextCursor::WordUnderCursor);
            QString word = cursor.selectedText();

            if (not isValid(word))
            {
                if (highlight)
                {
                    // Highlight errors
                    _highlight(cursor, _colors[0]);
                }
            }
        }
        while (cursor.movePosition(QTextCursor::NextWord));
    }
}

//------------------------------------------------------------------------------
//  Corrector::correct()
//------------------------------------------------------------------------------
QString Corrector::correct(const QString str)
{
    QString correction = str;

    QTextDocument* document = new QTextDocument();

    if (document != 0)
    {
        document->setHtml(correction);

        correct(document, false);

        correction = document->toHtml();

        delete document;
    }

    return correction;
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
//  Corrector::mergeOCRizedTexts()
//------------------------------------------------------------------------------
QString Corrector::mergeOCRizedTexts(const QString strA, const QString strB)
{
    // Run correction
    correct(strA);
    correct(strB);

    // Align strings
    QLevenshtein aligner(strA, strB);
    QAlignment alignment = aligner.align();

    QString alignedA = alignment.getStringA();
    QString alignedB = alignment.getStringB();

    // Merge
    QStringList validWords;

    int wordStart = 0;

    for (int i = 0; i < alignedA.size(); i++)
    {
        if (alignedA[i].isSpace() or alignedB[i].isSpace())
        {
            QString wordA = alignedA.mid(wordStart, i - wordStart).replace("$", "");
            QString wordB = alignedB.mid(wordStart, i - wordStart).replace("$", "");

            if (wordA.trimmed() == wordB.trimmed())
            {
                validWords << wordA;
            }
            else
            {
                bool wordAExists = isValid(wordA);
                bool wordBExists = isValid(wordB);

                if (wordAExists and not wordBExists)
                    validWords << wordA;
                else if (wordBExists and not wordAExists)
                    validWords << wordB;
                else if (wordAExists and wordBExists)
                    validWords << wordA;
                else
                {
                    if (wordA.isEmpty() and wordB.contains(QRegExp("[a-zA-Z0-9]")))
                        validWords << wordB;
                    else if (wordB.isEmpty() and wordA.contains(QRegExp("[a-zA-Z0-9]")))
                        validWords << wordA;
                    else if (not wordA.isEmpty() and not wordB.isEmpty())
                        validWords << wordA;
                }
            }


            wordStart = i + 1;
        }
    }

    return validWords.join(" ");
}

//------------------------------------------------------------------------------
//  Corrector::replaceAll()
//------------------------------------------------------------------------------
void Corrector::replaceAll(QTextDocument* document, QString before, QString after, bool highlight)
{
    QTextCursor cursor = document->find(before, 0, QTextDocument::FindCaseSensitively);
    while (not cursor.isNull())
    {
        QString str = cursor.selectedText();
        QString newStr = str.replace(before, after);

        QTextCharFormat format = cursor.charFormat();

        if (highlight)
            format = _getHighlightFormat(format, _colors[2]);

        cursor.insertText(newStr, format);

        // Find next occurrence
        cursor = document->find(before, cursor, QTextDocument::FindCaseSensitively);
    }
}

//------------------------------------------------------------------------------
//  Corrector::replaceAll()
//------------------------------------------------------------------------------
void Corrector::replaceAll(QTextDocument* document, QRegExp before, QString after, bool highlight)
{
    QTextCursor cursor = document->find(before);

    while (not cursor.isNull())
    {
        QString str = cursor.selectedText();
        QString newStr = str.replace(before, after);

        QTextCharFormat format = cursor.charFormat();

        if (highlight)
            format = _getHighlightFormat(format, _colors[2]);

        cursor.insertText(newStr, format);

        // Find next occurrence
        cursor = document->find(before, cursor);
    }
}

//------------------------------------------------------------------------------
//  Corrector::_getHighlightFormat()
//------------------------------------------------------------------------------
QTextCharFormat Corrector::_getHighlightFormat(const QTextCharFormat format, const QColor color)
{
    QTextCharFormat hlFormat = format;

    if (_highlightStyle == 0)
    {
        hlFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
        hlFormat.setUnderlineColor(color);
    }
    else if (_highlightStyle == 1)
    {
        hlFormat.setBackground(QBrush(color));
    }

    return hlFormat;
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
        if (not _isAlphaNum(str) and not _doublePonctuation.contains(str))
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
void Corrector::_highlight(QTextCursor& cursor, const QColor color)
{
    QTextCharFormat format = cursor.charFormat();

    if (_highlightStyle == 0)
    {
        format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
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
bool Corrector::_isAlphaNum(const QString str)
{
    return str.contains(QRegExp("[a-zA-Z0-9]"));
}

//------------------------------------------------------------------------------
//  Corrector::isValid()
//------------------------------------------------------------------------------
bool Corrector::isValid(const QString str)
{
    if (_dicNouns != 0)
    {
        if (_dicNouns->search(str.toLower()) != -1)
            return true;
        else if (_dicNames != 0 and _dicNames->search(str) != -1)
            return true;
        else if (_isValidWithApostrophe(str))
            return true;
        else if (str == "—" or _doublePonctuation.contains(str))
            return true;
    }

    return false;
}

//------------------------------------------------------------------------------
//  Corrector::_isValidWithApostrophe()
//------------------------------------------------------------------------------
bool Corrector::_isValidWithApostrophe(const QString str)
{
    const QRegExp APOSTROPHE = QRegExp("['’]");

    bool valid = false;
    if (str.count(APOSTROPHE) == 1)
    {
        QStringList parts = str.split(APOSTROPHE);

        if (_apostrophePrefixes.contains(parts[0], Qt::CaseInsensitive))
        {
            if (_dicNouns != 0 and _dicNouns->search(parts[1]) != -1)
                valid = true;
            else if (_dicNames != 0 and _dicNames->search(parts[1]) != -1)
                valid = true;
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
//  Corrector::_removePageNumber()
//------------------------------------------------------------------------------
void Corrector::_removePunctuationInsideWords(QTextDocument* document, bool highlight)
{
    QRegExp toFind = QRegExp("(\\w)[!\\?\\.\\,](\\w)");

    QTextCursor cursor = document->find(toFind, 0, QTextDocument::FindCaseSensitively);
    while (not cursor.isNull())
    {
        QString str = cursor.selectedText();
        QString newStr = str.replace(toFind, "\\1\\2");

        if (isValid(newStr))
        {
            QTextCharFormat format = cursor.charFormat();

            if (highlight)
            {
                if (_highlightStyle == 0)
                {
                    format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
                    format.setUnderlineColor(_colors[1]);
                }
                else if (_highlightStyle == 1)
                {
                    format.setBackground(QBrush(_colors[1]));
                }
            }

            cursor.insertText(newStr, format);
        }

        // Find next occurrence
        cursor = document->find(toFind, cursor.position(), QTextDocument::FindCaseSensitively);
    }
}

