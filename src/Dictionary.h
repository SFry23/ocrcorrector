#ifndef Dictionary_H
#define Dictionary_H


#include <QtCore>



class Dictionary
{
    public:
        /** Construct a Dictionary from a file */
        Dictionary(const QString path);

        /** Add a word to the Dictionary */
        void add(const QString s);

        /** Check if a string is in the dictionary */
        bool exists(const QString str);

        /** Get path to the Dictionary */
        QString getPath();

        /** Load the Dictionary */
        bool load();

        /** Remove a word from the Dictionary */
        bool remove(const QString s);

        /** Save the current Dictionary */
        bool save(const QString filename="");

        /** Search a word in the Dictionary */
        int search(const QString value);

        /** Set a QStringList as a Dictionary*/
        void set(QStringList l);

        /** Change path to the dictionary file */
        void setPath(const QString filename);

    private:
        /** List of words */
        QStringList _words;

        /** Path */
        QString _path;
};



#endif

