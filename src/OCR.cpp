#include "OCR.h"


// Unix commands to run OCR engines
const QString OCR::CUNEIFORM_CMD = QString("cuneiform");
const QString OCR::TESSERACT_CMD = QString("tesseract");


//------------------------------------------------------------------------------
//  OCR::OCR()
//------------------------------------------------------------------------------
OCR::OCR()
{
    _engine = CUNEIFORM;
    _language = FRENCH;
    _format = "html";
}

//------------------------------------------------------------------------------
//  OCR::~OCR()
//------------------------------------------------------------------------------
OCR::~OCR()
{
}

//------------------------------------------------------------------------------
//  OCR::getEngine()
//------------------------------------------------------------------------------
OCR::Engine OCR::getEngine()
{
    return _engine;
}

//------------------------------------------------------------------------------
//  OCR::getFormat()
//------------------------------------------------------------------------------
QString OCR::getFormat()
{
    return _format;
}

//------------------------------------------------------------------------------
//  OCR::getLanguage()
//------------------------------------------------------------------------------
OCR::Language OCR::getLanguage()
{
    return _language;
}

//------------------------------------------------------------------------------
//  OCR::postTreat()
//------------------------------------------------------------------------------
bool OCR::postTreat(QString inFilename, QString font, int fontSize)
{
    QFile textFile(inFilename);

    if (textFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        // Read OCR result
        QTextStream stream(&textFile);
        QString text = textFile.readAll();

        // Insert style
        QString newBody = QString("<body style=\"font-family:'%1'; font-size:%2pt;\">").arg(font, QString::number(fontSize));

        QString newText = text.replace("<body>", newBody);

        // Back to the beginning
        textFile.seek(0);

        // Replace text
        stream << newText;

        // Close file
        textFile.close();

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
//  OCR::run()
//------------------------------------------------------------------------------
bool OCR::run(const QString inFilename, const QString outFilename)
{
    if (not inFilename.isEmpty() and not outFilename.isEmpty())
    {
        QProcess process;
        QString cmd = _generateCommand(inFilename, outFilename);

        if (not cmd.isEmpty())
        {
            process.start(cmd);

            if (not process.waitForStarted())
                return false;

            if (not process.waitForFinished(OCR::TIMEOUT))
                return false;

            if (not process.readAllStandardOutput().isEmpty())
                return true;
        }
        else
        {
            qDebug() << cmd;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
//  OCR::setEngine()
//------------------------------------------------------------------------------
void OCR::setEngine(OCR::Engine engine)
{
    _engine = engine;
}

//------------------------------------------------------------------------------
//  OCR::setEngine()
//------------------------------------------------------------------------------
void OCR::setEngine(QString engineName)
{
    if (engineName == "Tesseract")
        _engine = TESSERACT;
    else if (engineName == "Cuneiform")
        _engine = CUNEIFORM;
    else if (engineName == "Tesseract + Cuneiform")
        _engine = BOTH;
}

//------------------------------------------------------------------------------
//  OCR::setFormat()
//------------------------------------------------------------------------------
void OCR::setFormat(QString format)
{
    _format = format;
}

//------------------------------------------------------------------------------
//  OCR::setLanguage()
//------------------------------------------------------------------------------
void OCR::setLanguage(OCR::Language language)
{
    _language = language;
}

//------------------------------------------------------------------------------
//  OCR::setLanguage()
//------------------------------------------------------------------------------
void OCR::setLanguage(QString languageName)
{
    if (languageName == "Français")
        _language = FRENCH;
    else if (languageName == "English")
        _language = ENGLISH;
}



//------------------------------------------------------------------------------
//  OCR::_generateCommand()
//------------------------------------------------------------------------------
QString OCR::_generateCommand(QString inFilename, QString outFilename)
{
    // Language
    QString languageCode = _generateLanguageCode(_language);

    if (not languageCode.isEmpty())
    {
        if (_engine == CUNEIFORM)
        {
            return QString("%1 \"%2\" -o \"%3\" -l %4 -f %5").arg(OCR::CUNEIFORM_CMD, inFilename, outFilename, languageCode, _format);
        }
        else
        {
            // Remove extension (necessary for Tesseract)
            QFileInfo fileInfo(outFilename);
            QString outName = fileInfo.path() + "/" + fileInfo.baseName();

            if (_engine == TESSERACT)
            {
                return QString("%1 \"%2\" \"%3\" -l %4 hocr").arg(OCR::TESSERACT_CMD, inFilename, outName, languageCode);
            }
            else if (_engine == BOTH)
            {
                QString cmdTesseract = QString("%1 \"%2\" \"%3\" -l %4 hocr").arg(OCR::TESSERACT_CMD, inFilename, outName + "-tess", languageCode);
                QString cmdCuneiform = QString("%1 \"%2\" -o \"%3\" -l %4 -f %5").arg(OCR::CUNEIFORM_CMD, inFilename, outName + "-cuneiform.html", languageCode, _format);
            }
        }
    }

    return QString();
}

//------------------------------------------------------------------------------
//  OCR::_generateLanguageCode()
//------------------------------------------------------------------------------
QString OCR::_generateLanguageCode(OCR::Language language)
{
    QMap<OCR::Language, QString> languageCodes;

    languageCodes[ENGLISH] = "en";
    languageCodes[FRENCH] = "fra";

    return languageCodes.value(language);
}

