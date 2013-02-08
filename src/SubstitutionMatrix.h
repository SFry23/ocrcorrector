#ifndef SUBSTITUTIONMATRIX_H
#define SUBSTITUTIONMATRIX_H

#include <QtCore>
#include <cstdlib>
#include <sstream>
#include <math.h>
#include <string>

#include "FileArray.h"



class SubstitutionMatrix : public FileArray<double>
{
    // Constructor and destructor
    public:
        SubstitutionMatrix();
        SubstitutionMatrix(int size);


    // Attributes
    private:
        /** Frequency of each letter */
        std::vector<int> _freq;


    // Methods
    public:
        /** Calculate and set score from substitution frequency, i frequency and j frequency
         *
         *  @param i ID of first item
         *  @param j ID of second item
         *  @param nSubstitutions Number of substitutions from i to j
         *  @param ni Total number of i
         *  @param nj Total number of j
         *
         *  @return Score to be inserted in the substitution matrix
         */
        void setScore(int i, int j, int nSubstitutions, int ni, int nj);

        /** Update a score int the matrix */
        void updateScore(int i, int j, int nDeltaSubst, int nDeltai, int nDeltaj);

        /** Read a substitution matrix from a file */
        bool read(std::string filename, char delim=' ');
        bool read(QString filename, char delim=' ');

        /** Resize substitution matrix */
        void resize(int size);

        /** Get frequencies */
        std::vector<int>& getFreqs();

        /** Get frequency of item i */
        int getFreq(int i);

        /** Save the substitution matrix in a file */
        bool save(std::string filename, char delim=' ');
        bool save(QString filename, char delim=' ');

        /** Set frequency of an item */
        void setFreq(int i, int freq);
};



#endif



