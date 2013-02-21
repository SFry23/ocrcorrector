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
Corrector::Corrector(QTextDocument* document,
                     QSharedPointer<Dictionary> dicNouns,
                     QSharedPointer<Dictionary> dicNames)
{
    _document = document;
    _dicNouns = dicNouns;
    _dicNames = dicNames;

    _document->setHtml(_document->toHtml().trimmed());

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

    _removeImages();
    _removePageNumber();
    _document->setHtml(_document->toHtml().trimmed());
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
void Corrector::autoReplace()
{
    // For cuneiform
    replaceAll("â€” ", "— ");
    replaceAll("â€”", "— ");
    replaceAll("*", "’");
    replaceAll(QRegExp("[I1]['’]"), "l’");

    // For tesseract
    replaceAll("ﬁ", "fi");
    replaceAll("ﬂ", "fl");

    // Generic
    replaceAll(QRegExp(". [l1][Il1] "), ". Il ");
    replaceAll(QRegExp(". I[I1] "), ". Il ");
    replaceAll(QRegExp(". H "), ". Il ");
    replaceAll(QRegExp("! H "), "! Il ");
    replaceAll(QRegExp("? H "), "? Il ");
    replaceAll(QRegExp("['']"), "’");
    replaceAll(QRegExp("- "), "");
    replaceAll(" A ", " À ");
    replaceAll("\nA", "\nÀ");
    replaceAll("['’]?", "?");
    replaceAll(".['’]", ".");
}

//------------------------------------------------------------------------------
//  Corrector::correct()
//------------------------------------------------------------------------------
QString Corrector::correct(const QString plainText)
{
    QString correctedText = plainText;

    // For cuneiform
    correctedText = correctedText.replace("â€” ", "— ");
    correctedText = correctedText.replace("â€”", "— ");
    correctedText = correctedText.replace("*", "’");
    correctedText = correctedText.replace(QRegExp("[I1]['’]"), "l’");

    // For tesseract
    correctedText = correctedText.replace("ﬁ", "fi");
    correctedText = correctedText.replace("ﬂ", "fl");

    // Generic
    correctedText = correctedText.replace(QRegExp(". [l1][Il1] "), ". Il ");
    correctedText = correctedText.replace(QRegExp(". I[I1] "), ". Il ");
    correctedText = correctedText.replace(QRegExp(". H "), ". Il ");
    correctedText = correctedText.replace(QRegExp("! H "), "! Il ");
    correctedText = correctedText.replace(QRegExp("? H "), "? Il ");
    correctedText = correctedText.replace(QRegExp("['']"), "’");
    correctedText = correctedText.replace(QRegExp("- "), "");
    correctedText = correctedText.replace(" A ", " À ");
    correctedText = correctedText.replace("\nA", "\nÀ");
    correctedText = correctedText.replace("['’]?", "?");
    correctedText = correctedText.replace(".['’]", ".");
    correctedText = correctedText.replace("- ", "");

    return correctedText;
}

//------------------------------------------------------------------------------
//  Corrector::detectErrors()
//------------------------------------------------------------------------------
void Corrector::detectErrors()
{
    if (_document != 0 and _dicNouns != 0)
    {
        QTextCursor cursor = QTextCursor(_document);

        autoReplace();

        do
        {
            cursor.select(QTextCursor::WordUnderCursor);
            QString word = cursor.selectedText();

            if (_dicNouns->search(word.toLower()) != -1)
                continue;
            else if (_dicNames != 0 and _dicNames->search(word) != -1)
                continue;
            else if (_isValidWithApostrophe(word, QList<QSharedPointer<Dictionary> >() << _dicNouns << _dicNames))
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
//  Corrector::getDocument()
//------------------------------------------------------------------------------
QTextDocument* Corrector::getDocument()
{
    return _document;
}

//------------------------------------------------------------------------------
//  Corrector::getHighlightStyle()
//------------------------------------------------------------------------------
int Corrector::getHighlightStyle()
{
    return _highlightStyle;
}

//------------------------------------------------------------------------------
//  Corrector::getNumberCorrectedErrors()
//------------------------------------------------------------------------------
int Corrector::getNumberCorrectedErrors()
{
    return _nCorrectedErrors;
}

//------------------------------------------------------------------------------
//  Corrector::getNumberErrors()
//------------------------------------------------------------------------------
int Corrector::getNumberErrors()
{
    return _nErrors;
}

//------------------------------------------------------------------------------
//  Corrector::isValid()
//------------------------------------------------------------------------------
bool Corrector::isValid(QString str, QList<QSharedPointer<Dictionary> >dics)
{
    if (not str.isEmpty())
    {
        if (str.contains("'") and _isValidWithApostrophe(str, dics))
        {
            return true;
        }

        for (int i = 0; i < dics.size(); i++)
        {
            QSharedPointer<Dictionary> dic = dics[i];

            if (not dic.isNull())
            {
                if (dic->search(str) != -1)
                    return true;
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------
//  Corrector::mergeOCRizedTexts()
//------------------------------------------------------------------------------
QString Corrector::mergeOCRizedTexts(const QString strA, const QString strB, QSharedPointer<Dictionary> dic, QSharedPointer<Dictionary> dicNames)
{
    // Run correction
    Corrector::correct(strA);
    Corrector::correct(strB);

    // Align strings
    QLevenshtein aligner(strA, strB);
    QAlignment alignment = aligner.align();

    QString alignedA = alignment.getStringA();
    QString alignedB = alignment.getStringB();

    qDebug() << alignedA;
    qDebug() << alignedB;

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
                bool wordAExists = isValid(wordA, QList<QSharedPointer<Dictionary> >() << dic << dicNames);
                bool wordBExists = isValid(wordB, QList<QSharedPointer<Dictionary> >() << dic << dicNames);

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
void Corrector::replaceAll(QString before, QString after)
{
    QTextCursor cursor = _document->find(before, 0, QTextDocument::FindCaseSensitively);
    while (not cursor.isNull())
    {
        cursor.insertText(after);

        // Apply format
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, after.size());
        cursor.select(QTextCursor::WordUnderCursor);
        _highlight(cursor, _colors[2]);

        // Find next occurrence
        cursor = _document->find(before, cursor.position(), QTextDocument::FindCaseSensitively);
    }
}

//------------------------------------------------------------------------------
//  Corrector::replaceAll()
//------------------------------------------------------------------------------
void Corrector::replaceAll(QRegExp before, QString after)
{
    QTextCursor cursor = _document->find(before);

    while (not cursor.isNull())
    {
        cursor.insertText(after);

        // Apply format
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, after.size());
        cursor.select(QTextCursor::WordUnderCursor);
        _highlight(cursor, _colors[2]);

        // Replace text
        QTextCharFormat format = cursor.charFormat();

        // Find next occurrence
        cursor = _document->find(before, cursor.position(), QTextDocument::FindCaseSensitively);
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

            if (_isValidWithApostrophe(correction, QList<QSharedPointer<Dictionary> >() << _dicNouns << _dicNames))
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
bool Corrector::_isValidWithApostrophe(const QString str,QList<QSharedPointer<Dictionary> >dics)
{
    const QRegExp APOSTROPHE = QRegExp("['’]");

    bool valid = false;
    if (str.count(APOSTROPHE) == 1)
    {
        QStringList parts = str.split(APOSTROPHE);

        if (_APOSTROPHE_PREFIXES.contains(parts[0].toLower()))
        {
            for (int i = 0; i < dics.size(); i++)
            {
                QSharedPointer<Dictionary> dic = dics[i];

                if (not dic.isNull())
                {
                    if (dic->search(parts[1]) != -1)
                        valid = true;
                }
            }
        }
    }

    return valid;
}

//------------------------------------------------------------------------------
//  Corrector::_removeImages()
//------------------------------------------------------------------------------
void Corrector::_removeImages()
{
    QString content = _document->toHtml();

    content.replace(QRegExp("<img [^>]* />"), "");

    _document->setHtml(content);
}

//------------------------------------------------------------------------------
//  Corrector::_removePageNumber()
//------------------------------------------------------------------------------
void Corrector::_removePageNumber()
{
    QTextCursor cursor = _document->find(QRegExp("^[ ]*[0-9]{1,4}[ ]*$"));

    if (not cursor.isNull())
        cursor.removeSelectedText();
}

//------------------------------------------------------------------------------
//  Corrector::_stripPunctuation()
//------------------------------------------------------------------------------
QString Corrector::_stripPunctuation(QString str)
{
    while (_SIMPLE_PUNCTUATION.contains(str.right(1)))
    {
        str.truncate(str.size() - 1);
        _nCorrectedErrors++;
        _nErrors++;
    }

    return str;
}


