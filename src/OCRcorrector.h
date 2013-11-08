#ifndef OCRCorrector_h
#define OCRCorrector_h


#include <QObject>
#include <QTextEdit>
#include <QTextCodec>
#include <QScrollArea>
#include <QSplitter>
#include <QLabel>
#include <QToolButton>
#include <QPushButton>
#include <QFontComboBox>
#include <QHBoxLayout>
#include <QHBoxLayout>
#include <QMainWindow>


// Config
#include "Config.h"

// Document
#include "DocumentList.h"

// Correction
#include "Corrector.h"
#include "Dictionary.h"

// OCR
#include "OCR.h"

// Resources
#include "Resources.h"




// Char displayed after filename if unsaved
#define CHAR_UNSAVED "*"

// Font size
#define FONT_MIN_SIZE 6
#define FONT_MAX_SIZE 48

#define TIME_MSG 2000

/******************************************************************************/
class Word
{
    public:
        Word(QString str, bool isItalic, bool isNewLine);
        ~Word();

    private:
        QString _str;
        bool _italic;
        bool _newLine;

    public:
        QString getString();
        bool isItalic();
        bool isNewLine();
};
/******************************************************************************/



// Software Configuration
extern QSharedPointer<Config> gConfig;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    // Constructor and Destructor
    public:
        /** Default constructor */
        MainWindow();

        /** Default destructor */
        ~MainWindow();


    // Methods
    public:
        /** Add a 'browse' button when no image is displayed */
        void addBrowseButton();

        /** Display a popup to confirm file deletion */
        bool confirmDelete();

        /** Crete menu bar */
        void createMenuBar();

        /** Create splitter */
        void createSplitter();

        /** Create status bar */
        void createStatusBar();

        /** Create toolbar */
        void createToolBar();

        /** Delete the 'browse" button when an image is displayed */
        void deleteBrowseButton();

        /** Display current image */
        void displayCurrentImage();

        /** Display current text document */
        void displayCurrentText();

        /** Check if text may be save */
        bool maybeSave();

        /** Refresh menu buttons */
        void refreshMenu();

        /** Refresh toolbar buttons */
        void refreshToolbar();

        /** Save text */
        void saveFile(QString filename, QByteArray format);

        /** Set current image filename in status bar */
        void setStatusImageFilename(QString text);

        /** Set current text filename in status bar */
        void setStatusTextFilename(QString filename);

    public slots:
        /** Add the word selected by cursor to Dictionary of names */
        void addToNamesDictionary();

        /** Add the word selected by cursor to Dictionary of nouns */
        void addToNounsDictionary();

        /** Center text in the textedit */
        void alignCenter();

        /** Justify text in the textedit */
        void alignJustify();

        /** Align left text in the textedit */
        void alignLeft();

        /** Align right text in the textedit */
        void alignRight();

        /** Set text as bold */
        void applyBold(bool);

        /** Close current document */
        void close();

        /** Delete current text file */
        void deleteFile();

        /** Display 'about' window */
        void displayAbout();

        /** Edit correction preferences*/
        void editCorrectionPreferences();

        /** Edit OCR preferences*/
        void editOCRPreferences();

        /** Export current text to Open Document format */
        void exportODT();

        /** Insert a char at the current position in the textedit */
        void insertChar();

        /** Learn from correction*/
        //~ void learnFromText();

        /** Merge text files */
        void mergeFiles();

        /** Go to next document */
        void next();

        /** Open text files */
        void openFiles();

        /** Open images */
        void openImages();

        /** Go to previous document */
        void previous();

        /** Refresh font informations in toolbar */
        void refreshFontInformations();

        /** Refresh the image part */
        void refreshImage();

        /** Refresh status bar */
        void refreshStatusBar();

        /** Refresh the text part */
        void refreshText();

        /** Refresh whole user interface */
        void refreshUI();

        /** Reload dictionaries */
        void reloadDictionaries();

        /** Called on resize */
        void resizeEvent(QResizeEvent* event);

        /** Apply automatic correction on multiple documents */
        void runGroupCorrection();

        /** OCRize several documents and display generated texts */
        void runGroupOCR();

        /** Apply automatic correction on a single document */
        void runSingleCorrection();

        /** OCRize a single document and display generated text */
        void runSingleOCR();

        /** Save a file */
        void save();

        /** Save a file under a new name */
        void saveAs();

        /** Set current font */
        void setFont(const QString font);

        /** Set current font size */
        void setFontSize(const QString size);

        /** Set current text document as 'saved' */
        void setSaved();

        /** Set current text document as 'unsaved'*/
        void setUnsaved();

        /** Set a multiple document view */
        void setViewMdi();

        /** Set a single document view */
        void setViewSdi();

        /** Context */
        void showContextMenu(const QPoint& position);

        /** Put current chars to lowercase */
        void toLower();

        /** Put current chars to uppercase */
        void toUpper();


    // Signals
    signals:
        /** Signal triggered when the image part of the document is modified */
        void imageChange();

        /** Signal triggered when the text part of the document is modified */
        void textChange();

        /** Signal triggered when the UI needs to be refreshed */
        void uiChange();


    // Attributes
    private:
        // OCR
        OCR _ocrManager;


        /** List of all documents currently treated */
        DocumentList _documents;

        /** Text area */
        QTextEdit* _textEdit;

        /** Image area */
        QScrollArea* _scrollArea;

        /** Splitter */
        QSplitter* _splitter;

        /** Default path when loading a file */
        QString _defaultPath;

        /** Container of textfilename in the status bar */
        QLabel* _statusTextFilename;

        /** Container of image filename in the status bar */
        QLabel* _statusImageFilename;

        /** Layout displaying image */
        QHBoxLayout* _subLayout;

        /** Browse button in the image part */
        QPushButton* _browseButton;

        /** Label containing image */
        QLabel* _imgLabel;

        // Menu
        QAction* closeFile;
        QAction* open_img;
        QAction* deleteTextFile;
        QAction* sdi;
        QAction* mdi;
        QAction* merge;
        QAction *run_ocr, *run_gocr, *_ocrPreferences;
        QAction *_runCorr, *_runGroupCorr, *_correctionPreferences;
        QAction *bold, *italic, *underline;

        // Toolbar
        QFontComboBox* fontbox;
        QComboBox* sizebox;
        QToolButton* toolbarBold;
        QToolButton* toolbarItalic;
        QToolButton* toolbarUnderline;
        QToolButton* prevButton;
        QToolButton* nextButton;
        QPushButton* _buttonLearnFromText;

        // Correction
        QSharedPointer<Dictionary> _dictionary, _dictionaryNames;

    private:
        /** Merge texts */
        QString _mergeTexts(DocumentList& documents);
};



#endif
