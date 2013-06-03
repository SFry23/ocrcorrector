#ifndef Utils_h
#define Utils_h

#include <QtCore>
#include "SubstitutionMatrix.h"


/** Alignment of two strings result */
class QAlignment
{
    // Constructor and Destructor
    public:
        QAlignment(QString strA, QString strB, double score);
        ~QAlignment();

    // Attributes
    private:
        /** Aligned strings */
        QString _strA, _strB;

        /** Alignment score */
        double _score;

    // Methods
    public:
        /** Get the alignment score */
        double getScore();

        /** Get the first string aligned */
        QString getStringA();

        /** Get the second string aligned */
        QString getStringB();
};

/** Levenshtein alignment between two strings */
class QLevenshtein
{
    // Constructor and Destructor
    public:
        QLevenshtein(QString strA, QString strB);
        ~QLevenshtein();

    // Attributes
    private:
        SubstitutionMatrix _S;
        QString _strA, _strB;
        double _gapOpeningPenalty;
        double _gapExtensionPenalty;
        char _gapSymbol;

    // Methods
    public:
        /** Align the two strings */
        QAlignment align();

        /** Get the current gap extension penalty */
        double getGapExtensionPenalty();

        /** Get the current gap opening penalty */
        double getGapOpeningPenalty();

        /** Get the gap symbol */
        char getGapSymbol();

        /** Get the substitution matrix */
        SubstitutionMatrix getSubstitutionMatrix();

        /** Change the current gap extension penalty */
        void setGapExtensionPenalty(double gapPenalty);

        /** Change the current gap opening penalty */
        void setGapOpeningPenalty(double gapPenalty);

        /** Change gap symbol */
        void setGapSymbol(char gapSymbol);

        /** Use a specific substitution matrix stored in a file */
        bool setSubstitutionMatrix(QString filename);

    private:
        /** Convert a char to an int */
        int _charToNum(const QChar c);

        /** Compute score matrix */
        FileArray<double> _createScoreMatrix();

        /** Retrieve alignment from score matrix */
        QAlignment _retrieveAlignment(FileArray<double> scoreArray);
};


#endif
