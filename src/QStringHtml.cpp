#include "QStringHtml.h"



//------------------------------------------------------------------------------
//  QStringHtml::QStringHtml()
//------------------------------------------------------------------------------
QStringHtml::QStringHtml() : QString()
{
}

//------------------------------------------------------------------------------
//  QStringHtml::QStringHtml()
//------------------------------------------------------------------------------
QStringHtml::QStringHtml(QString string) : QString(string)
{
}

//------------------------------------------------------------------------------
//  QStringHtml::~QStringHtml()
//------------------------------------------------------------------------------
QStringHtml::~QStringHtml()
{
}



//------------------------------------------------------------------------------
//  QStringHtml::deleteLines()
//------------------------------------------------------------------------------
QStringHtml QStringHtml::deleteLines(int startingLine, int n)
{
    // Reach first line to be deleted
    int startingPosition = 0;
    for (int i = 0; i < startingLine; i++)
        startingPosition = indexOf("\n", startingPosition) + 1;

    // Delete n lines
    int endingPosition = startingPosition;
    for (int i = 0; i < n; i++)
    {
        endingPosition = indexOf("\n", endingPosition) + 1;

        if (endingPosition == 0)
            endingPosition = size();
    }

    if (endingPosition == size() and startingPosition != 0)
        startingPosition -= 1;

    QString newString = remove(startingPosition, endingPosition - startingPosition);

    return QStringHtml(newString);
}

//------------------------------------------------------------------------------
//  QStringHtml::reformatHtml()
//------------------------------------------------------------------------------
QStringHtml QStringHtml::reformatHtml()
{
    QString cleanHtml;

    int currPos = 0;
    bool inBody = false;
    bool inItalic = false;

    while (currPos < size())
    {
        if (at(currPos) == '<')
        {
            QRegExp rx = QRegExp("<([^>]*)>");

            if (indexOf(rx, currPos) != -1)
            {
                QString tag = rx.capturedTexts()[1];

                QStringList parts = tag.split(" ");

                if (parts[0] == "p")
                {
                    cleanHtml += "<p> ";
                }
                else if (parts[0] == "/p")
                {
                    cleanHtml += " </p>\n";
                }
                else if (parts[0] == "body")
                {
                    cleanHtml += rx.cap();
                    inBody = true;
                }
                else if (parts[0] == "/body")
                {
                    cleanHtml += rx.cap() + "\n";
                }
                else if (parts[0] == "!DOCTYPE")
                {
                    cleanHtml += rx.cap() + "\n";
                }


                if (tag.contains("style=\" font-style:italic;\""))
                {
                    cleanHtml += " <i> ";
                    inItalic = true;
                }

                if (inItalic and parts[0] == "/span")
                {
                    cleanHtml += " </i> ";
                    inItalic = false;
                }

                currPos += rx.matchedLength();
            }
        }
        else
        {
            QRegExp rx = QRegExp("[^<]*");

            if (indexOf(rx, currPos) != -1)
            {
                if (inBody)
                    cleanHtml += rx.cap();

                currPos += rx.matchedLength();
            }
        }
    }

    /** Convert tesseract italic style <i> word1 </i>  <i> word2 </i>
     *  to standart style <i> word1 word2 </i> */
    cleanHtml = cleanHtml.replace(QRegExp(" </i> ( ?) <i> "), "\\1");

    /** Remove extra spaces */
    cleanHtml = cleanHtml.replace(QRegExp("(\\s)\\s+"), "\\1");

    return cleanHtml;
}

//------------------------------------------------------------------------------
//  QStringHtml::removeNewLineTags()
//------------------------------------------------------------------------------
QStringHtml QStringHtml::removeNewLineTags()
{
    return replace(QRegExp("([^>.\?!]) </p>([^<]*)<p>"), "\\1");
}

//------------------------------------------------------------------------------
//  QStringHtml::stripHtmlTags()
//------------------------------------------------------------------------------
QStringHtml QStringHtml::stripHtmlTags(StripMode mode)
{
    QString string = QString(toUtf8());

    if (mode == LEADING)
    {
        while(string.size() > 0 and (string.startsWith("<") or string.startsWith("\n")))
        {
            int position = string.indexOf(">");

            if (position != -1)
                string = string.remove(0, position + 1);
            else
                break;
        }
    }
    else if (mode == TRAILING)
    {
        while(string.size() > 0 and (string.endsWith(">") or string.endsWith("\n")))
        {
            int position = string.lastIndexOf("<");

            if (position != -1)
                string = string.remove(position, string.size() - position + 1);
            else
                break;
        }
    }

    return string;
}

//------------------------------------------------------------------------------
//  QStringHtml::toQString()
//------------------------------------------------------------------------------
QString QStringHtml::toQString()
{
    return remove(0, 0);
}


//------------------------------------------------------------------------------
//  QStringHtml::removeBold()
//------------------------------------------------------------------------------
QStringHtml QStringHtml::removeBold()
{
    return replace(" font-weight:600;", "");
}

//------------------------------------------------------------------------------
//  QStringHtml::removeHighlight()
//------------------------------------------------------------------------------
QStringHtml QStringHtml::removeHighlight()
{
    return replace(QRegExp(" background-color[^\"]*"), "");
}

//------------------------------------------------------------------------------
//  QStringHtml::removeItalic()
//------------------------------------------------------------------------------
QStringHtml QStringHtml::removeItalic()
{
    return replace(QRegExp(" font-style:[\\s]*italic;"), "");
}

//------------------------------------------------------------------------------
//  QStringHtml::removeUnderline()
//------------------------------------------------------------------------------
QStringHtml QStringHtml::removeUnderline()
{
    return replace(QRegExp(" text-decoration:[\\s]*underline;"), "");
}

//------------------------------------------------------------------------------
//  QStringHtml::replaceMeta()
//------------------------------------------------------------------------------
QStringHtml QStringHtml::replaceMeta()
{
    return replace(QString("<meta name=\"qrichtext\" content=\"1\" />"),
                   QString("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"));
}




