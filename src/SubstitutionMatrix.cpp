#include "SubstitutionMatrix.h"



//-----------------------------------------------------------------------------
//  SubstitutionMatrix::SubstitutionMatrix()
//-----------------------------------------------------------------------------
SubstitutionMatrix::SubstitutionMatrix(int size) : FileArray<double>(size, size)
{
    for (int i = 0; i < size; i++)
        at(i, i) = 1;

    _freq.resize(size, 0);
}

//-----------------------------------------------------------------------------
//  SubstitutionMatrix::getFreq()
//-----------------------------------------------------------------------------
int SubstitutionMatrix::getFreq(int i)
{
    return _freq[i];
}

//-----------------------------------------------------------------------------
//  SubstitutionMatrix::getFreqs()
//-----------------------------------------------------------------------------
std::vector<int>& SubstitutionMatrix::getFreqs()
{
    return _freq;
}

//-----------------------------------------------------------------------------
//  SubstitutionMatrix::setScore()
//-----------------------------------------------------------------------------
void SubstitutionMatrix::setScore(int i, int j, int nSubstitutions, int ni, int nj)
{
    const double MAX_SCORE = 10;

    if (ni != 0 and nj != 0)
    {
        if (nSubstitutions > 0)
            at(i, j) = log((double) nSubstitutions / (ni * nj));
        else
            at(i, j) = MAX_SCORE;

        _freq[i] = ni;
        _freq[j] = nj;
    }
}

//-----------------------------------------------------------------------------
//  SubstitutionMatrix::updateScore()
//-----------------------------------------------------------------------------
void SubstitutionMatrix::updateScore(int i, int j, int nDeltaSubst, int nDeltai, int nDeltaj)
{
    // Invert score function
    double nSubst = pow(at(i, j), 10) * _freq[i] * _freq[j];

    // Calc new score
    setScore(i, j, nSubst + nDeltaSubst, _freq[i] + nDeltai, _freq[j] + nDeltaj);
}

bool SubstitutionMatrix::read(QString filename, char delim)
{
    std::ifstream f(filename.toStdString().c_str());

    if (f.is_open())
    {
        std::string line;
        this->resize(0);

        int lineNumber = 0;
        while (std::getline(f, line) and line != "")
        {
            std::stringstream ss(line);

            std::string token;
            std::vector<std::string> tokens;

            while (std::getline(ss, token, delim))
                tokens.push_back(token);

            if (sizeX() != (int) tokens.size())
                resize(tokens.size());

            for (int i = 0; i < sizeX(); i++)
            {
                double value;
                std::stringstream ss(tokens[i]);

                if (not (ss >> value))
                    value = 0.0;

                at(lineNumber, i) = value;
            }
            lineNumber++;
        }

        while (f.good())
        {
            // Skiped 1 empty line after matrix, read the important one
            std::getline(f, line);

            if (line != "")
            {
                std::stringstream ss(line);
                std::string token;

                for (int i = 0; i < sizeX(); i++)
                {
                    std::getline(ss, token, delim);

                    double value = atof(token.c_str());
                    _freq[i] = value;
                }
            }
        }

        f.close();

        return true;
    }
    else
        return false;
}

//-----------------------------------------------------------------------------
//  SubstitutionMatrix::resize()
//-----------------------------------------------------------------------------
void SubstitutionMatrix::resize(int size)
{
    FileArray::resize(size, size);

    _freq.resize(size, 0);
}

bool SubstitutionMatrix::save(QString filename, char delim)
{
    FileArray::save(filename.toStdString(), delim);

    // Add a row with frequency of each letter
    QFile file(filename);

    if (file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream f(&file);
        f << endl << endl;

        for (int i = 0; i < ((int) _freq.size()) - 1; i++)
            f << _freq[i] << delim;

        f << _freq.back() << endl;

        file.close();

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
//  SubstitutionMatrix::setFreq()
//-----------------------------------------------------------------------------
void SubstitutionMatrix::setFreq(int i, int freq)
{
    _freq[i] = freq;
}




