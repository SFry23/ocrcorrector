#ifndef TEXTEDIT_H
#define TEXTEDIT_H


#include <QtCore>
#include <QtGui>


class TextEdit : public QTextEdit
{
    public:
        TextEdit(QWidget* parent = NULL);

        // Change format
        void removeBold();
        void removeHighlight();
        void removeItalic();
        void removeUnderline();

        void replaceMeta();
        void removeRightSpaces();
};



#endif
