#ifndef FILEARRAY_H
#define FILEARRAY_H


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Array.h"


template<typename T>
class FileArray : public Array<T>
{
    public:
        /** Constructors */
        FileArray() : Array<T>()
        {
        }

        FileArray(int nLines, int nCols) : Array<T>(nLines, nCols)
        {
        }

        /** Destructor */
        virtual ~FileArray()
        {
        }

        /** Read an array from a file */
        void read(std::string filename, char delim=' ')
        {
            std::ifstream f(filename.c_str());

            if (f.is_open())
            {
                std::string line;
                this->resize(0, 0);

                while (f.good())
                {
                    std::getline(f, line);

                    std::stringstream ss(line);
                    std::string token;
                    std::vector<std::string> tokens;

                    while (std::getline(ss, token, delim))
                        tokens.push_back(token);

                    this->resize(this->sizeX() + 1, tokens.size());

                    for (int i = 0; i < (int) tokens.size(); i++)
                    {
                        T value;
                        std::stringstream ss(tokens[i]);

                        if (not (ss >> value))
                            value = 0;

                        this->at(this->sizeX() - 1, i) = value;
                    }
                }

                f.close();
            }
            else
                std::cerr << "Unable to open " << filename << std::endl;
        }

        /** Write the array in a file */
        void save(std::string filename, char delim=' ')
        {
            std::ofstream f(filename.c_str());

            if (f.is_open())
            {
                for (int i = 0; i < this->sizeX(); i++)
                {
                    for (int j = 0; j < this->sizeY(); j++)
                    {
                        if (j > 0)
                            f << delim << this->at(i, j);
                        else
                            f << this->at(i, j);
                    }

                    if (i < this->sizeX() - 1)
                        f << std::endl;
                }

                f.close();
            }
            else
                std::cerr << "Unable to open " << filename << std::endl;
        }
};


#endif
