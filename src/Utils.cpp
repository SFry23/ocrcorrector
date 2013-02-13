#include "Utils.h"



//------------------------------------------------------------------------------
//  Alignment::Alignment()
//------------------------------------------------------------------------------
Alignment::Alignment(std::string strA, std::string strB, double score)
{
    _strA = strA;
    _strB = strB;
    _score = score;
}

//------------------------------------------------------------------------------
//  Alignment::~Alignment()
//------------------------------------------------------------------------------
Alignment::~Alignment()
{
}

//------------------------------------------------------------------------------
//  Alignment::getScore()
//------------------------------------------------------------------------------
double Alignment::getScore()
{
    return _score;
}

//------------------------------------------------------------------------------
//  Alignment::getStringA()
//------------------------------------------------------------------------------
std::string Alignment::getStringA()
{
    return _strA;
}

//------------------------------------------------------------------------------
//  Alignment::getStringB()
//------------------------------------------------------------------------------
std::string Alignment::getStringB()
{
    return _strB;
}




//------------------------------------------------------------------------------
//  QAlignment::QAlignment()
//------------------------------------------------------------------------------
QAlignment::QAlignment(QString strA, QString strB, double score)
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
//  Levenshtein::Levenshtein()
//------------------------------------------------------------------------------
Levenshtein::Levenshtein(std::string strA, std::string strB)
{
    _strA = strA;
    _strB = strB;
    _gapPenalty = -0.1;
    _gapSymbol = '$';
    _S = SubstitutionMatrix(200);

    //~ _S.read("src/dic/.tesseract-substitution.txt");
}

//------------------------------------------------------------------------------
//  Levenshtein::~Levenshtein()
//------------------------------------------------------------------------------
Levenshtein::~Levenshtein()
{
}

//------------------------------------------------------------------------------
//  Levenshtein::align()
//------------------------------------------------------------------------------
Alignment Levenshtein::align()
{
    // Compare two words
    FileArray<double> F(_strA.size() + 1, _strB.size() + 1);

    // First line
    for (int i = 1; i < F.sizeX(); i++)
        F(i, 0) = _gapPenalty * i;

    // First column
    for (int i = 1; i < F.sizeY(); i++)
        F(0, i) = _gapPenalty * i;

    // Needleman wunsch
    for (int i = 1; i <= (int) _strA.size(); i++)
    {
        for (int j = 1; j <= (int) _strB.size(); j++)
        {
            double choice1 = 0;

            int codeLetterA = _charToNum(_strA[i-1]);
            int codeLetterB = _charToNum(_strB[j-1]);

            if (codeLetterA >= 0 and codeLetterA < _S.sizeX() and codeLetterB >= 0 and codeLetterB < _S.sizeY())
                choice1 = F(i - 1, j - 1) + _S(codeLetterA, codeLetterB);
            else
                std::cout << _S.sizeX() << "x" << _S.sizeY() << " " << _strA[i-1] << "(" << codeLetterA << ") " << _strB[j-1] << "(" << codeLetterB << ")" << std::endl;


            double choice2 = F(i - 1, j) + _gapPenalty;
            double choice3 = F(i, j - 1) + _gapPenalty;

            F(i, j) = std::max(std::max(choice1, choice2), choice3);
        }
    }

    // Retrieve alignment
    std::string alignA;
    std::string alignB;

    int i = _strA.size();
    int j = _strB.size();

    while (i > 0 and j > 0)
    {
        double currScore = F(i, j);
        double scoreLeft = F(i - 1, j);
        double scoreUp = F(i, j - 1);


        if (currScore == scoreLeft + _gapPenalty)
        {
            alignA = _strA[i-1] + alignA;
            alignB = _gapSymbol + alignB;
            i--;
        }
        else if (currScore == scoreUp + _gapPenalty)
        {
            alignA = _gapSymbol + alignA;
            alignB = _strB[j-1] + alignB;
            j--;
        }
        else
        {
            alignA = _strA[i-1] + alignA;
            alignB = _strB[j-1] + alignB;
            i--;
            j--;
        }
    }

    while (i > 0)
    {
        alignA = _strA[i-1] + alignA;
        alignB = _gapSymbol + alignB;
        i--;
    }

    while (j > 0)
    {
        alignA = _gapSymbol + alignA;
        alignB = _strB[j-1] + alignB;
        j--;
    }


    Alignment align(alignA, alignB, F(_strA.size(), _strB.size()));

    return align;
}

//------------------------------------------------------------------------------
//  Levenshtein::getGapPenalty()
//------------------------------------------------------------------------------
double Levenshtein::getGapPenalty()
{
    return _gapPenalty;
}

//------------------------------------------------------------------------------
//  Levenshtein::getSubstitutionMatrix()
//------------------------------------------------------------------------------
SubstitutionMatrix Levenshtein::getSubstitutionMatrix()
{
    return _S;
}

