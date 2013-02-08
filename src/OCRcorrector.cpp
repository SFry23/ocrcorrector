#include "OCRcorrector.h"
#include "Config.h"
#include "Dictionary.h"
#include "QStringHtml.h"
#include "DialogPreferencesOcr.h"
#include "DialogPreferencesCorrection.h"
#include "Utils.h"



//------------------------------------------------------------------------------
//  MainWindow::MainWindow()
//------------------------------------------------------------------------------
MainWindow::MainWindow() : QMainWindow()
{
    // Window title
    setWindowTitle(APP_NAME);

    // Resize screen
    const QRect screen = QDesktopWidget().screenGeometry(QDesktopWidget().primaryScreen());
    resize(screen.size() / 1.5);

    // Create text area
    _textEdit = new TextEdit(this);
    _textEdit->setContextMenuPolicy(Qt::CustomContextMenu);

    // Create image area
    _scrollArea = new QScrollArea(this);
    _imgLabel = 0;

    // Load settings
    gConfig->read();

    // Create bars
    createMenuBar();
    createToolBar();
    createStatusBar();

    // Create splitter
    createSplitter();

    // Initialize view
    setViewMdi();

    // Initialize OCR manager
    _ocrManager = OCR();

    // Load dictionaries
    _dictionary = QSharedPointer<Dictionary>(new Dictionary(gConfig->pathDic));
    _dictionaryNames = QSharedPointer<Dictionary>(new Dictionary(gConfig->pathDicNames));

    // Connections
    connect(_textEdit, SIGNAL(selectionChanged()), this, SLOT(refreshFontInformations()));
    connect(_textEdit, SIGNAL(textChanged()), this, SLOT(setUnsaved()));
    connect(_textEdit, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

    connect(this, SIGNAL(imageChange()), this, SLOT(refreshImage()));
    connect(this, SIGNAL(textChange()), this, SLOT(refreshText()));
    connect(this, SIGNAL(uiChange()), this, SLOT(refreshUI()));
}

//------------------------------------------------------------------------------
//  MainWindow::~MainWindow()
//------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    // Save settings
    gConfig->write();

    // Delete status bar
    deleteBrowseButton();

    // Save and delete dictionaries
    if (not _dictionary.isNull() and not _dictionary->save())
        QMessageBox::critical(this, tr("ERREUR"), QString(tr("Impossible de sauvegarder le dictionnaire sous '%1' !")).arg(_dictionary->getPath()));

    if (not _dictionaryNames.isNull() and not _dictionaryNames->save())
        QMessageBox::critical(this, tr("ERREUR"), QString(tr("Impossible de sauvegarder le dictionnaire sous '%1' !")).arg(_dictionaryNames->getPath()));
}



//------------------------------------------------------------------------------
//  MainWindow::addBrowseButton()
//------------------------------------------------------------------------------
void MainWindow::addBrowseButton()
{
    if (_scrollArea->layout() == 0)
    {
        // Remove previous image
        _scrollArea->takeWidget();

        // Create button
        _browseButton = new QPushButton(tr("Parcourir..."));

        // Display 'browse' button which load an image
        _subLayout = new QHBoxLayout();
        _subLayout->addWidget(_browseButton);
        _scrollArea->setLayout(_subLayout);

        connect(_browseButton, SIGNAL(clicked()), this, SLOT(openImages()));
    }
}

//------------------------------------------------------------------------------
//  MainWindow::confirmDelete()
//------------------------------------------------------------------------------
bool MainWindow::confirmDelete()
{
    QMessageBox::StandardButton msgBox = QMessageBox::critical(this,
        tr("Supprimer fichier"),
        QString("Êtes-vous sûr(e) de vouloir supprimer complètement le fichier '%1' ?").arg(_documents.getTextFilename()),
        QMessageBox::Yes | QMessageBox::No);

    if (msgBox == QMessageBox::Yes)
        return true;
    else
        return false;
}

