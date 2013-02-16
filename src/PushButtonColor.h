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
        /** Colorize the button */
        void colorize();

        /** Get the current color of the button */
        QColor getColor();

        /** Return button ID */
        int getId();

        /** Chnage button color */
        void setColor(QColor color);

        /** Change button ID */
        void setId(int id);

    signals:
        void clicked(const QColor&);

    public slots:
        void reemitClicked();
};



#endif
