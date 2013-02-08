#ifndef TextDocument_h
#define TextDocument_h


#include <QtCore>
#include <QtGui>



class TextDocument : public QFile
{
    public:
        // Constructors
        TextDocument();
        TextDocument(QString filename);
        TextDocument(QFile* pFile);

        /** Destructor */
        virtual ~TextDocument();

    private:
        /** Displayed text */
        QTextDocument* _currentDocument;

        /** Text from OCR */
        QTextDocument* _ocrDocument;

        /** True if current text is saved in a file */
        bool _saved;

    public:
        /** Get current document */
        QTextDocument* getCurrentDocument();

        /** Get current text */
        QString getCurrentText();

        /** Get document from the OCR */
        QTextDocument* getOcrDocument();

        /** Get text from OCR */
        QString getOcrText();

        /** True if current content has been saved in a file, False otherwise */
        bool isSaved();


        /** Load content from file */
        void loadContent();


        /** Set current document */
        void setCurrentDocument(QTextDocument* document);

        /** Set current text */
        void setCurrentText(QString str);

        /** Set document from OCR */
        void setOcrDocument(QTextDocument* document);

        /** Set text from OCR */
        void setOcrText(QString str);

        /** Set saved property */
        void setSaved(bool saved=true);
};



#endif