//------------------------------------------------------------------------------
//  MainWindow::createMenuBar()
//------------------------------------------------------------------------------
void MainWindow::createMenuBar()
{
    QMenu* m_file = menuBar()->addMenu(tr("&Fichier"));
    QMenu* m_ocr = menuBar()->addMenu(tr("&OCR"));
    QMenu* m_correction = menuBar()->addMenu(tr("&Correction"));
    QMenu* m_insert = menuBar()->addMenu(tr("&Insérer"));
    QMenu* m_mode = menuBar()->addMenu(tr("&Mode"));
    QMenu* m_format = menuBar()->addMenu(tr("Fo&rmat"));
    QMenu* m_help = menuBar()->addMenu(tr("&Aide"));


                     //       FILE       //
                     //       ====       //
    // Open file
    QAction *open_file = m_file->addAction(tr("Ouvrir"));
    open_file->setShortcut(QKeySequence("Ctrl+O"));
    open_file->setStatusTip(tr("Ouvrir un ou plusieurs fichier(s) texte"));
    connect(open_file, SIGNAL(triggered()), this, SLOT(openFiles()));

    // Open image
    open_img = m_file->addAction(tr("Ouvrir image(s)"));
    open_img->setShortcut(QKeySequence("Ctrl+Shift+O"));
    open_img->setStatusTip(tr("Ouvrir un ou plusieurs scan(s)"));
    connect(open_img, SIGNAL(triggered()), this, SLOT(openImages()));

    // Save file
    QAction *save = m_file->addAction(tr("Enregistrer"));
    save->setShortcut(QKeySequence("Ctrl+S"));
    save->setStatusTip(tr("Enregistrer le fichier courant"));
    connect(save, SIGNAL(triggered()), this, SLOT(save()));

    // Save file under...
    QAction *save_under = m_file->addAction(tr("Enregistrer sous..."));
    save_under->setShortcut(QKeySequence("Ctrl+Shift+S"));
    save_under->setStatusTip(tr("Enregistrer le fichier sous un nouveau nom"));
    connect(save_under, SIGNAL(triggered()), this, SLOT(saveAs()));

    // Export
    QAction *exp = m_file->addAction(tr("Exporter en ODT"));
    exp->setStatusTip(tr("Enregistrer le fichier au format ODT"));
    connect(exp, SIGNAL(triggered()), this, SLOT(exportODT()));

    // Merge files
    merge = m_file->addAction(tr("Fusionner"));
    merge->setShortcut(QKeySequence("Ctrl+M"));
    merge->setStatusTip(tr("Fusionner plusieurs documents"));
    merge->setEnabled(false);
    connect(merge, SIGNAL(triggered()), this, SLOT(mergeFiles()));

    // Close
    closeFile = m_file->addAction(tr("Fermer"));
    closeFile->setShortcut(QKeySequence("Ctrl+W"));
    closeFile->setStatusTip(tr("Fermer le document (texte + image) courant"));
    closeFile->setEnabled(false);
    connect(closeFile, SIGNAL(triggered()), this, SLOT(close()));

    // Delete file
    deleteTextFile = m_file->addAction(tr("Supprimer"));
    deleteTextFile->setStatusTip(tr("Effacer le fichier courant du disque dur"));
    deleteTextFile->setEnabled(false);
    connect(deleteTextFile, SIGNAL(triggered()), this, SLOT(deleteFile()));

    // Quit
    QAction *quit = m_file->addAction(tr("Quitter"));
    quit->setShortcut(QKeySequence("Ctrl+Q"));
    quit->setStatusTip(tr("Quitter OCRcorrector"));
    connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));


                     //       OCR       //
                     //       ===       //
    // Run OCR
    run_ocr = m_ocr->addAction(tr("Lancer"));
    run_ocr->setShortcut(QKeySequence("Ctrl+T"));
    run_ocr->setStatusTip(tr("Lancer OCR (cuneiform)"));
    run_ocr->setEnabled(false);
    connect(run_ocr, SIGNAL(triggered()), this, SLOT(runSingleOCR()));

    // Run group OCR
    run_gocr = m_ocr->addAction(tr("Lancement groupé"));
    run_gocr->setShortcut(QKeySequence("Ctrl+Shift+T"));
    run_gocr->setStatusTip(tr("Lancer l'OCR sur toutes les images ouvertes"));
    run_gocr->setEnabled(false);
    connect(run_gocr, SIGNAL(triggered()), this, SLOT(runGroupOCR()));

    // Preferences
    _ocrPreferences = m_ocr->addAction(tr("Préférences"));
    _ocrPreferences->setStatusTip(tr("Éditer les préférences de reconnaissance des caractères"));
    connect(_ocrPreferences, SIGNAL(triggered()), this, SLOT(editOCRPreferences()));


                   //       CORRECTION       //
                   //       ==========       //
    // Run correction
    _runCorr = m_correction->addAction(tr("Lancer"));
    _runCorr->setShortcut(QKeySequence("Ctrl+R"));
    _runCorr->setStatusTip(tr("Lancer correction"));
    connect(_runCorr, SIGNAL(triggered()), this, SLOT(runSingleCorrection()));

    // Run group correction
    _runGroupCorr = m_correction->addAction(tr("Lancement groupé"));
    _runGroupCorr->setShortcut(QKeySequence("Ctrl+Shift+R"));
    _runGroupCorr->setStatusTip(tr("Lancer la correction sur tous les documents"));
    _runGroupCorr->setEnabled(false);
    connect(_runGroupCorr, SIGNAL(triggered()), this, SLOT(runGroupCorrection()));

    // Preferences
    _correctionPreferences = m_correction->addAction(tr("Préférences"));
    _correctionPreferences->setStatusTip(tr("Éditer les préférences de correction"));
    connect(_correctionPreferences, SIGNAL(triggered()), this, SLOT(editCorrectionPreferences()));


                    //       INSERTION      //
                    //       =========      //
    // Open quote
    QAction* oq = m_insert->addAction(QString("«") + "    " + tr("guillemet gauche"));
    oq->setStatusTip(tr("Insérer un guillemet ouvrant à la position du curseur"));
    connect(oq, SIGNAL(triggered()), this, SLOT(insertChar()));

    // Close quote
    QAction* cq = m_insert->addAction(QString("»") + "    " + tr("guillemet droit"));
    cq->setStatusTip(tr("Insérer un guillemet fermant à la position du curseur"));
    connect(cq, SIGNAL(triggered()), this, SLOT(insertChar()));

    // Cadratin
    QAction* cadratin = m_insert->addAction(QString("—") + "    " + tr("cadratin"));
    cadratin->setStatusTip(tr("Insérer un cadratin à la position du curseur"));
    connect(cadratin, SIGNAL(triggered()), this, SLOT(insertChar()));

    // Semi cadratin
    QAction* sc = m_insert->addAction(QString("–") + "    " + tr("demi-cadratin"));
    sc->setStatusTip(tr("Insérer un demi-cadratin à la position du curseur"));
    connect(sc, SIGNAL(triggered()), this, SLOT(insertChar()));

    // Suspension point
    QAction* sp = m_insert->addAction(QString("…") + "    " + QString("points de suspension"));
    sp->setStatusTip(tr("Insérer des points de supsension à la position du curseur"));
    connect(sp, SIGNAL(triggered()), this, SLOT(insertChar()));


                        //       MODE      //
                        //       ====      //
    // Single view (SDI)
    sdi = m_mode->addAction(tr("Vue Simple"));
    sdi->setStatusTip(tr("Afficher uniquement le texte à corriger"));
    sdi->setCheckable(true);
    connect(sdi, SIGNAL(triggered()), this, SLOT(setViewSdi()));

    // Dual view (MDI)
    mdi = m_mode->addAction(tr("Vue Relecture"));
    mdi->setStatusTip(tr("Afficher le texte à corriger et l'image source côte à côte"));
    mdi->setCheckable(true);
    connect(mdi, SIGNAL(triggered()), this, SLOT(setViewMdi()));


                       //       FORMAT      //
                       //       ======      //
   // Bold
    bold = m_format->addAction(tr("Gras"));
    bold->setShortcut(QKeySequence("Ctrl+G"));
    bold->setStatusTip(tr("Applique le formatage gras à la sélection"));
    bold->setCheckable(true);
    connect(bold, SIGNAL(toggled(bool)), this, SLOT(applyBold(bool)));

    // Italic
    italic = m_format->addAction(tr("Italique"));
    italic->setShortcut(QKeySequence("Ctrl+I"));
    italic->setStatusTip(tr("Applique le formatage italique à la sélection"));
    italic->setCheckable(true);
    connect(italic, SIGNAL(toggled(bool)), _textEdit, SLOT(setFontItalic(bool)));

    // Underline
    underline = m_format->addAction(tr("Souligné"));
    underline->setShortcut(QKeySequence("Ctrl+U"));
    underline->setStatusTip(tr("Souligne la sélection"));
    underline->setCheckable(true);
    connect(underline, SIGNAL(toggled(bool)), _textEdit, SLOT(setFontUnderline(bool)));

    // To lower
    QAction* to_lower = m_format->addAction(tr("minuscules"));
    to_lower->setShortcut(QKeySequence("Ctrl+L"));
    to_lower->setStatusTip(tr("Toutes les lettres du mot deviennent des minuscules"));
    connect(to_lower, SIGNAL(triggered()), this, SLOT(toLower()));

    // To upper
    QAction* to_upper = m_format->addAction(tr("MAJUSCULES"));
    to_upper->setShortcut(QKeySequence("Ctrl+Shift+L"));
    to_upper->setStatusTip(tr("Toutes les lettres du mot deviennent des majuscules"));
    connect(to_upper, SIGNAL(triggered()), this, SLOT(toUpper()));


                        //       HELP      //
                        //       ====      //
    // About
    QAction* about = m_help->addAction(tr("À propos"));
    about->setStatusTip(tr("Afficher l'à propos"));
    connect(about, SIGNAL(triggered()), this, SLOT(displayAbout()));
}

