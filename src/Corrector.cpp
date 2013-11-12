#include "Corrector.h"



//------------------------------------------------------------------------------
//  Corrector::Corrector()
//------------------------------------------------------------------------------
Corrector::Corrector(DictionaryPtr dicNouns, DictionaryPtr dicNames)
{
    _dicNouns = dicNouns;
    _dicNames = dicNames;

    _highlightStyle = 0;

    _colors.append(QColor(Qt::red));
    _colors.append(QColor(Qt::green));
    _colors.append(QColor(Qt::cyan));

    // Constants
    _simplePonctuation = QStringList() << "." << "…" << ";" << "," << ")" << ":";
    _doublePonctuation = QStringList() << "?" << "!" << ":" << "»";
    _apostrophePrefixes = QStringList() << "c" << "ç" <<"Ç" << "d" << "j" << "l" << "m" << "n" \
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
    QMap<QString, QString> simpleRules = createSimpleReplacementRules();
    QMap<QString, QString> regexRules = createRegexReplacementRules();

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
    if (document != 0)
    {
        _removeImages(document);
        _removePageNumber(document);
        _removePunctuationInsideWords(document);

        autoReplace(document, highlight);

        QTextCursor cursor = QTextCursor(document);

        do
        {
            cursor.select(QTextCursor::WordUnderCursor);
            QString word = cursor.selectedText();

            if (not isValid(word))
            {
                // Word considered as correct if present at least twice in the document
                if (word.contains(QRegExp("^[A-ZÉÈÊÎÔÛ]")))
                {
                    if (document->toHtml().count(word[0]) >= 2)
                        continue;
                }

                //~ if (word.size() == 1)
                    //~ cursor.removeSelectedText();

                if (cursor.selectionStart() != 0 and not _correctWord(cursor))
                {
                    if (highlight)
                    {
                        // Highlight errors
                        _highlight(cursor, _colors[0]);
                    }
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
    }

    delete document;

    return correction;
}

//------------------------------------------------------------------------------
//  Corrector::createRegexReplacementRules()
//------------------------------------------------------------------------------
QMap<QString, QString> Corrector::createRegexReplacementRules()
{
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
    regexRules.insert("at-(elle|il)", "a-t-\\1");

    return regexRules;
}

//------------------------------------------------------------------------------
//  Corrector::createSimpleReplacementRules()
//------------------------------------------------------------------------------
QMap<QString, QString> Corrector::createSimpleReplacementRules()
{
    // Substitutions of simple strings
    QMap<QString, QString> simpleRules;

    simpleRules.insert("â€” ", "— ");
    simpleRules.insert("â€”", "— ");
    simpleRules.insert("*", "’");
    simpleRules.insert("â€” ", "— ");
    simpleRules.insert("A ", "À ");
    simpleRules.insert("oeil", "œil");
    simpleRules.insert("ﬁ", "fi");
    simpleRules.insert("ﬂ", "fl");

    return simpleRules;
}

//------------------------------------------------------------------------------
//  Corrector::findSimilarWord()
//------------------------------------------------------------------------------
QString Corrector::findSimilarWord(const QString str)
{
    QString result = QString();

    if (str.size() > 3)
    {
        QStringList words;
        double treshold = 0.70;
        double max = 0;

        words = retrieveCandidates(str);

        for (int i = 0; i < words.size(); i++)
        {
            QLevenshtein aligner = QLevenshtein(str, words[i]);
            QAlignment alignment = aligner.align();

            if (alignment.getScore() / words[i].size() > treshold)
            {
                if (alignment.getScore() > max)
                {
                    result = words[i];
                    max = alignment.getScore();
                }
            }
        }
    }

    return result;
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


//
//          TMP
//
void debug_print_alignment(QString A, QString B)
{
    int l = A.size();
    if (A.size() < B.size())
        l = B.size();

    int colSize = 120;

    for (int i = 0; i < l; i += colSize)
    {
        qDebug() << QString("A :") + A.replace("\n", "").mid(i, colSize);
        qDebug() << QString("B :") + B.replace("\n", "").mid(i, colSize);
    }
}

QString saveItalic(const QString text, QList<QPair<int, int> > &positions)
{
    QStringList words = text.split(" ");

    int curr_pos = -1;
    int start = 0;

    for (int i = 0; i < words.size(); i++)
    {
        if (words[i] == "<i>")
        {
            start = curr_pos;
        }
        else if (words[i] == "</i>")
        {
            positions.append(QPair<int, int>(start, curr_pos));
        }
        else
        {
            curr_pos += words[i].size() + 1;
        }
    }

    QString noItalicText = text;

    noItalicText.replace("<i>", "").replace("</i>", "");

    return noItalicText;
}

QString restoreItalic(const QString text, const QList<QPair<int, int> > positions)
{
    QString htmlText = QString(text);

    int offset = 0, htmlOffset = 0, j = 0;

    for (int i = 0; i < text.size(); i++)
    {
        if (j < positions.size() and i + offset == positions[j].first)
        {
            htmlText = htmlText.insert(i + offset + htmlOffset, "<i>");
            htmlOffset += 3;
        }
        else if (j < positions.size() and  i + offset == positions[j].second)
        {
            htmlText = htmlText.insert(i + offset + htmlOffset, "</i>");
            htmlOffset += 4;
            j++;
        }
    }

    return htmlText;
}

//------------------------------------------------------------------------------
//  Corrector::mergeOCRizedTexts()
//------------------------------------------------------------------------------
QString Corrector::mergeOCRizedTexts(const QString strA, const QString strB)
{
    // Remove Italic
    QString noItalicA, noItalicB;
    QList<QPair<int, int> > italicPositionsA, italicPositionsB;

    noItalicA = saveItalic(strA, italicPositionsA);
    noItalicB = saveItalic(strB, italicPositionsB);

    // Align strings
    QLevenshtein aligner(noItalicA, noItalicB);
    QAlignment alignment = aligner.align();

    QString alignedA = alignment.getStringA();
    QString alignedB = alignment.getStringB();


    qDebug() << strA;
    qDebug() << strB;
    debug_print_alignment(alignedA, alignedB);


    // -------------------------------------------------------------------------
    // Remove noise
    // -------------------------------------------------------------------------
    QStringList regexps;

    // Remove '$' chars at the begining of the first paragraph
    regexps << "<body [^>]*>[^<]*<p> (.?\\$+)";

    // Remove '$' char at the begining of the text, before the first paragraph
    regexps << "<body [^>]*>[\n\\s]*(\\$+)";

    // Remove '$' char at the end of the last paragraph
    regexps << "([\\s\\$]+)</p>[^>]*</body>";

    // Remove '$' char at the end of the text, after the last paragraph
    regexps << "((\\$+\n?)+)</body>";

    for (int i = 0; i < regexps.size(); i++)
    {
        QRegExp regexA = QRegExp(regexps[i]);
        QRegExp regexB = QRegExp(regexps[i]);

        alignedA.indexOf(regexA);
        alignedB.indexOf(regexB);

        if (regexA.cap(1).size() > regexB.cap(1).size())
        {
            alignedA = alignedA.replace(regexA.pos(1), regexA.cap(1).size(), "");
            alignedB = alignedB.replace(regexA.pos(1), regexA.cap(1).size(), "");
        }
        else
        {
            alignedA = alignedA.replace(regexB.pos(1), regexB.cap(1).size(), "");
            alignedB = alignedB.replace(regexB.pos(1), regexB.cap(1).size(), "");
        }
    }

    // -------------------------------------------------------------------------
    // Merge
    // -------------------------------------------------------------------------
    QStringList validWords;

    int wordStart = 0;

    for (int i = 0; i < alignedA.size(); i++)
    {
        if (alignedA[i].isSpace() or alignedB[i].isSpace() or alignedA[i] == '\n' or alignedB[i] == '\n')
        {
            const QString wordA = alignedA.mid(wordStart, i - wordStart).replace("$", "");
            const QString wordB = alignedB.mid(wordStart, i - wordStart).replace("$", "");

            if (wordA.trimmed() == wordB.trimmed())
            {
                validWords << wordA;
            }
            else
            {
                // New lines
                if (wordA == "</p>" or wordB == "</p>")
                {
                    if (validWords.last().right(1) == "." or
                        validWords.last().right(1) == "!" or
                        validWords.last().right(1) == "?" or
                        validWords.last().right(1) == ">" )
                    {
                        validWords << "</p>";
                    }
                }
                else if (wordA != "<p>" and wordB != "<p>")
                {
                    QString wordA_clean = QString(wordA).replace(QRegExp("\\.{3}|[\\.\\?!,]$"), "");
                    QString wordB_clean = QString(wordB).replace(QRegExp("\\.{3}|[\\.\\?!,]$"), "");

                    bool wordAExists = isValid(wordA_clean) or _doublePonctuation.contains(wordA);
                    bool wordBExists = isValid(wordB_clean) or _doublePonctuation.contains(wordB);

                    qDebug() << wordA_clean << ":" << wordAExists << " " << wordB_clean << ":" << wordBExists;

                    if (wordAExists and not wordBExists)
                        validWords << wordA;
                    else if (wordBExists and not wordAExists)
                        validWords << wordB;
                    else if (wordAExists and wordBExists)
                    {
                        if (validWords.last().right(1) == "." or
                            validWords.last().right(1) == "!" or
                            validWords.last().right(1) == "?" or
                            validWords.last().right(1) == ">" )
                        {
                            if (wordA[0].isUpper() and not wordB[0].isUpper())
                                validWords << wordA;
                            else if (wordB[0].isUpper() and not wordA[0].isUpper())
                                validWords << wordB;
                            else
                                validWords << wordA;
                        }
                        //~ else
                        //~ {
                            //~ if (wordA.contains(QRegExp("[A-Z0-9]")) and not wordB.contains(QRegExp("[A-Z0-9]")))
                                //~ validWords << wordA;
                            //~ else
                                //~ validWords << wordB;
                        //~ }
                    }
                    else
                    {
                        if (wordA.isEmpty() and wordB.size() >= 2 and wordB.contains(QRegExp("[a-zA-Z0-9]")))
                        {
                            validWords << wordB;
                        }
                        else if (wordB.isEmpty() and wordA.size() >= 2 and wordA.contains(QRegExp("[a-zA-Z0-9]")))
                        {
                            validWords << wordA;
                        }
                        else if (not wordA.isEmpty() and not wordB.isEmpty())
                        {
                            if (alignedB.mid(wordStart, i - wordStart).count("$") < alignedA.mid(wordStart, i - wordStart).count("$"))
                                validWords << wordB;
                            else
                                validWords << wordA;
                        }
                    }
                }
            }

            wordStart = i + 1;
        }
    }

    QString mergedText = validWords.join(" ");

    mergedText = mergedText.replace(QRegExp(" </p>\\s+"), "</p>\n");

    //~ mergedText = restoreItalic(mergedText, italicPositionsB);

    qDebug() << mergedText;

    return mergedText;
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
//  Corrector::retrieveCandidates()
//------------------------------------------------------------------------------
QStringList Corrector::retrieveCandidates(const QString str)
{
    QStringList candidates;

    if (_dicNouns != 0)
    {
        QStringList words = _dicNouns->getWords();

        QStringList queryGrams = _generateNGrams(str, 3);

        int max = 0;
        QString best = "";

        for (int i = 0; i < words.size(); i++)
        {
            if (words[i].size() < str.size() * 2)
            {
                QStringList targetGrams = _generateNGrams(words[i], 3);
                targetGrams.removeDuplicates();

                int duplicates = (queryGrams + targetGrams).removeDuplicates();

                if (duplicates > max)
                {
                    max = duplicates;
                    best = words[i];
                }

                if (duplicates > 0.4 * queryGrams.size())
                    candidates << words[i];
            }
        }
    }

    return candidates;
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
    QString str = cursor.selectedText();
    QString correction;
    bool corrected = false;

    if (not str.isNull())
    {
        // Remove non-words
        if (not _isAlphaNum(str) and not _doublePonctuation.contains(str))
            cursor.removeSelectedText();

        // Correct tesseract error with "l'" : Phologramme -> l'hologramme
        else if (str.startsWith("P"))
        {
            QString correction = str.replace(0, 1, "l’");

            if (_isValidWithApostrophe(correction))
                corrected = true;
        }
        else
        {
            correction = _tryInsertHyphen(str);
            corrected = not correction.isEmpty();

            if (not corrected)
            {
                if (str.count(QRegExp("['’]")) == 1)
                {
                    QStringList parts = str.split(QRegExp("['’]"));

                    if (_apostrophePrefixes.contains(parts[0]))
                    {
                        correction = findSimilarWord(parts[1]);
                        corrected = not correction.isEmpty();
                        correction = parts[0] + "'" + correction;
                    }
                }
                else
                {
                    correction = findSimilarWord(str);
                    corrected = not correction.isEmpty();
                }
            }
        }

        if (corrected)
        {
            _highlight(cursor, _colors[1]);
            cursor.insertText(correction, cursor.charFormat());
        }
    }

    return corrected;
}

//------------------------------------------------------------------------------
//  Corrector::_generateNGrams()
//------------------------------------------------------------------------------
QStringList Corrector::_generateNGrams(const QString str, const int n)
{
    QString query = "#" + str + "#";

    QStringList grams;

    for (int i = 0; i < query.size() - n; i++)
        grams << query.mid(i, n);

    return grams;
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
//  Corrector::_isAlphaNum()
//------------------------------------------------------------------------------
bool Corrector::_isAlphaNum(const QString str)
{
    return str.contains(QRegExp("[a-zA-Z0-9]"));
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

        if (_apostrophePrefixes.contains(parts[0].toLower()))
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
//  Corrector::_removePunctuationInsideWords()
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

//------------------------------------------------------------------------------
//  Corrector::_tryInsertHyphen()
//------------------------------------------------------------------------------
QString Corrector::_tryInsertHyphen(const QString str)
{
    for (int i = 1; i < str.size() - 1; i++)
    {
        QString newString = str.mid(0, i) + "-" + str.mid(i);

        if (_dicNouns != 0 and _dicNouns->search(newString) != -1)
            return newString;
    }

    return QString();
}

//------------------------------------------------------------------------------
//  Corrector::_tryInsertSpace()
//------------------------------------------------------------------------------
QString Corrector::_tryInsertSpace(const QString str)
{
    for (int i = 1; i < str.size() - 1; i++)
    {
        QString newString1 = str.mid(0, i);
        QString newString2 = str.mid(i);

        if (_dicNouns != 0)
        {
            if (_dicNouns->search(newString1) != -1 and _dicNouns->search(newString2) != -1)
                return newString1 + " " + newString2;
        }
    }

    return QString();
}
