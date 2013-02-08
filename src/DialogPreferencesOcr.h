#ifndef DialogPreferencesOcr_h
#define DialogPreferencesOcr_h

#include <QtCore>
#include <QtGui>
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
        QRadioButton *tesseractButton, *cuneiformButton;

        /** Language selection */
        QComboBox *comboBox;

        /** OCR Engines names */
        static const QString TESSERACT;
        static const QString CUNEIFORM;

    // Slots
    public slots:
        /** Onclick 'Ok' */
        void accept();
};



#endif