//------------------------------------------------------------------------------
//  MainWindow::createSplitter()
//------------------------------------------------------------------------------
void MainWindow::createSplitter()
{
    // Create splitter
    _splitter = new QSplitter(this);

    // Define as central widget
    setCentralWidget(_splitter);

    // Add left widget (_textEdit)
    _splitter->addWidget(_textEdit);

    // Add right widget (image)
    _scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _splitter->addWidget(_scrollArea);

    // Split screen in two equal parts
    QList<int> sizes;
    sizes.append(window()->width() / 2);
    sizes.append(window()->width() / 2);
    _splitter->setSizes(sizes);

    // Handle movement
    connect(_splitter, SIGNAL(splitterMoved(int, int)), SLOT(refreshImage()));
}

//------------------------------------------------------------------------------
//  MainWindow::createStatusBar()
//------------------------------------------------------------------------------
void MainWindow::createStatusBar()
{
    _statusTextFilename = new QLabel(this);
    _statusImageFilename = new QLabel(this);

    statusBar()->addWidget(_statusTextFilename);
    statusBar()->addPermanentWidget(_statusImageFilename);

    statusBar()->showMessage(tr("Prêt"), TIME_MSG);
}

//------------------------------------------------------------------------------
//  MainWindow::createToolBar()
//------------------------------------------------------------------------------
void MainWindow::createToolBar()
{
    // Properties
    QToolBar *toolbar = addToolBar("Barre_outils");
    toolbar->setFloatable(false);

    // Font selection
    fontbox = new QFontComboBox(this);
    fontbox->setCurrentFont(QFont(gConfig->font));
    toolbar->addWidget(fontbox);
    connect(fontbox, SIGNAL(currentIndexChanged(const QString)),
            this, SLOT(setFont(const QString)));

    // Font size selection
    sizebox = new QComboBox(this);
    for (int i = FONT_MIN_SIZE; i <= FONT_MAX_SIZE; i++)
    {
        char int_str[16];
        sprintf(int_str, "%d", i);
        sizebox->insertItem(i, int_str);
    }
    connect(sizebox, SIGNAL(currentIndexChanged(const QString)),
                this, SLOT(setFontSize(const QString)));

    // Set normal size
    if (gConfig->fontSize > 0)
        sizebox->setCurrentIndex(gConfig->fontSize - FONT_MIN_SIZE);
    else
        sizebox->setCurrentIndex(FONT_MIN_SIZE);

    toolbar->addWidget(sizebox);

    // Bold
    toolbarBold = new QToolButton(toolbar);
    toolbarBold->setIcon(QIcon(":/bold.png"));
    toolbarBold->setCheckable(true);
    toolbar->addWidget(toolbarBold);
    connect(toolbarBold, SIGNAL(toggled(bool)), this, SLOT(applyBold(bool)));

    // Italic
    toolbarItalic = new QToolButton(toolbar);
    toolbarItalic->setIcon(QIcon(":/italic.png"));
    toolbarItalic->setCheckable(true);
    toolbar->addWidget(toolbarItalic);
    connect(toolbarItalic, SIGNAL(toggled(bool)), _textEdit, SLOT(setFontItalic(bool)));

    // Underline
    toolbarUnderline = new QToolButton(toolbar);
    toolbarUnderline->setIcon(QIcon(":/underline.png"));
    toolbarUnderline->setCheckable(true);
    toolbar->addWidget(toolbarUnderline);
    connect(toolbarUnderline, SIGNAL(toggled(bool)), _textEdit, SLOT(setFontUnderline(bool)));

    // Align left
    QToolButton *align_left = new QToolButton(toolbar);
    align_left->setIcon(QIcon(":/align_left.png"));
    toolbar->addWidget(align_left);
    connect(align_left, SIGNAL(clicked()), this, SLOT(alignLeft()));

    // Align center
    QToolButton *align_center = new QToolButton(toolbar);
    align_center->setIcon(QIcon(":/align_center.png"));
    toolbar->addWidget(align_center);
    connect(align_center, SIGNAL(clicked()), this, SLOT(alignCenter()));

    // Align right
    QToolButton *align_right = new QToolButton(toolbar);
    align_right->setIcon(QIcon(":/align_right.png"));
    toolbar->addWidget(align_right);
    connect(align_right, SIGNAL(clicked()), this, SLOT(alignRight()));

    // Align justify
    QToolButton *align_justify = new QToolButton(toolbar);
    align_justify->setIcon(QIcon(":/align_justify.png"));
    toolbar->addWidget(align_justify);
    connect(align_justify, SIGNAL(clicked()), this, SLOT(alignJustify()));

    // Learn from text
    //~ _buttonLearnFromText = new QPushButton(toolbar);
    //~ _buttonLearnFromText->setText(tr("Apprendre"));
    //~ toolbar->addWidget(_buttonLearnFromText);
    //~ _buttonLearnFromText->setEnabled(false);
    //~ connect(_buttonLearnFromText, SIGNAL(clicked()), this, SLOT(learnFromText()));

    // Previous file
    prevButton = new QToolButton(toolbar);
    prevButton->setIcon(QIcon(":/arrow_left.png"));
    prevButton->setEnabled(false);
    toolbar->addWidget(prevButton);
    connect(prevButton, SIGNAL(clicked()), this, SLOT(previous()));

    // Next file
    nextButton = new QToolButton(toolbar);
    nextButton->setIcon(QIcon(":/arrow_right.png"));
    nextButton->setEnabled(false);
    toolbar->addWidget(nextButton);
    connect(nextButton, SIGNAL(clicked()), this, SLOT(next()));
}

//------------------------------------------------------------------------------
//  MainWindow::deleteBrowseButton()
//------------------------------------------------------------------------------
void MainWindow::deleteBrowseButton()
{
    if (_scrollArea->layout() != 0)
    {
        _scrollArea->layout()->removeWidget(_browseButton);
        delete _browseButton;
        delete _subLayout;
    }
}

