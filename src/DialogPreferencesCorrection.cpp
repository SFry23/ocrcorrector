#include "DialogPreferencesCorrection.h"



//------------------------------------------------------------------------------
//  DialogPreferencesCorrection::~DialogPreferencesCorrection()
//------------------------------------------------------------------------------
DialogPreferencesCorrection::DialogPreferencesCorrection(QWidget* parent)
{
    setWindowTitle(tr("Préférences"));
    resize(parent->size() / 3);
    setModal(true);

    // Load settings
    _colors.append(gConfig->colorError);
    _colors.append(gConfig->colorCorrected);
    _colors.append(gConfig->colorReplace);

    // Tab widget
    QTabWidget* tabWidget = new QTabWidget(this);

    QWidget* page1 = new QWidget(this);
    QWidget* page2 = new QWidget(this);
    tabWidget->addTab(page1, tr("Apparence"));
    tabWidget->addTab(page2, tr("Dictionnaires"));


    // Page 1 ------------------------------------------------------------------


    // ------ Group Box 1
    QGroupBox* groupBox11 = new QGroupBox(tr("Signalement des erreurs"), page1);

    QLabel* label11 = new QLabel(tr("Mise en évidence des erreurs : "));

    _comboBoxStyle = new QComboBox(page1);
    _comboBoxStyle->addItem(tr("Souligner"));
    _comboBoxStyle->addItem(tr("Surligner"));
    _comboBoxStyle->setCurrentIndex(gConfig->highlightStyle);

    QHBoxLayout* hLayout11 = new QHBoxLayout(groupBox11);
    hLayout11->addWidget(label11);
    hLayout11->addWidget(_comboBoxStyle);
    // ------------------


    // ------ Group Box 2
    QGroupBox* groupBox12 = new QGroupBox(tr("Couleurs"), page1);

    QGridLayout* gridLayout = new QGridLayout(groupBox12);

    QStringList labels = (QStringList() << tr("Erreur") << tr("Erreur corrigée") << tr("Remplacement automatique"));

    for (int i = 0; i < _colors.size(); i++)
    {
        QLabel* label = new QLabel(labels[i] + " : ");

        PushButtonColor* button = new PushButtonColor(_colors[i], this);
        button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        button->setId(i);
        button->colorize();
        connect(button, SIGNAL(clicked(const QColor&)), this, SLOT(chooseColor(const QColor&)));

        gridLayout->addWidget(label, i, 0);
        gridLayout->addWidget(button, i, 1);
    }
    // ------------------


    // Spacer
    QSpacerItem* vSpacer1 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    QVBoxLayout* vLayout1 = new QVBoxLayout(page1);
    vLayout1->addWidget(groupBox11);
    vLayout1->addWidget(groupBox12);
    vLayout1->addItem(vSpacer1);
    //--------------------------------------------------------------------------


    // Page 2 ------------------------------------------------------------------

    // ------ First groupbox
    QGroupBox* groupBox = new QGroupBox(tr("Dictionnaire noms communs"), this);
    _lineEdit = new QLineEdit(gConfig->pathDic, groupBox);
    QPushButton* browseButton = new QPushButton(tr("Parcourir..."), groupBox);

    QHBoxLayout* horizontalLayout = new QHBoxLayout(groupBox);
    horizontalLayout->addWidget(_lineEdit);
    horizontalLayout->addWidget(browseButton);


    // ------ Second groupbox
    QGroupBox* groupBox2 = new QGroupBox(tr("Dictionnaire noms propres"), this);
    _lineEdit2 = new QLineEdit(gConfig->pathDicNames, groupBox2);
    QPushButton* browseButton2 = new QPushButton(tr("Parcourir..."), groupBox2);

    QHBoxLayout* horizontalLayout2 = new QHBoxLayout(groupBox2);
    horizontalLayout2->addWidget(_lineEdit2);
    horizontalLayout2->addWidget(browseButton2);

    // Buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    // Spacer
    QSpacerItem* vSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    QVBoxLayout* vLayout2 = new QVBoxLayout(page2);
    vLayout2->addWidget(groupBox);
    vLayout2->addWidget(groupBox2);
    vLayout2->addItem(vSpacer);

    // Connect browse buttons
    connect(browseButton, SIGNAL(clicked()), this, SLOT(chooseDicNouns()));
    connect(browseButton2, SIGNAL(clicked()), this, SLOT(chooseDicNames()));

    page2->setLayout(vLayout2);
    //--------------------------------------------------------------------------

    // Main Layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);

    // Cancel and OK buttons
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

//------------------------------------------------------------------------------
//  DialogPreferencesCorrection::~DialogPreferencesCorrection()
//------------------------------------------------------------------------------
DialogPreferencesCorrection::~DialogPreferencesCorrection()
{
}

//------------------------------------------------------------------------------
//  DialogPreferencesCorrection::chooseDicNouns()
//------------------------------------------------------------------------------
void DialogPreferencesCorrection::chooseDicNouns()
{
    QString info = tr("Sélectionner dictionnaire de noms communs");
    QString filename = QFileDialog::getOpenFileName(this,
                                                    info,
                                                    gConfig->pathDic,
                                                    "*.txt");
    if (not filename.isEmpty())
        _lineEdit->setText(filename);
}

//------------------------------------------------------------------------------
//  DialogPreferencesCorrection::chooseDicNames()
//------------------------------------------------------------------------------
void DialogPreferencesCorrection::chooseDicNames()
{
    QString info = tr("Sélectionner dictionnaire de noms propres");
    QString filename = QFileDialog::getOpenFileName(this,
                                                    info,
                                                    gConfig->pathDicNames,
                                                    "*.txt");
    if (not filename.isEmpty())
        _lineEdit2->setText(filename);
}


//------------------------------------------------------------------------------
//  DialogPreferencesCorrection::chooseColor()
//------------------------------------------------------------------------------
void DialogPreferencesCorrection::chooseColor(const QColor& oldColor)
{
    PushButtonColor* emitter = qobject_cast<PushButtonColor*>(sender());

    QColor color = QColorDialog::getColor(oldColor, this);

    if (color.isValid())
    {
        _colors[emitter->getId()] = color;
        emitter->setColor(color);
        emitter->colorize();
    }
}

//------------------------------------------------------------------------------
//  DialogPreferencesCorrection::accept()
//------------------------------------------------------------------------------
void DialogPreferencesCorrection::accept()
{
    // Save dictionaries
    if (gConfig->pathDic != _lineEdit->text())
    {
        gConfig->pathDic = _lineEdit->text();
        emit dicChanged();
    }

    if (gConfig->pathDicNames != _lineEdit2->text())
    {
        gConfig->pathDicNames = _lineEdit2->text();
        emit dicChanged();
    }

    // Save highlight style
    gConfig->highlightStyle = _comboBoxStyle->currentIndex();

    // Save colors
    gConfig->colorError = _colors[0];
    gConfig->colorCorrected = _colors[1];
    gConfig->colorReplace = _colors[2];

    close();
}


