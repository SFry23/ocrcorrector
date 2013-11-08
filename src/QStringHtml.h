#ifndef QStringHtml_h
#define QStringHtml_h

#include <QtCore>



enum StripMode
{
    LEADING,
    TRAILING
};



class QStringHtml : public QString
{
    // Constructor and Destructor
    public:
        /** Default constructor */
        QStringHtml();

        /** Create a QStringHtml from a QString */
        QStringHtml(QString);

        /** Default destructor */
        ~QStringHtml();

    // Methods
    public:
        /** Delete <i>n</i> lines from <code>startingLine</code>
         *
         *  @param startingLine Starting line number.
         *  @param n The number of lines to be deleted.
         *
         *  @return The new truncated QStringHtml.
         */
        QStringHtml deleteLines(int startingLine, int n);

        /** Simplify, clean and uniformize HTML */
        QStringHtml reformatHtml();

        /** Remove bold parts */
        QStringHtml removeBold();

        /** Remove highlighting */
        QStringHtml removeHighlight();

        /** Remove italic parts */
        QStringHtml removeItalic();

        /** Concatenate all HTML paragraphs into a single one */
        QStringHtml removeNewLineTags();

        /** Remove spaces at the end of lines */
        QStringHtml removeRightSpaces();

        /** Remove underlined parts */
        QStringHtml removeUnderline();

        /** Replace meta tag with a standard one */
        QStringHtml replaceMeta();

        /** Remove the first or the last consecutive HTML tags
         *
         *  @param mode LEADING removes the first HTML tag (default).<br/>
         *                   TRAILING mode removes the last HTML tag.
         *  @return Modified string.
         */
        QStringHtml stripHtmlTags(StripMode mode=LEADING);

        /** Convert to QString.
         *
         *  @return A QString with the same content.
         */
        QString toQString();
};



#endif
