#ifndef OCR_h
#define OCR_h


#include <QtCore>


class OCR
{
    public:
        enum Engine
        {
            CUNEIFORM,
            TESSERACT,
            BOTH
        };

        enum Language
        {
            ENGLISH,
            FRENCH
        };

        static const int TIMEOUT = 60000;
        static const QString CUNEIFORM_CMD;
        static const QString TESSERACT_CMD;


    // Constructor and Destructor
    public:
        /** Default constructor */
        OCR();

        /** Default destructor */
        ~OCR();


    // Attributes
    private:
        /** OCR Engine */
        OCR::Engine _engine;

        /** OCR format */
        QString _format;

        /** OCR Language */
        OCR::Language _language;


    // Methods
    public:
        /** Get the current OCR engine */
        OCR::Engine getEngine();

        /** Get the current output format */
        QString getFormat();

        /** Get the current OCR Language */
        OCR::Language getLanguage();

        /** Post Treat an OCRized file. Change font and font size.
         *
         *  @param inFilename Input filename.
         *  @param font Font to be applied.
         *  @param fontSize Font size to be applied.
         */
        bool postTreat(QString inFilename, QString font, int fontSize);

        /** OCRize a file
         *
         *  @param inFilename Name of the file to be OCRized.
         *  @param outFilename Output filename.
         *
         *  @return True in case of success, False otherwise.
         */
        bool run(const QString inFilename, const QString outFilename);

        /** Select an OCR engine */
        void setEngine(OCR::Engine ocrEngine);
        void setEngine(QString engineName);

        /** Select the output format */
        void setFormat(QString ocrFormat);

        /** Select an OCR language */
        void setLanguage(OCR::Language ocrLanguage);
        void setLanguage(QString languageName);

    private:
        /** Generate a unix command for OCR */
        QString _generateCommand(QString inFilename, QString outFilename);

        /** Return the language code corresponding to 'Language' */
        QString _generateLanguageCode(OCR::Language language);
};



#endif
