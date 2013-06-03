#include "Utils.h"



//------------------------------------------------------------------------------
//  QAlignment::QAlignment()
//------------------------------------------------------------------------------
QAlignment::QAlignment(const QString strA, const QString strB, double score)
{
    _strA = strA;
    _strB = strB;
    _score = score;
}

//------------------------------------------------------------------------------
//  QAlignment::~QAlignment()
//------------------------------------------------------------------------------
QAlignment::~QAlignment()
{
}

//------------------------------------------------------------------------------
//  QAlignment::getScore()
//------------------------------------------------------------------------------
double QAlignment::getScore()
{
    return _score;
}

//------------------------------------------------------------------------------
//  QAlignment::getStringA()
//------------------------------------------------------------------------------
QString QAlignment::getStringA()
{
    return _strA;
}

//------------------------------------------------------------------------------
//  QAlignment::getStringB()
//------------------------------------------------------------------------------
QString QAlignment::getStringB()
{
    return _strB;
}




//------------------------------------------------------------------------------
//  QLevenshtein::QLevenshtein()
//------------------------------------------------------------------------------
QLevenshtein::QLevenshtein(QString strA, QString strB)
{
    _strA = strA;
    _strB = strB;
    _gapOpeningPenalty = -1;
    _gapExtensionPenalty = -0.6;
    _gapSymbol = '$';
    _S = SubstitutionMatrix(200);
}

//------------------------------------------------------------------------------
//  QLevenshtein::~QLevenshtein()
//------------------------------------------------------------------------------
QLevenshtein::~QLevenshtein()
{
}

//------------------------------------------------------------------------------
//  QLevenshtein::align()
//------------------------------------------------------------------------------
QAlignment QLevenshtein::align()
{
    FileArray<double> F = _createScoreMatrix();

    return _retrieveAlignment(F);
}

//------------------------------------------------------------------------------
//  QLevenshtein::getGapExtensionPenalty()
//------------------------------------------------------------------------------
double QLevenshtein::getGapExtensionPenalty()
{
    return _gapOpeningPenalty;
}

//------------------------------------------------------------------------------
//  QLevenshtein::getGapOpeningPenalty()
//------------------------------------------------------------------------------
double QLevenshtein::getGapOpeningPenalty()
{
    return _gapOpeningPenalty;
}

//------------------------------------------------------------------------------
//  QLevenshtein::getSubstitutionMatrix()
//------------------------------------------------------------------------------
SubstitutionMatrix QLevenshtein::getSubstitutionMatrix()
{
    return _S;
}

//------------------------------------------------------------------------------
//  QLevenshtein::setGapExtensionPenalty()
//------------------------------------------------------------------------------
void QLevenshtein::setGapExtensionPenalty(double gapPenalty)
{
    _gapExtensionPenalty = gapPenalty;
}

//------------------------------------------------------------------------------
//  QLevenshtein::setGapOpeningPenalty()
//------------------------------------------------------------------------------
void QLevenshtein::setGapOpeningPenalty(double gapPenalty)
{
    _gapOpeningPenalty = gapPenalty;
}

//------------------------------------------------------------------------------
//  QLevenshtein::useSubstitutionMatrix()
//------------------------------------------------------------------------------
bool QLevenshtein::setSubstitutionMatrix(QString filename)
{
    return _S.read(filename);
}




//------------------------------------------------------------------------------
//  QLevenshtein::_charToNum()
//------------------------------------------------------------------------------
int QLevenshtein::_charToNum(const QChar c)
{
    return (int) c.toAscii();
}

//------------------------------------------------------------------------------
//  QLevenshtein::_createScoreMatrix()
//------------------------------------------------------------------------------
FileArray<double> QLevenshtein::_createScoreMatrix()
{
    FileArray<double> F(_strA.size() + 1, _strB.size() + 1);

    // Initialization
    for (int i = 1; i < F.sizeX(); i++)
        F(i, 0) = _gapOpeningPenalty * i;

    for (int j = 1; j < F.sizeY(); j++)
        F(0, j) = _gapOpeningPenalty * j;


    // Compute matrix
    for (int i = 1; i <= (int) _strA.size(); i++)
    {
        for (int j = 1; j <= (int) _strB.size(); j++)
        {
            double choice1 = F(i - 1, j - 1);
            double choice2 = F(i - 1, j) + _gapOpeningPenalty;

            if (i < 2)
                choice2 = F(i - 1, j) + _gapExtensionPenalty;
            else if (F(i - 1, j) == F(i - 2, j) + _gapOpeningPenalty or F(i - 1, j) == F(i - 2, j) + _gapExtensionPenalty)
                choice2 = F(i - 1, j) + _gapExtensionPenalty;

            double choice3 = F(i, j - 1) + _gapOpeningPenalty;

            if (i < 2)
                choice3 = F(i - 1, j) + _gapExtensionPenalty;
            else if (F(i, j - 1) == F(i, j - 2) + _gapOpeningPenalty or F(i, j - 1) == F(i, j - 2) + _gapExtensionPenalty)
                choice3 = F(i, j - 1) + _gapExtensionPenalty;

            int codeLetterA = _charToNum(_strA[i-1]);
            int codeLetterB = _charToNum(_strB[j-1]);

            if (codeLetterA >= 0 and codeLetterA < _S.sizeX() and
                codeLetterB >= 0 and codeLetterB < _S.sizeY())
            {
                choice1 += _S(codeLetterA, codeLetterB);
            }
            else
            {
                if (codeLetterA == codeLetterB)
                    choice1 += abs(_gapOpeningPenalty / 2);
            }

            F(i, j) = std::max(std::max(choice1, choice2), choice3);
        }
    }

    return F;
}

//------------------------------------------------------------------------------
//  QLevenshtein::_retrieveAlignment()
//------------------------------------------------------------------------------
QAlignment QLevenshtein::_retrieveAlignment(FileArray<double> F)
{
    QString alignedA;
    QString alignedB;

    int i = _strA.size();
    int j = _strB.size();

    while (i > 0 and j > 0)
    {
        double currScore = F(i, j);
        double scoreLeft = F(i - 1, j);
        double scoreUp = F(i, j - 1);

        if (currScore == scoreLeft + _gapOpeningPenalty or
            currScore == scoreLeft + _gapExtensionPenalty)
        {
            alignedA = _strA[i-1] + alignedA;
            alignedB = _gapSymbol + alignedB;
            i--;
        }
        else if (currScore == scoreUp + _gapOpeningPenalty or
                 currScore == scoreUp + _gapExtensionPenalty)
        {
            alignedA = _gapSymbol + alignedA;
            alignedB = _strB[j-1] + alignedB;
            j--;
        }
        else
        {
            alignedA = _strA[i-1] + alignedA;
            alignedB = _strB[j-1] + alignedB;
            i--;
            j--;
        }
    }

    while (i > 0)
    {
        alignedA = _strA[i-1] + alignedA;
        alignedB = _gapSymbol + alignedB;
        i--;
    }

    while (j > 0)
    {
        alignedA = _gapSymbol + alignedA;
        alignedB = _strB[j-1] + alignedB;
        j--;
    }

    return QAlignment(alignedA, alignedB, F(_strA.size(), _strB.size()));
}

