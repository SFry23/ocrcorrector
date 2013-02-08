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






