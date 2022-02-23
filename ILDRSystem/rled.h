#ifndef RLED_H
#define RLED_H

#include <QWidget>

class QColor;

class RLed : public QWidget
{
    Q_OBJECT
public:
    RLed(QWidget *parent = 0);
    ~RLed();

    QColor color() const;
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

public slots:
    void setColor(const QColor &color);
    void toggle();
    void turnOn(bool on=false);
    void turnOff(bool off=false);
	void setText(QString str);

protected:
    void paintEvent(QPaintEvent *);
    int ledWidth() const;

private:
    struct Private;
    Private * const m_d;
};

#endif // RLED_H