//------------------------------------------------------------------------------
//  MainWindow::displayCurrentImage()
//------------------------------------------------------------------------------
void MainWindow::displayCurrentImage()
{
    if (_documents.getImageFile() != 0)
    {
        QPixmap img = QPixmap(_documents.getImageFilename());

        if (not img.isNull())
        {
            // Scale image
            int width = _splitter->sizes()[1];

            img = img.scaledToWidth(width, Qt::SmoothTransformation);

            // Create container
            _imgLabel = new QLabel(this);
            _imgLabel->setPixmap(img);

            // Display on right part of the layout
            _scrollArea->setWidget(_imgLabel);

            // UI has to be modified
            emit uiChange();
        }
        else
        {
            // Critical messagebox
            QMessageBox::critical(this, tr("ERREUR"), tr("Format de l'image non supporté !"));

            // Delete previous image
            _scrollArea->takeWidget();
        }
    }
}

//------------------------------------------------------------------------------
//  MainWindow::displayCurrentText()
//------------------------------------------------------------------------------
void MainWindow::displayCurrentText()
{
    // Load file
    TextDocument *textFile = _documents.getTextFile();

    if (textFile != 0)
    {
        QTextDocument* document = textFile->getCurrentDocument();

        if (document == 0)
            textFile->loadContent();
        else
            _textEdit->setDocument(document);

        if (textFile->isSaved())
            setSaved();
        else
        {
            setUnsaved();
            textFile->setSaved(true);
        }
    }
    else
    {
        _textEdit->clear();
    }
}

