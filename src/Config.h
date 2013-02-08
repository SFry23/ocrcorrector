#ifndef CONFIG_H
#define CONFIG_H



#include <QtCore>
#include <QtGui>

#include "Resources.h"

class Config
{
    // Attributes
    public:
        QString font;
        int fontSize;
        QString ocrEngine;
        QString ocrLanguage;
        QString pathDic;
        QString pathDicNames;

        QString pathDefaultDic;
        QString pathDefaultDicNames;

        QColor colorError;
        QColor colorCorrected;
        QColor colorReplace;

        int highlightStyle;

    // Methods
    public:

        /** Read previous configuration */
        void read()
        {
            QSettings settings(APP_NAME, APP_NAME);

            font = settings.value("UI/Font", "Ubuntu").toString();
            fontSize = settings.value("UI/FontSize", 12).toInt();
            ocrEngine = settings.value("OCR/Engine", "Tesseract").toString();
            ocrLanguage = settings.value("OCR/Language", "Français").toString();
            pathDic = settings.value("Correction/NounsDictionary", pathDefaultDic).toString();
            pathDicNames = settings.value("Correction/NamesDictionary", pathDefaultDicNames).toString();
            highlightStyle = settings.value("Correction/HighlightStyle", 0).toInt();

            // Color of errors
            colorError = settings.value("Correction/ColorError").value<QColor>();
            if (not colorError.isValid())
                colorError = QColor(255, 0, 0);

            // Color of corrected words
            colorCorrected = settings.value("Correction/ColorCorrected").value<QColor>();
            if (not colorCorrected.isValid())
                colorCorrected = QColor(0, 255, 0);

            // Color of automatic replace
            colorReplace = settings.value("Correction/ColorReplace").value<QColor>();
            if (not colorReplace.isValid())
                colorReplace = QColor(85, 170, 255);
        }

        /** Save current configuration */
        void write()
        {
            QSettings settings(APP_NAME, APP_NAME);

            settings.setValue("UI/Font", font);
            settings.setValue("UI/FontSize", fontSize);
            settings.setValue("OCR/Engine", ocrEngine);
            settings.setValue("OCR/Language", ocrLanguage);
            settings.setValue("Correction/NounsDictionary", pathDic);
            settings.setValue("Correction/NamesDictionary", pathDicNames);
            settings.setValue("Correction/ColorError", colorError);
            settings.setValue("Correction/ColorCorrected", colorCorrected);
            settings.setValue("Correction/ColorReplace", colorReplace);
            settings.setValue("Correction/HighlightStyle", highlightStyle);
        }

        /** Create dictionaries at first install */
        void createUserDictionaries(const char* execPath)
        {
            QFile targetDicNouns(QDir::homePath() + QDir::separator() +
                                 QString(".local") + QDir::separator() +
                                 QString("share") + QDir::separator() +
                                 APP_NAME + QDir::separator() +
                                 QString("dictionaries") + QDir::separator() +
                                 QString("default.txt"));

            QFile targetDicNames(QDir::homePath() + QDir::separator() +
                                 QString(".local") + QDir::separator() +
                                 QString("share") + QDir::separator() +
                                 APP_NAME + QDir::separator() +
                                 QString("dictionaries") + QDir::separator() +
                                 QString("names.txt"));


            // First case (compiled version)
            QFileInfo exec(QFile::decodeName(execPath));
            QFileInfo dicInfo(exec.absolutePath() + QDir::separator() +
                              QString("dictionaries") + QDir::separator() +
                              QString("default.txt"));

            if (dicInfo.exists())
            {
                this->pathDefaultDic = dicInfo.absoluteFilePath();
                this->pathDefaultDicNames = exec.absolutePath() + QDir::separator() +
                                               QString("dictionaries") + QDir::separator() +
                                               QString("names.txt");
            }
            else
            {
                // Second case (package installation)
                QDir targetDir(QDir::homePath() + QDir::separator() +
                               QString(".local") + QDir::separator() +
                               QString("share") + QDir::separator() +
                               APP_NAME);

                if (! targetDir.exists())
                {
                    QDir share(QDir::homePath() + QDir::separator() +
                               QString(".local") + QDir::separator() +
                               QString("share"));

                    share.mkdir(APP_NAME);
                }

                QFile dicNouns(QDir::rootPath() + QDir::separator() +
                               "usr" + QDir::separator() +
                               "share" + QDir::separator() +
                               APP_NAME + QDir::separator() +
                               "dictionaries" + QDir::separator() +
                               "default.txt");

                QFile dicNames(QDir::rootPath() + QDir::separator() +
                               "usr" + QDir::separator() +
                               "share" + QDir::separator() +
                               APP_NAME + QDir::separator() +
                               "dictionaries" + QDir::separator() +
                               "names.txt");

                if (dicNouns.exists())
                {
                    if (! targetDicNouns.exists())
                        this->pathDefaultDic = dicNouns.fileName();
                }

                if (dicNames.exists())
                {
                    if (! targetDicNames.exists())
                        this->pathDefaultDicNames = dicNames.fileName();
                }
            }
        }

    private:
        QSettings* _settings;
};



#endif