//------------------------------------------------------------------------------
//  Levenshtein::setGapPenalty()
//------------------------------------------------------------------------------
void Levenshtein::setGapPenalty(double gapPenalty)
{
    _gapPenalty = gapPenalty;
}

//------------------------------------------------------------------------------
//  Levenshtein::useSubstitutionMatrix()
//------------------------------------------------------------------------------
bool Levenshtein::useSubstitutionMatrix(std::string filename)
{
    return _S.read(filename);
}

//------------------------------------------------------------------------------
//  Levenshtein::_charToNum()
//------------------------------------------------------------------------------
int Levenshtein::_charToNum(const char c)
{
    return (int) c;
}






//------------------------------------------------------------------------------
//  QLevenshtein::QLevenshtein()
//------------------------------------------------------------------------------
QLevenshtein::QLevenshtein(QString strA, QString strB)
{
    _strA = strA;
    _strB = strB;
    _gapPenalty = -0.5;
    _gapSymbol = '$';
    _S = SubstitutionMatrix(200);

    //~ _S.read("src/dic/.tesseract-substitution.txt");

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
    // Compare two words
    FileArray<double> F(_strA.size() + 1, _strB.size() + 1);

    // First line
    for (int i = 1; i < F.sizeX(); i++)
        F(i, 0) = _gapPenalty * i;

    // First column
    for (int i = 1; i < F.sizeY(); i++)
        F(0, i) = _gapPenalty * i;

    // Levenshtein distance
    for (int i = 1; i <= (int) _strA.size(); i++)
    {
        for (int j = 1; j <= (int) _strB.size(); j++)
        {
            double choice1 = F(i - 1, j - 1);
            double choice2 = F(i - 1, j) + _gapPenalty;
            double choice3 = F(i, j - 1) + _gapPenalty;

            int codeLetterA = _charToNum(_strA[i-1]);
            int codeLetterB = _charToNum(_strB[j-1]);

            if (codeLetterA >= 0 and codeLetterA < _S.sizeX() and codeLetterB >= 0 and codeLetterB < _S.sizeY())
                choice1 += _S(codeLetterA, codeLetterB);
            else
            {
                //~ qDebug() << _S.sizeX() << "x" << _S.sizeY() << " " << _strA[i-1] << "(" << codeLetterA << ") " << _strB[j-1] << "(" << codeLetterB << ")";

                if (codeLetterA == codeLetterB)
                    choice1 += abs(_gapPenalty * 2);
            }

            F(i, j) = std::max(std::max(choice1, choice2), choice3);
        }
    }

    // Retrieve alignment
    QString alignA;
    QString alignB;

    int i = _strA.size();
    int j = _strB.size();

    while (i > 0 and j > 0)
    {
        double currScore = F(i, j);
        double scoreLeft = F(i - 1, j);
        double scoreUp = F(i, j - 1);


        if (currScore == scoreLeft + _gapPenalty)
        {
            alignA = _strA[i-1] + alignA;
            alignB = _gapSymbol + alignB;
            i--;
        }
        else if (currScore == scoreUp + _gapPenalty)
        {
            alignA = _gapSymbol + alignA;
            alignB = _strB[j-1] + alignB;
            j--;
        }
        else
        {
            alignA = _strA[i-1] + alignA;
            alignB = _strB[j-1] + alignB;
            i--;
            j--;
        }
    }

    while (i > 0)
    {
        alignA = _strA[i-1] + alignA;
        alignB = _gapSymbol + alignB;
        i--;
    }

    while (j > 0)
    {
        alignA = _gapSymbol + alignA;
        alignB = _strB[j-1] + alignB;
        j--;
    }


    QAlignment align(alignA, alignB, F(_strA.size(), _strB.size()));

    return align;
}

//------------------------------------------------------------------------------
//  QLevenshtein::getGapPenalty()
//------------------------------------------------------------------------------
double QLevenshtein::getGapPenalty()
{
    return _gapPenalty;
}

//------------------------------------------------------------------------------
//  QLevenshtein::getSubstitutionMatrix()
//------------------------------------------------------------------------------
SubstitutionMatrix QLevenshtein::getSubstitutionMatrix()
{
    return _S;
}

//------------------------------------------------------------------------------
//  QLevenshtein::setGapPenalty()
//------------------------------------------------------------------------------
void QLevenshtein::setGapPenalty(double gapPenalty)
{
    _gapPenalty = gapPenalty;
}

//------------------------------------------------------------------------------
//  QLevenshtein::useSubstitutionMatrix()
//------------------------------------------------------------------------------
bool QLevenshtein::useSubstitutionMatrix(QString filename)
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


int charToNum(const QChar c)
{
    return (int) c.toAscii();
}