//------------------------------------------------------------------------------
//  MainWindow::maybeSave()
//------------------------------------------------------------------------------
bool MainWindow::maybeSave()
{
    if (_textEdit->document()->isModified())
    {
        QMessageBox::StandardButton ret = QMessageBox::information(this,
                tr("Changements non sauvegardés"),
                tr("Attention : Les changements apportés au document n'ont pas été sauvegardés !"),
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (ret == QMessageBox::Save)
            save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }

    return true;
}

//------------------------------------------------------------------------------
//  MainWindow::refreshMenu()
//------------------------------------------------------------------------------
void MainWindow::refreshMenu()
{
    // Enable or disable 'close' button
    closeFile->setEnabled(_documents.size() > 0);

    // Enable or disable 'delete' button
    deleteTextFile->setEnabled(_documents.getTextFile() != 0);

    // Enable or disable OCR
    run_ocr->setEnabled(_documents.getImageFile() != 0 and mdi->isChecked());
    run_gocr->setEnabled(_documents.size() > 1 and mdi->isChecked());

    // Enable or disable correction
    _runGroupCorr->setEnabled(_documents.size() > 1);

    // Enable or disable 'merge' button
    merge->setEnabled(_documents.size() > 1);
}

//------------------------------------------------------------------------------
//  MainWindow::refreshToolbar()
//------------------------------------------------------------------------------
void MainWindow::refreshToolbar()
{
    // Enable or disable 'Learn' button in the toolbar
    //~ if (_documents.getTextFile() != 0)
        //~ _buttonLearnFromText->setEnabled(not _documents.getTextFile()->getOCRText().isEmpty());

    // Enable or disable prevButton in the toolbar
    prevButton->setEnabled(_documents.hasPrevious(sdi->isChecked()));

    // Enable or diable nextButton in the toolbar
    nextButton->setEnabled(_documents.hasNext(sdi->isChecked()));
}

//------------------------------------------------------------------------------
//  MainWindow::saveFile()
//------------------------------------------------------------------------------
void MainWindow::saveFile(QString filename, QByteArray format="HTML")
{
    // Check if filename is valid
    if (not filename.isEmpty())
    {
        // Modify HTML
        TextEdit* tmpTextEdit = new TextEdit();
        tmpTextEdit->setHtml(_textEdit->toHtml());

        tmpTextEdit->removeUnderline();
        tmpTextEdit->removeHighlight();
        tmpTextEdit->replaceMeta();

        // Create a writer
        QTextDocumentWriter *doc = new QTextDocumentWriter(filename, format);

        // Save
        if (doc->write(tmpTextEdit->document()))
        {
            // Set document as saved
            setSaved();
            statusBar()->showMessage(tr("Fichier enregistré"), TIME_MSG);
        }

        delete doc;
        delete tmpTextEdit;
    }
}

//------------------------------------------------------------------------------
//  MainWindow::setStatusImageFilename()
//------------------------------------------------------------------------------
void MainWindow::setStatusImageFilename(QString filename)
{
    statusBar()->removeWidget(_statusImageFilename);

    if (_statusImageFilename == 0)
        _statusImageFilename = new QLabel(this);

    _statusImageFilename->setText(filename);
    _statusImageFilename->show();
    statusBar()->addPermanentWidget(_statusImageFilename);
}

//------------------------------------------------------------------------------
//  MainWindow::setStatusTextFilename()
//------------------------------------------------------------------------------
void MainWindow::setStatusTextFilename(QString filename)
{
    _statusTextFilename->setText(filename);
}








//------------------------------------------------------------------------------
//
//                                  SLOTS
//

//------------------------------------------------------------------------------
//  MainWindow::addToNamesDictionary()
//------------------------------------------------------------------------------
void MainWindow::addToNamesDictionary()
{
    QTextCursor cursor = _textEdit->textCursor();
    _dictionaryNames->add(cursor.selectedText());
    cursor.clearSelection();
}

//------------------------------------------------------------------------------
//  MainWindow::addToNounsDictionary()
//------------------------------------------------------------------------------
void MainWindow::addToNounsDictionary()
{
    QTextCursor cursor = _textEdit->textCursor();
    _dictionary->add(cursor.selectedText());
    cursor.clearSelection();
}

//------------------------------------------------------------------------------
//  MainWindow::alignCenter()
//------------------------------------------------------------------------------
void MainWindow::alignCenter()
{
    _textEdit->setAlignment(Qt::AlignCenter);
}

//------------------------------------------------------------------------------
//  MainWindow::alignJustify()
//------------------------------------------------------------------------------
void MainWindow::alignJustify()
{
    _textEdit->setAlignment(Qt::AlignJustify);
}

//------------------------------------------------------------------------------
//  MainWindow::alignLeft()
//------------------------------------------------------------------------------
void MainWindow::alignLeft()
{
    _textEdit->setAlignment(Qt::AlignLeft);
}

//------------------------------------------------------------------------------
//  MainWindow::alignRight()
//------------------------------------------------------------------------------
void MainWindow::alignRight()
{
    _textEdit->setAlignment(Qt::AlignRight);
}

//------------------------------------------------------------------------------
//  MainWindow::applyBold()
//------------------------------------------------------------------------------
void MainWindow::applyBold(bool bold)
{
    if (bold)
        _textEdit->setFontWeight(QFont::Bold);
    else
        _textEdit->setFontWeight(QFont::Normal);
}

//------------------------------------------------------------------------------
//  MainWindow::close()
//------------------------------------------------------------------------------
void MainWindow::close()
{
    // Ask for saving
    if (maybeSave())
    {
        _textEdit->setDocument(new QTextDocument(_textEdit));

        if (not _documents.isLocked())
            _documents.removeCurrent();
        else
            _documents.clear();

        // Unlock document
        _documents.setLocked(false);

        emit textChange();
        emit imageChange();
        emit uiChange();
    }
}

//------------------------------------------------------------------------------
//  MainWindow::deleteFile()
//------------------------------------------------------------------------------
void MainWindow::deleteFile()
{
    TextDocument* file = _documents.getTextFile();

    if (file != 0)
    {
        if (confirmDelete())
        {
            // Delete file
            file->remove();

            // Close current document
            close();
        }
    }
}

//------------------------------------------------------------------------------
//  MainWindow::displayAbout()
//------------------------------------------------------------------------------
void MainWindow::displayAbout()
{
    // Title
    QString title = tr("À propos d'") + APP_NAME;

    // Version
    QString version = tr("Version : ") + APP_VERSION;

    // License
    QString license = tr("Licence : ") + LICENSE;

    // Contact
    QString contact = tr("Contact : ") + APP_MAIL;

    // Create dialog
    QString content = version + "\n" + license + "\n" + contact;
    QMessageBox::about(this, title, content);
}

//------------------------------------------------------------------------------
//  MainWindow::editCorrectionPreferences()
//------------------------------------------------------------------------------
void MainWindow::editCorrectionPreferences()
{
    DialogPreferencesCorrection *dialog = new DialogPreferencesCorrection(this);

    connect(dialog, SIGNAL(dicChanged()), this, SLOT(reloadDictionaries()));

    dialog->exec();

    delete dialog;
}

//------------------------------------------------------------------------------
//  MainWindow::editOCRPreferences()
//------------------------------------------------------------------------------
void MainWindow::editOCRPreferences()
{
    DialogPreferencesOcr *dialog = new DialogPreferencesOcr(this);
    dialog->exec();

    delete dialog;
}

//------------------------------------------------------------------------------
//  MainWindow::exportODT()
//------------------------------------------------------------------------------
void MainWindow::exportODT()
{
    QFileDialog *dialog = new QFileDialog(this);

    dialog->setDefaultSuffix("odt");
    dialog->setNameFilter(QString("ODT (*.odt);; ") + tr("Tout") + QString("(*)"));
    dialog->setAcceptMode(QFileDialog::AcceptSave);
    dialog->setViewMode(QFileDialog::List);

    // Set default filename
    QFile* currFile = _documents.getImageFile();

    if (currFile != 0)
    {
        QFileInfo fileInfo(*currFile);
        dialog->selectFile(fileInfo.path() + "/" + fileInfo.baseName() + ".odt");
    }
    else
    {
        dialog->selectFile(".odt");
    }

    // Save file
    if (dialog->exec())
        saveFile(dialog->selectedFiles()[0], "ODF");

    delete dialog;
}

//------------------------------------------------------------------------------
//  MainWindow::insertChar()
//------------------------------------------------------------------------------
void MainWindow::insertChar()
{
    QAction *emiter = qobject_cast<QAction*>(sender());
    _textEdit->textCursor().insertText((emiter->text().split(" ")[0]));
}

//------------------------------------------------------------------------------
//  MainWindow::learnFromText()
//------------------------------------------------------------------------------
//~ void MainWindow::learnFromText()
//~ {
    //~ // Compare texts
    //~ TextDocument* textDoc = _documents.getTextFile();
//~
    //~ // Frequency matrix
    //~ SubstitutionMatrix freqMatrix(200);
//~
    //~ if (textDoc != 0)
    //~ {
        //~ // 'Wait' cursor
        //~ QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
//~
        //~ QString wrongText = textDoc->getOcrText();
        //~ QString wrightText = _textEdit->toPlainText();
//~
        //~ QLevenshtein aligner(wrongText, wrightText);
//~
        //~ QAlignment alignment = aligner.align();
//~
        //~ QString alignedA = alignment.getStringA();
        //~ QString alignedB = alignment.getStringB();
//~
        //~ QString wordA, wordB;
//~
        //~ for (int i = 0; i < alignedA.size(); i++)
        //~ {
            //~ if (alignedA[i] == alignedB[i] and (alignedA[i] == ' ' or alignedA[i] == '\n'))
            //~ {
                //~ if (not wordA.isNull() and not wordB.isNull())
                //~ {
                    //~ if (wordA != wordB)
                    //~ {
                        //~ for (int j = 0; j < wordA.size(); j++)
                        //~ {
                            //~ int codeLetterA = charToNum(wordA[j]);
                            //~ int codeLetterB = charToNum(wordB[j]);
//~
                            //~ if (codeLetterA > 0 and codeLetterB > 0)
                            //~ {
                                //~ if (codeLetterA < freqMatrix.sizeX() and codeLetterB < freqMatrix.sizeX())
                                //~ {
                                    //~ freqMatrix(codeLetterA, codeLetterB)++;
                                    //~ freqMatrix.setFreq(codeLetterA, freqMatrix.getFreq(codeLetterA) + 1);
                                    //~ freqMatrix.setFreq(codeLetterB, freqMatrix.getFreq(codeLetterB) + 1);
                                //~ }
                            //~ }
                        //~ }
//~
                        //~ qDebug() << wordA << " " << wordB;
                    //~ }
                //~ }
//~
                //~ wordA = "";
                //~ wordB = "";
            //~ }
            //~ else
            //~ {
                //~ wordA += alignedA[i];
                //~ wordB += alignedB[i];
            //~ }
        //~ }
//~
        //~ SubstitutionMatrix S;
        //~ if (S.read(std::string("src/dic/.tesseract-substitution.txt")))
        //~ {
            //~ for (int i = 0; i < freqMatrix.sizeX(); i++)
            //~ {
                //~ for (int j = 0; j < freqMatrix.sizeX(); j++)
                //~ {
                    //~ S.updateScore(i, j, freqMatrix(i, j), freqMatrix.getFreq(i), freqMatrix.getFreq(j));
                //~ }
            //~ }
//~
            //~ S.save(std::string("src/dic/.tesseract-substitution.txt"));
        //~ }
//~
        //~ // Back to normal cursor
        //~ QApplication::restoreOverrideCursor();
    //~ }
//~ }

//------------------------------------------------------------------------------
//  MainWindow::mergeFiles()
//------------------------------------------------------------------------------
void MainWindow::mergeFiles()
{
    // Merge text
    QString newText = _mergeTexts(_documents);

    // Display text
    _textEdit->setHtml(newText);

    _documents.setLocked(true);

    emit uiChange();
}

//------------------------------------------------------------------------------
//  MainWindow::next()
//------------------------------------------------------------------------------
void MainWindow::next()
{
    if (_documents.isLocked())
    {
        _documents.next();
    }
    else if (maybeSave())
    {
        _documents.getTextFile()->loadContent();
        _documents.next();

        emit textChange();
        emit uiChange();
    }

    emit imageChange();
}

//------------------------------------------------------------------------------
//  MainWindow::openImages()
//------------------------------------------------------------------------------
void MainWindow::openImages()
{
    QString SUPPORTED_FORMATS = tr("Images") + QString(" (*.bmp *.gif *.jpg *.jpeg *.png *.pnm *.pbm *.pgm *.ppm *.tiff *.xbm *.xpm)");

    if (maybeSave())
    {
        QStringList filenames = QFileDialog::getOpenFileNames(this,
                                                        tr("Ouvrir image(s)"),
                                                        _defaultPath,
                                                        SUPPORTED_FORMATS);
        if (not filenames.isEmpty())
        {
            // Keep path as default path
            _defaultPath = filenames[0].left(filenames[0].lastIndexOf("/"));

            _documents.clear();

            for (int i = 0; i < filenames.size(); i++)
            {
                QString basename = filenames[i].left(filenames[i].lastIndexOf("."));
                QString textFilename = basename + QString(".html");

                _documents.add(textFilename, filenames[i]);

                // Load content
                TextDocument* textFile = _documents.getTextFile(i);

                if (textFile != 0)
                    textFile->loadContent();
            }

            emit imageChange();
            emit textChange();
            emit uiChange();
        }
    }
}

//------------------------------------------------------------------------------
//  MainWindow::openFiles()
//------------------------------------------------------------------------------
void MainWindow::openFiles()
{
    QStringList filenames = QFileDialog::getOpenFileNames(
                                        this,
                                        tr("Ouvrir fichier(s)"),
                                        _defaultPath,
                                        tr("Texte") + QString(" (*.txt *.html);; Tout (*)"));
    if (not filenames.isEmpty())
    {
        // Keep path as default path
        _defaultPath = filenames[0].left(filenames[0].lastIndexOf("/"));

        // Remove all current documents
        _documents.clear();
        _documents.setLocked(false);

        // Add all new documents in the list
        for (int i = 0; i < filenames.size(); i++)
        {
            _documents.add(filenames[i], "");

            // Load content
            TextDocument* textFile = _documents.getTextFile(i);

            if (textFile != 0)
                textFile->loadContent();
        }

        // Refresh UI
        emit textChange();
        emit uiChange();
    }
}

//------------------------------------------------------------------------------
//  MainWindow::previous()
//------------------------------------------------------------------------------
void MainWindow::previous()
{
    if (_documents.isLocked())
    {
        _documents.previous();
    }
    else if (maybeSave())
    {
        _documents.getTextFile()->loadContent();
        _documents.previous();

        emit textChange();
        emit uiChange();
    }

    emit imageChange();
}

//------------------------------------------------------------------------------
//  MainWindow::refreshFontInformations()
//------------------------------------------------------------------------------
void MainWindow::refreshFontInformations()
{
    // Text format at the new position
    QTextCharFormat newFormat = _textEdit->textCursor().charFormat();

    // Display informations in the toolbar
    fontbox->setCurrentFont(newFormat.font());
    sizebox->setCurrentIndex(newFormat.font().pointSize() - FONT_MIN_SIZE);

    if (! _textEdit->textCursor().hasSelection())
    {
        // Bold
        bool isBold = newFormat.font().weight() == QFont::Bold;
        bold->setChecked(isBold);
        toolbarBold->setChecked(isBold);

        // Italic
        bool isItalic = newFormat.font().italic();
        italic->setChecked(isItalic);
        toolbarItalic->setChecked(isItalic);

        // Underline
        bool isUnderline = newFormat.font().underline();
        underline->setChecked(isUnderline);
        toolbarUnderline->setChecked(isUnderline);
    }
}

//------------------------------------------------------------------------------
//  MainWindow::refreshImage()
//------------------------------------------------------------------------------
void MainWindow::refreshImage()
{
    if (_documents.getImageFile() != 0)
    {
        deleteBrowseButton();
        displayCurrentImage();
    }
    else
    {
        addBrowseButton();
        setStatusImageFilename("");
    }
}

//------------------------------------------------------------------------------
//  MainWindow::refreshStatusBar()
//------------------------------------------------------------------------------
void MainWindow::refreshStatusBar()
{
    if (_textEdit->document()->isModified())
        setStatusTextFilename(_documents.getTextFilename() + CHAR_UNSAVED);
    else
        setStatusTextFilename(_documents.getTextFilename());
}

//------------------------------------------------------------------------------
//  MainWindow::refreshText()
//------------------------------------------------------------------------------
void MainWindow::refreshText()
{
    if (not _documents.isLocked())
        displayCurrentText();
    else
        setStatusTextFilename(CHAR_UNSAVED);
}

//------------------------------------------------------------------------------
//  MainWindow::refreshUI()
//------------------------------------------------------------------------------
void MainWindow::refreshUI()
{
    refreshMenu();
    refreshToolbar();
    refreshStatusBar();

    if (mdi->isChecked())
        setStatusImageFilename(_documents.getImageFilename());
    else
        setStatusImageFilename("");
}

//------------------------------------------------------------------------------
//  MainWindow::reloadDictionaries()
//------------------------------------------------------------------------------
void MainWindow::reloadDictionaries()
{
    if (_dictionary != 0)
    {
        _dictionary->save();
        _dictionary->setPath(gConfig->pathDic);
        _dictionary->load();
    }

    if (_dictionaryNames != 0)
    {
        _dictionaryNames->save();
        _dictionaryNames->setPath(gConfig->pathDic);
        _dictionaryNames->load();
    }
}

//------------------------------------------------------------------------------
//  MainWindow::resizeEvent()
//------------------------------------------------------------------------------
void MainWindow::resizeEvent(QResizeEvent* event)
{
    refreshImage();

    // Unused parameter
    (void) event;
}

//------------------------------------------------------------------------------
//  MainWindow::runGroupCorrection()
//------------------------------------------------------------------------------
void MainWindow::runGroupCorrection()
{
    // Waint cursor
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Progression bar
    statusBar()->clearMessage();
    setStatusTextFilename("");
    QProgressBar* progressBar = new QProgressBar(this);
    statusBar()->addWidget(progressBar);
    progressBar->setRange(0, _documents.size());

    int savedIndex = _documents.getIndex();

    for (int i = 0; i < _documents.size(); i++)
    {
        progressBar->setValue(i);
        _documents.moveTo(i);

        displayCurrentText();

        _textEdit->removeBold();
        _textEdit->removeUnderline();
        _textEdit->removeHighlight();

        Corrector* corrector = new Corrector(_textEdit->document(), _dictionary, _dictionaryNames);
        corrector->setColors(gConfig->colorError, gConfig->colorCorrected, gConfig->colorReplace);
        corrector->setHighlightStyle(gConfig->highlightStyle);


        if (corrector != 0)
        {
            // Find and correct errors
            corrector->detectErrors();

            _textEdit->setDocument(corrector->getDocument());

            _documents.getTextFile()->setCurrentDocument(corrector->getDocument());
            _documents.getTextFile()->setSaved(false);

            delete corrector;
        }
    }

    // Retrieve document
    _documents.moveTo(savedIndex);

    emit textChange();

    // Remove progress bar
    statusBar()->removeWidget(progressBar);
    delete progressBar;

    // Restore cursor
    statusBar()->showMessage(tr("Correction terminée !"), TIME_MSG);
    QApplication::restoreOverrideCursor();
}

//------------------------------------------------------------------------------
//  MainWindow::runGroupOCR()
//------------------------------------------------------------------------------
void MainWindow::runGroupOCR()
{
    // Progression bar
    QProgressBar* progressBar = new QProgressBar(this);
    setStatusTextFilename("");
    //~ statusBar()->removeWidget(_statusTextFilename);
    statusBar()->addWidget(progressBar);
    progressBar->setRange(0, _documents.size());

    int savedIndex = _documents.getIndex();

    for (int i = 0; i < _documents.size(); i++)
    {
        // Progress bar
        progressBar->setValue(i+1);

        // Change document
        _documents.moveTo(i);

        QFile* pImg = _documents.getImageFile();

        if (pImg != 0)
        {
            // Filenames
            QString imgFileName = pImg->fileName();

            QFileInfo imgInfos(imgFileName);
            QString textFileName = imgInfos.path() + "/" + imgInfos.baseName() + ".html";

            // OCR settings
            _ocrManager.setEngine(gConfig->ocrEngine);
            _ocrManager.setLanguage(gConfig->ocrLanguage);

            // Run OCR
            if (_ocrManager.run(imgFileName, textFileName))
            {
                // Change font and font size
                _ocrManager.postTreat(textFileName, fontbox->currentText(), sizebox->currentText().toInt());

                // Load OCRized text
                TextDocument* document = _documents.getTextFile();

                if (document != 0)
                {
                    document->loadContent();
                    document->setOcrText(document->getCurrentText());
                }
            }
            else
            {
                qDebug() << "Échec lors de l'OCRisation de " << imgFileName;
            }
        }
        else
            _documents.getTextFile()->setCurrentText("");
    }

    // Retrieve current document
    _documents.moveTo(savedIndex);

    // Remove progress bar
    statusBar()->removeWidget(progressBar);
    //~ _statusTextFilename->show();
    //~ statusBar()->addWidget(_statusTextFilename);
    delete progressBar;

    setSaved();

    emit textChange();
}

//------------------------------------------------------------------------------
//  MainWindow::runSingleCorrection()
//------------------------------------------------------------------------------
void MainWindow::runSingleCorrection()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    statusBar()->showMessage(tr("Correction en cours..."), TIME_MSG);

    _textEdit->removeBold();
    _textEdit->removeUnderline();
    _textEdit->removeHighlight();

    Corrector* corrector = new Corrector(_textEdit->document(), _dictionary, _dictionaryNames);

    if (corrector != 0)
    {
        // Corrector settings
        corrector->setColors(gConfig->colorError, gConfig->colorCorrected, gConfig->colorReplace);
        corrector->setHighlightStyle(gConfig->highlightStyle);

        // Find and correct errors
        corrector->detectErrors();

        // Display corrected text
        _textEdit->setDocument(corrector->getDocument());

        delete corrector;
    }

    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Correction terminée !"), TIME_MSG);
}

