#ifndef CORRECTOR_H
#define CORRECTOR_H

#include <QtCore>
#include <QtGui>
#include "Array.h"
#include "Dictionary.h"
#include "FileArray.h"
#include "SubstitutionMatrix.h"
#include "Utils.h"



typedef QSharedPointer<Dictionary> DictionaryPtr;

class Corrector
{
    /** Constructor and Destructor */
    public:
        Corrector(DictionaryPtr dic, DictionaryPtr names=0);
        ~Corrector();

    private:
        /** Dictionaries used for correction*/
        DictionaryPtr _dicNouns, _dicNames;

        /** Errors and corrections highlighting style
         *  0 : Underline
         *  1 : Highlight
         */
        int _highlightStyle;

        /** Colors for correction
         *  0 : uncorrected error
         *  1 : corrected error
         *  2 : format correction
         */
        QList<QColor> _colors;

        // Constants
        QStringList _simplePonctuation;
        QStringList _doublePonctuation;
        QStringList _apostrophePrefixes;


    public:
        /** Auto replace to correct OCR errors */
        void autoReplace(QTextDocument* document, bool highlight=true);

        /** Try to correct errors in a document */
        void correct(QTextDocument* document, bool highlight=true);

        /** Try to correct errors in a HTML content */
        QString correct(const QString html);

        /** Regex replacement rules */
        QMap<QString, QString> createRegexReplacementRules();

        /** Simple replacement rules */
        QMap<QString, QString> createSimpleReplacementRules();

        /** Find the nearest word of str using levenshtein alignment */
        QString findSimilarWord(const QString str);

        /** Get colors used for correction */
        QList<QColor> getColors();

        /** Get highlighting style */
        int getHighlightStyle();

        /** Check if a word is valid */
        bool isValid(const QString str);

        /** Merge results from two different OCR engines */
        QString mergeOCRizedTexts(QString strA, QString strB);

        /** Replace all occurrences of <i>before</i> with <i>after</i>
          * and highlight modifications if wanted */
        void replaceAll(QTextDocument* document, QString before,
                                            QString after, bool highlight=true);
        void replaceAll(QTextDocument* document, QRegExp before,
                                            QString after, bool highlight=true);

        /** Retrieve closest words to <i>str</i> */
        QStringList retrieveCandidates(const QString str);

        /** Set correction colors */
        void setColors(QColor colorError, QColor colorCorrected, QColor colorFormatting);

        /** Change highlighting style */
        void setHighlightStyle(int highlightStyle);

    private:
        /** Correct the word under the cursor */
        bool _correctWord(QTextCursor& cursor);

        /** Generate Ngrams for a string */
        QStringList _generateNGrams(const QString str, const int n=2);

        /** Get the format to highlight a word */
        QTextCharFormat _getHighlightFormat(const QTextCharFormat format,
                                            const QColor color);

        /** Highlight text in the document */
        void _highlight(QTextCursor& cursor, QColor color);

        /** Check if a word is alpha numeric */
        bool _isAlphaNum(const QString str);

        /** Check if a word with an apostrophe is valid */
        bool _isValidWithApostrophe(const QString str);

        /** Detect and remove punctuation inside words */
        void _removePunctuationInsideWords(QTextDocument* document,
                                           bool highlight=true);

        /** Remove all images */
        void _removeImages(QTextDocument* document);

        /** Remove page number at the end of the document */
        void _removePageNumber(QTextDocument* document);

        /** Try to correct a word by inserting an hyphen */
        QString _tryInsertHyphen(const QString str);

        /** Try to correct a word by inserting a space */
        QString _tryInsertSpace(const QString str);
};

int charToNum(const QChar _char);



#endif
