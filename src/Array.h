#ifndef Array_h
#define Array_h



#include <vector>



template<typename T>
class Array
{
    public:
        /** Default constructor */
        Array()
        {
        }

        /** Constructor */
        Array(int nLines, int nCols)
        {
            for (int i = 0; i < nLines; i++)
                _array.push_back(std::vector<T>(nCols));
        }

        /** Destructor */
        virtual ~Array()
        {
        }

        /** Get a value */
        T& at(int x, int y)
        {
            return _array[x][y];
        }

        /** Fill the array with a single value */
        void fill(T value)
        {
            for (int i = 0; i < sizeX(); i++)
                for (int j = 0; j < sizeY(); j++)
                    at(i, j) = value;
        }

        /** Resize array */
        void resize(int x, int y)
        {
            _array.resize(x);

            for (int i = 0; i < sizeX(); i++)
                _array[i].resize(y);
        }

        /** Horizontal size of the array */
        int sizeX()
        {
            return _array.size();
        }

        /** Vertical size of the array */
        int sizeY()
        {
            if (sizeX() > 0)
                return _array[0].size();
            else
                return 0;
        }

        /** Operator () to retrieve a value in the array */
        T& operator()(int x, int y)
        {
            return at(x, y);
        }

    protected:
        /** Array */
        std::vector<std::vector<T> > _array;
};


#endif
