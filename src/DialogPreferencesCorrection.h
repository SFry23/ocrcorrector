#ifndef DialogPreferencesCorrection_h
#define DialogPreferencesCorrection_h

#include <QtCore>
#include <QtGui>

#include "Config.h"
#include "PushButtonColor.h"


// Software Configuration
extern QSharedPointer<Config> gConfig;



class DialogPreferencesCorrection : public QDialog
{
    Q_OBJECT;


    public:
        /** Constructor */
        DialogPreferencesCorrection(QWidget* parent);

        /** Destructor */
        ~DialogPreferencesCorrection();

    // Attributes
    private:
        /** Highlight style selection */
        QComboBox* _comboBoxStyle;

        /** Line edit where path to dictionnaries have to be writed */
        QLineEdit *_lineEdit, *_lineEdit2;

        /** Colors of corrections */
        QList<QColor> _colors;


    public slots:
        /** Save all settings and close */
        void accept();

        /** Choose a color thanks to a dialog window */
        void chooseColor(const QColor&);

        /** Choose a new dictionary for common nouns */
        void chooseDicNouns();

        /** Choose a new dictionary for names */
        void chooseDicNames();

    signals:
        void dicChanged();
};



#endif
