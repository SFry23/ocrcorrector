#ifndef CORRECTOR_H
#define CORRECTOR_H

#include <QtCore>
#include <QtGui>
#include "Array.h"
#include "Dictionary.h"
#include "FileArray.h"
#include "SubstitutionMatrix.h"
#include "Utils.h"



class Corrector
{
    // Constructor and Destructor
    public:
        Corrector(QSharedPointer<Dictionary> dic, QSharedPointer<Dictionary> names=0);
        ~Corrector();

    private:
        /** Dictionary used for correction*/
        QSharedPointer<Dictionary> _dicNouns;

        /** Dictionary of proper nouns */
        QSharedPointer<Dictionary> _dicNames;

        /** Errors and corrections highlighting style
         *  0 : Underline
         *  1 : Highlight
         */
        int _highlightStyle;

        /** Colors for correction
         *  0 : uncorrected error
         *  1 : format correction
         *  2 : corrected error
         */
        QList<QColor> _colors;


        // Constants
        static const QStringList _SIMPLE_PUNCTUATION;
        static const QStringList _DOUBLE_PUNCTUATION;
        static const QStringList _APOSTROPHE_PREFIXES;


    public:
        /** Automatic substitutions */
        void autoReplace();

        /** Run correction on a plain text */
        static QString correct(QString plainText);

        /** Find all errors in the document */
        void correct(QTextDocument* document);

        /** Get highlighting style */
        int getHighlightStyle();

        /** Merge results from two different OCR engines */
        static QString mergeOCRizedTexts(QString strA, QString strB, QSharedPointer<Dictionary> dic, QSharedPointer<Dictionary> dicNouns=QSharedPointer<Dictionary>(0));

        /** Get colors used for correction */
        QList<QColor> getColors();

        /** Replace all occurrences of <i>before</i> with <i>after</i>
          * and highlight modifications */
        void replaceAll(QTextDocument* document, QString before, QString after);
        void replaceAll(QTextDocument* document, QRegExp before, QString after);

        /** Change colors */
        void setColors(QColor colorError, QColor colorCorrected, QColor colorFormatting);

        /** Change highlighting style */
        void setHighlightStyle(int highlightStyle);

    private:
        void autoReplace(QTextDocument* document);

        /** Correct the word under the cursor */
        bool _correctWord(QTextCursor& cursor);

        /** Damerau Levenshtein distance between two strings */
        int _levenshteinDistance(QString str1, QString str2);

        /** Highlight text in the document */
        void _highlight(QTextCursor& cursor, QColor color);

        /** Init corrector */
        void _init();

        /** Check if a word is alpha numeric */
        bool _isAlphaNum(const QString& str);

        /** Check if a word with an apostrophe is valid */
        static bool _isValidWithApostrophe(const QString str, QList<QSharedPointer<Dictionary> >dics);

        /** Remove all images */
        void _removeImages(QTextDocument* document);

        /** Remove page number at the end of the document */
        void _removePageNumber(QTextDocument* document);

        /** Remove punctuation at the beginning and at the end of <i>str</i> */
        QString _stripPunctuation(QString str);
<<<<<<< HEAD
=======

        /** Align two strings*/
        QStringList _align(QString str1, QString str2);
>>>>>>> 7f53aed... Improve auto replace : add regular expressions in internal search and replace
};

int charToNum(const QChar _char);



#endif
