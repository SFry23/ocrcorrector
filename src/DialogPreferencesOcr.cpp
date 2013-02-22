#include "Config.h"
#include "DialogPreferencesOcr.h"



const QString DialogPreferencesOcr::CUNEIFORM = "Cuneiform";
const QString DialogPreferencesOcr::TESSERACT = "Tesseract";
const QString DialogPreferencesOcr::BOTH = "Tesseract + Cuneiform";

//------------------------------------------------------------------------------
//  DialogPreferencesOcr::~DialogPreferencesOcr()
//------------------------------------------------------------------------------
DialogPreferencesOcr::DialogPreferencesOcr(QWidget* parent) : QDialog(parent)
{
    // Load settings
    QString ocrEngine = gConfig->ocrEngine;
    QString lang = gConfig->ocrLanguage;

    // Create dialog
    setWindowTitle(tr("Préférences"));
    resize(parent->size() / 3);
    setModal(true);


    // Group Box 1 -------------------------------------------------------------
    QGroupBox* groupBox = new QGroupBox(tr("OCR engine"), this);
    tesseractButton = new QRadioButton(TESSERACT, groupBox);
    cuneiformButton = new QRadioButton(CUNEIFORM, groupBox);
    bothButton = new QRadioButton(BOTH, groupBox);

    tesseractButton->setChecked(ocrEngine == TESSERACT);
    cuneiformButton->setChecked(ocrEngine == CUNEIFORM);
    bothButton->setChecked(ocrEngine == BOTH);

    QVBoxLayout* vLayout1 = new QVBoxLayout(groupBox);
    vLayout1->addWidget(tesseractButton);
    vLayout1->addWidget(cuneiformButton);
    vLayout1->addWidget(bothButton);
    // -------------------------------------------------------------------------


    // Group Box 2 -------------------------------------------------------------
    QGroupBox* groupBox2 = new QGroupBox(tr("Langue"), this);

    // Language selection
    comboBox = new QComboBox(groupBox2);
    comboBox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    QStringList languages = QStringList();
    languages << "Français";
    languages << "English";

    comboBox->insertItems(0, languages);
    comboBox->setCurrentIndex(languages.indexOf(lang));

    QVBoxLayout* vLayout2 = new QVBoxLayout(groupBox2);
    vLayout2->addWidget(comboBox);
    // -------------------------------------------------------------------------

    // Spacer
    QSpacerItem* verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum,
                                                        QSizePolicy::Expanding);

    // Buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel |
                                  QDialogButtonBox::Ok);


    // Main layout
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addWidget(groupBox);
    vLayout->addWidget(groupBox2);
    vLayout->addItem(verticalSpacer);
    vLayout->addWidget(buttonBox);


    // OK and Cancel buttons
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

//------------------------------------------------------------------------------
//  DialogPreferencesOcr::accept()
//------------------------------------------------------------------------------
void DialogPreferencesOcr::accept()
{
    // Save OCR engine
    if (tesseractButton->isChecked())
        gConfig->ocrEngine = TESSERACT;
    else if (cuneiformButton->isChecked())
        gConfig->ocrEngine = CUNEIFORM;
    else if (bothButton->isChecked())
        gConfig->ocrEngine = BOTH;

    // Save Language
    gConfig->ocrLanguage = comboBox->currentText();

    close();
}
