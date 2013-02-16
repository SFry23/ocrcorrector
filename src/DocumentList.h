#ifndef DocumentList_h
#define DocumentList_h



#include <QtCore>
#include <vector>

#include "TextDocument.h"



class DocumentList
{
    public:
        /** Default constructor */
        DocumentList();

        /** Default destructor */
        ~DocumentList();

    private:
        std::vector<QFile*> _images;
        std::vector<TextDocument*> _texts;

        TextDocument* _singleText;

        int _index;

        bool _locked;

    public:
        /** Add a document at the end of the list */
        void add(TextDocument* textFile, QFile* imageFile);

        /** Add a document at the end of the list */
        void add(QString textFilename, QString imageFilename);

        /** Get the current index of the document list */
        int getIndex();

        /** Get current image file */
        QFile* getImageFile(int position=-1);

        /** Get current image filename */
        QString getImageFilename(int position=-1);

        /** Get current text file */
        TextDocument* getTextFile(int position=-1);

        /** Get current text filename */
        QString getTextFilename(int position=-1);

        /** Check if there is a next document */
        bool hasNext(bool textOnly=false);

        /** Check if there is a previous document */
        bool hasPrevious(bool textOnly=false);

        /** Return 'true' if document list is locked, 'false' otherwise. */
        bool isLocked();

        /** Move to <i>position</i> position in the document list */
        void moveTo(int position);

        /** Go to next image and next text */
        void next();

        /** Go to previous image and previous texts */
        void previous();

        /** Clear all documents in the list */
        void removeAll();

        /** Remove current text file and current image file */
        void removeCurrent();

        /** Lock or unlock text files. In locked mode, only images can be browsed */
        void setLocked(bool locked);

        /** Set current text filename */
        void setTextFilename(QString filename, int position=-1);

        /** Get the image count */
        int size();
};


#endif

