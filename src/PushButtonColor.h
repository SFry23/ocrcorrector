#ifndef PushButtonColor_h
#define PushButtonColor_h



#include <QtCore>
#include <QtGui>



class PushButtonColor : public QPushButton
{
    Q_OBJECT;

    // Constructors
    public:
        PushButtonColor(QWidget* parent);
        PushButtonColor(const QColor& color, QWidget* parent);

    private:
        /** Button ID */
        int _id;

        /** Button color */
        QColor _color;

    public:
        void colorize();
        QColor getColor();
        int getId();
        void setColor(QColor color);
        void setId(int id);

    signals:
        void clicked(const QColor&);

    public slots:
        void reemitClicked();
};



#endif
