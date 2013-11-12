#include <QTextCodec>
#include <QApplication>

#include "OCRcorrector.h"
#include "Resources.h"



// Program settings
QSharedPointer<Config> gConfig;


//------------------------------------------------------------------------------
//  Main
//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    // Create application
    QApplication app(argc, argv);

    // Use UTF-8
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    // Personnal translator
    QString locale = QLocale::system().name().section("_", 0, 0);
    QTranslator translator;
    translator.load(":/lang_" + locale);
    app.installTranslator(&translator);

    // QT translator
    QTranslator stdTranslator;
    stdTranslator.load("qt_" + QLocale::system().name(),
                       QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&stdTranslator);

    // Create configuration manager
    gConfig = QSharedPointer<Config>(new Config());
    gConfig->createUserDictionaries(argv[0]);

    // Create Main Window
    MainWindow window;
    window.show();

    return app.exec();
}
