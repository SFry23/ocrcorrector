#ifndef CORRECTOR_H
#define CORRECTOR_H

#include <QtCore>
#include <QtGui>
#include "Array.h"
#include "Dictionary.h"
#include "FileArray.h"
#include "SubstitutionMatrix.h"



class Corrector
{
    // Constructor and Destructor
    public:
        Corrector(QTextDocument* document, QSharedPointer<Dictionary> dic, QSharedPointer<Dictionary> names=0);
        ~Corrector();

    private:
        /** Document to be corrected */
        QTextDocument* _document;

        /** Dictionary used for correction*/
        QSharedPointer<Dictionary> _dicNouns;

        /** Dictionary of proper nouns */
        QSharedPointer<Dictionary> _dicNames;

        /** Errors and corrections highlighting style */
        int _highlightStyle;

        /** Colors */
        QList<QColor> _colors;

        /** Number of errors */
        int _nErrors;

        /** Number of corrected errors */
        int _nCorrectedErrors;


        // Constants
        static const QStringList _SIMPLE_PUNCTUATION;
        static const QStringList _DOUBLE_PUNCTUATION;
        static const QStringList _APOSTROPHE_PREFIXES;


    public:
        /** Find all errors in the document */
        void detectErrors();

        /** Get the corrected document */
        QTextDocument* getDocument();

        /** Get highlighting style */
        int getHighlightStyle();

        /** Get number of corrected errors */
        int getNumberCorrectedErrors();

        /** Get number of errors */
        int getNumberErrors();

        /** Replace all occurrences of <i>before</i> with <i>after</i>
          * and unerline modifications */
        void replaceAll(QString before, QString after);
        void replaceAll(QRegExp before, QString after);

        /** Change colors */
        void setColors(QColor colorError, QColor colorCorrected, QColor colorFormatting);

        /** Change highlighting style */
        void setHighlightStyle(int highlightStyle);

        //~ void _createSubsitutionMatrixFrom(QString filename);

    private:
        /** Correct the word under the cursor */
        bool _correctWord(QTextCursor& cursor);

        /** Damerau Levenshtein distance between two strings */
        int _levenshteinDistance(QString str1, QString str2);

        /** Highlight text in the document */
        void _highlight(QTextCursor& cursor, QColor color);

        /** Check if a word is alpha numeric */
        bool _isAlphaNum(const QString& str);

        /** Check if a word with an apostrophe is valid */
        bool _isValidWithApostrophe(const QString& str);

        /** Remove all images */
        void _removeImages();

        /** Remove page number at the end of the document */
        void _removePageNumber();

        /** Remove punctuation at the beginning and at the end of <i>str</i> */
        QString _stripPunctuation(QString str);

        QStringList _align(QString str1, QString str2);
};

int charToNum(const QChar _char);

#endif
