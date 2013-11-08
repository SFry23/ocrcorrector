#ifndef DialogPreferencesCorrection_h
#define DialogPreferencesCorrection_h


#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QTabWidget>
#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QColorDialog>


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

        /** Set a new dictionary for common nouns */
        void chooseDicNouns();

        /** Set a new dictionary for proper names */
        void chooseDicNames();

    signals:
        void dicChanged();
};



#endif