//------------------------------------------------------------------------------
//  MainWindow::runSingleOCR()
//------------------------------------------------------------------------------
void MainWindow::runSingleOCR()
{
    statusBar()->showMessage(tr("OCR en cours..."), TIME_MSG);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    _documents.setLocked(false);

    // Run OCR engine
    QFile* pImg = _documents.getImageFile();

    if (pImg != 0)
    {
        // Filenames
        QString imgFileName = pImg->fileName();

        QFileInfo imgInfos(imgFileName);
        QString textFileName = imgInfos.path() + "/" + imgInfos.baseName() + ".html";

        // OCR settings
        _ocrManager.setEngine(gConfig->ocrEngine);
        _ocrManager.setLanguage(gConfig->ocrLanguage);

        // Run OCR
        if (_ocrManager.run(imgFileName, textFileName))
        {
            // Change font and font size
            _ocrManager.postTreat(textFileName, fontbox->currentText(), sizebox->currentText().toInt());
        }
    }

    // Display text
    TextDocument* textDocument = _documents.getTextFile();

    if (textDocument != 0)
    {
        textDocument->loadContent();
        textDocument->setOcrText(textDocument->getCurrentText());

        setSaved();

        emit textChange();
    }

    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Reconnaissance terminée !"), TIME_MSG);
}

