#ifndef Utils_h
#define Utils_h

#include <QtCore>
#include <string>
#include "SubstitutionMatrix.h"


class Alignment
{
    // Constructor and Destructor
    public:
        Alignment(std::string strA, std::string strB, double score);
        ~Alignment();

    // Attributes
    private:
        std::string _strA, _strB;
        double _score;

    // Methods
    public:
        /** Get the alignment score */
        double getScore();

        /** Get the first string aligned */
        std::string getStringA();

        /** Get the second string aligned */
        std::string getStringB();
};

class QAlignment
{
    // Constructor and Destructor
    public:
        QAlignment(QString strA, QString strB, double score);
        ~QAlignment();

    // Attributes
    private:
        QString _strA, _strB;
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


class Levenshtein
{
    // Constructor and Destructor
    public:
        Levenshtein(std::string strA, std::string strB);
        ~Levenshtein();

    // Attributes
    private:
        SubstitutionMatrix _S;
        std::string _strA, _strB;
        double _gapPenalty;
        char _gapSymbol;

    // Methods
    public:
        /** Align the two strings */
        Alignment align();

        /** Get the current gap penalty */
        double getGapPenalty();

        /** Get the gap symbol */
        char getGapSymbol();

        /** Get the substitution matrix */
        SubstitutionMatrix getSubstitutionMatrix();

        /** Change the current gap penalty */
        void setGapPenalty(double gapPenalty);

        /** Change gap symbol */
        void setGapSymbol(char gapSymbol);

        /** Use a specific substitution matrix stored in a file */
        bool useSubstitutionMatrix(std::string filename);

    private:
        /** Convert a char to an int */
        int _charToNum(const char c);
};

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
        double _gapPenalty;
        char _gapSymbol;

    // Methods
    public:
        /** Align the two strings */
        QAlignment align();

        /** Get the current gap penalty */
        double getGapPenalty();

        /** Get the gap symbol */
        char getGapSymbol();

        /** Get the substitution matrix */
        SubstitutionMatrix getSubstitutionMatrix();

        /** Change the current gap penalty */
        void setGapPenalty(double gapPenalty);

        /** Change gap symbol */
        void setGapSymbol(char gapSymbol);

        /** Use a specific substitution matrix stored in a file */
        bool useSubstitutionMatrix(QString filename);

    private:
        /** Convert a char to an int */
        int _charToNum(const QChar c);
};

// TODO : changer la fonction de place
int charToNum(const QChar c);


#endif
