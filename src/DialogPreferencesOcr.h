#ifndef DialogPreferencesOcr_h
#define DialogPreferencesOcr_h

#include <QDialog>
#include <QDialogButtonBox>
#include <QRadioButton>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "Config.h"

extern QSharedPointer<Config> gConfig;

class DialogPreferencesOcr : public QDialog
{
    Q_OBJECT;

    public:
        /** Constructor */
        DialogPreferencesOcr(QWidget* parent=NULL);

    // Attributes
    private:
        /** OCR engine selection */
        QRadioButton *tesseractButton, *cuneiformButton, *bothButton;

        /** Language selection */
        QComboBox *comboBox;

        /** OCR Engines names */
        static const QString TESSERACT;
        static const QString CUNEIFORM;
        static const QString BOTH;

    // Slots
    public slots:
        /** Onclick 'Ok' */
        void accept();
};



#endif