//------------------------------------------------------------------------------
//  MainWindow::save()
//------------------------------------------------------------------------------
void MainWindow::save()
{
    if (_documents.getTextFile() == 0)
        saveAs();
    else
        saveFile(_documents.getTextFilename(), "HTML");
}

//------------------------------------------------------------------------------
//  MainWindow::saveAs()
//------------------------------------------------------------------------------
void MainWindow::saveAs()
{
    QFileDialog *dialog = new QFileDialog(this, tr("Enregistrer sous..."));
    dialog->setDefaultSuffix("html");
    dialog->setNameFilter(tr("HTML") + QString(" (*.html);; ") +
                          tr("Tout") + QString(" (*)"));

    if (_documents.getTextFile() == 0)
        dialog->selectFile(".html");
    else
        dialog->selectFile(_documents.getTextFilename());

    dialog->setAcceptMode(QFileDialog::AcceptSave);
    dialog->setViewMode(QFileDialog::List);

    if (dialog->exec())
    {
        QString filename = dialog->selectedFiles()[0];

        if (_documents.size() == 0)
            _documents.add(filename, 0);
        else
            _documents.setTextFilename(filename);

        saveFile(filename, "HTML");
    }
}

//------------------------------------------------------------------------------
//  MainWindow::setFont()
//------------------------------------------------------------------------------
void MainWindow::setFont(const QString font)
{
    _textEdit->setFontFamily(font);
    gConfig->font = font;
}

