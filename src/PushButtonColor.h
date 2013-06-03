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
        void colorize(const QColor color);

        /** Get the current color of the button */
        QColor getColor();

        /** Return button ID */
        int getId();

        /** Change button color */
        void setColor(const QColor color);

        /** Change button ID */
        void setId(const int id);

    signals:
        void clicked(const QColor&);

    public slots:
        void reemitClicked();
};



#endif