//------------------------------------------------------------------------------
//  MainWindow::setFontSize()
//------------------------------------------------------------------------------
void MainWindow::setFontSize(const QString size)
{
    _textEdit->setFontPointSize(size.toInt());
    gConfig->fontSize = size.toInt();
}

//------------------------------------------------------------------------------
//  MainWindow::setSaved()
//------------------------------------------------------------------------------
void MainWindow::setSaved()
{
    if (_textEdit->document() != 0)
    {
        _textEdit->document()->setModified(false);
        refreshStatusBar();
    }
}

//------------------------------------------------------------------------------
//  MainWindow::setUnsaved()
//------------------------------------------------------------------------------
void MainWindow::setUnsaved()
{
    if (_textEdit->document() != 0)
    {
        _textEdit->document()->setModified(true);
        refreshStatusBar();
    }
}

//------------------------------------------------------------------------------
//  MainWindow::setViewMdi()
//------------------------------------------------------------------------------
void MainWindow::setViewMdi()
{
    // Show splitter
    _splitter->widget(1)->setVisible(true);
    _splitter->addWidget(_scrollArea);

    // Enable/Disable buttons
    this->sdi->setChecked(false);
    this->mdi->setChecked(true);

    // Enable 'Open image' in 'File' menu
    this->open_img->setEnabled(true);


    refreshImage();
    refreshUI();
}

//------------------------------------------------------------------------------
//  MainWindow::setViewSdi()
//------------------------------------------------------------------------------
void MainWindow::setViewSdi()
{
    // Hide splitter
    _splitter->widget(1)->setVisible(false);

    // Enable/Disable buttons
    sdi->setChecked(true);
    mdi->setChecked(false);

    // Disable 'Open image' in 'File' menu
    open_img->setEnabled(false);

    // Refresh interface
    refreshUI();
}

//------------------------------------------------------------------------------
//  MainWindow::showContextMenu()
//------------------------------------------------------------------------------
void MainWindow::showContextMenu(const QPoint& position)
{
    QMenu* contextMenu = _textEdit->createStandardContextMenu();

    contextMenu->addSeparator();


    // Retrieve selection
    QString selection = _textEdit->textCursor().selectedText();

    if (selection.isNull())
    {
        QTextCursor cursor = _textEdit->cursorForPosition(position);
        cursor.select(QTextCursor::WordUnderCursor);
        selection = cursor.selectedText();
        _textEdit->setTextCursor(cursor);
    }

    // Add a first button
    QAction* add = new QAction(QString(tr("Ajouter '%1' au dictionnaire des noms communs")).arg(selection), contextMenu);
    contextMenu->addAction(add);
    connect(add, SIGNAL(triggered()), this, SLOT(addToNounsDictionary()));

    // Add a second button
    QAction* add2 = new QAction(QString(tr("Ajouter '%1' au dictionnaire des noms propres")).arg(selection), contextMenu);
    contextMenu->addAction(add2);
    connect(add2, SIGNAL(triggered()), this, SLOT(addToNamesDictionary()));


    if (selection.isNull() or selection.contains(" "))
    {
        add->setEnabled(false);
        add2->setEnabled(false);
    }
    else
    {
        if (_dictionary->search(selection) != -1)
        {
            add->setEnabled(false);
            add->setText(QString(tr("'%1' fait déjà partie du dictionnaire")).arg(selection));

            add2->setEnabled(false);
        }
        else if (_dictionaryNames->search(selection) != -1)
        {
            add2->setEnabled(false);
            add2->setText(QString(tr("'%1' fait déjà partie du dictionnaire")).arg(selection));
        }
    }

    contextMenu->exec(_textEdit->mapToGlobal(position));
}

//------------------------------------------------------------------------------
//  MainWindow::toLower()
//------------------------------------------------------------------------------
void MainWindow::toLower()
{
    if (_textEdit != 0)
    {
        QTextCursor cursor = _textEdit->textCursor();
        QString selection = cursor.selectedText();

        if (not selection.isEmpty())
        {
            cursor.insertText(selection.toLower());
            cursor.setPosition(cursor.selectionStart(), QTextCursor::MoveAnchor);
            cursor.setPosition(cursor.selectionEnd(), QTextCursor::KeepAnchor);
            _textEdit->setTextCursor(cursor);
        }
    }
}

//------------------------------------------------------------------------------
//  MainWindow::toUpper()
//------------------------------------------------------------------------------
void MainWindow::toUpper()
{
    if (_textEdit != 0)
    {
        QTextCursor cursor = _textEdit->textCursor();
        QString selection = cursor.selectedText();

        if (not selection.isEmpty())
        {
            cursor.insertText(selection.toUpper());
            cursor.setPosition(cursor.selectionStart(), QTextCursor::MoveAnchor);
            cursor.setPosition(cursor.selectionEnd(), QTextCursor::KeepAnchor);
            _textEdit->setTextCursor(cursor);
        }
    }
}




//------------------------------------------------------------------------------
//  MainWindow::_mergeTexts()
//------------------------------------------------------------------------------
QString MainWindow::_mergeTexts(DocumentList& documents)
{
    const int MERGE_NO_SPACE = 0;
    const int MERGE_SPACE = 1;
    const int NOT_MERGE = 2;

    int flagMerge = NOT_MERGE;

    QStringList PONCTUATION;
    PONCTUATION << "?" << "!" << "." << "…";

    QString text = "";

    for (int i = 0; i < documents.size(); i++)
    {
        QFile* file = documents.getTextFile(i);

        if (file->exists())
        {
            file->open(QIODevice::ReadOnly);
            QStringHtml newPart = QStringHtml(file->readAll());
            file->close();

            if (newPart != "")
            {
                // Delete header
                newPart = _deleteLines(newPart, 0, 4);

                if (flagMerge == MERGE_SPACE or flagMerge == MERGE_NO_SPACE)
                {
                    newPart = newPart.stripHtmlTags(LEADING);

                    // If first char is not a space, add a space
                    if (flagMerge == MERGE_SPACE and (newPart.size() > 0 or not newPart.startsWith(" ")))
                        newPart = QString(" ") + newPart;
                }

                // Bottom
                newPart = newPart.stripHtmlTags(TRAILING);

                if (newPart.size() > 0)
                {
                    if (newPart.endsWith("-"))
                    {
                        newPart = newPart.remove(newPart.size() - 2, 1);
                        flagMerge = MERGE_NO_SPACE;
                    }
                    else if (PONCTUATION.contains(newPart.right(1)))
                    {
                        flagMerge = MERGE_SPACE;
                    }
                    else
                    {
                        flagMerge = NOT_MERGE;
                    }

                    text += newPart;
                }
            }
        }
    }

    return text;
}



