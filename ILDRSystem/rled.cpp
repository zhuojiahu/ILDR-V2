#include <QtGui>
#include "rled.h"

struct RLed::Private
{
public:
    Private()
        : darkerFactor(300), color(Qt::red), isOn(true),m_label("")
    { }

    int darkerFactor;
    QColor color;
    bool isOn;
	QString m_label;
};

RLed::RLed(QWidget *parent)
    :QWidget(parent), m_d(new Private)
{
}

RLed::~RLed()
{
    delete m_d;
}

QColor RLed::color() const
{
    return m_d->color;
}

void RLed::setColor(const QColor &color)
{
    if (m_d->color == color)
        return;
    update();
}

QSize RLed::sizeHint() const
{
    return QSize(20, 20);
}

QSize RLed::minimumSizeHint() const
{
    return QSize(16, 16);
}


void RLed::setText(QString str)
{
	if (m_d->m_label == str)
		return;
	m_d->m_label = str;
	update();
}

void RLed::toggle()
{
    m_d->isOn = !m_d->isOn;
    update();
}

void RLed::turnOn(bool on)
{
    m_d->isOn = on;
    update();
}

void RLed::turnOff(bool off)
{
    turnOn(!off);
}

void RLed::paintEvent(QPaintEvent * /*event*/)
{
    int width = ledWidth();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor color = m_d->isOn ? m_d->color
                             : QColor(Qt::green);

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(color);
    painter.setBrush(brush);
    // draw plain
    painter.drawEllipse(1, 1, width-1, width-1);

    QPen pen;
    pen.setWidth(2);

    int pos = width / 5 + 1;
    int lightWidth = width * 2 / 3;
    int lightQuote = 130 * 2 / (lightWidth ? lightWidth : 1) + 100;

    // draw bright spot
    while (lightWidth) {
        color = color.lighter(lightQuote);
        pen.setColor(color);
        painter.setPen(pen);
        painter.drawEllipse(pos, pos, lightWidth, lightWidth);
        lightWidth--;

        if (!lightWidth)
            break;

        painter.drawEllipse(pos, pos, lightWidth, lightWidth);
        lightWidth--;

        if (!lightWidth)
            break;

        painter.drawEllipse(pos, pos, lightWidth, lightWidth);
        pos++;
        lightWidth--;
    }

    //draw border
    painter.setBrush(Qt::NoBrush);

    int angle = -720;
    color = palette().color(QPalette::Light);

    for (int arc=120; arc<2880; arc+=240) {
        pen.setColor(color);
        painter.setPen(pen);
        int w = width - pen.width()/2;
        painter.drawArc(pen.width()/2, pen.width()/2, w, w, angle+arc, 240);
        painter.drawArc(pen.width()/2, pen.width()/2, w, w, angle-arc, 240);
        color = color.darker(110);
    }
}

int RLed::ledWidth() const
{
    int width = qMin(this->width(), this->height());
    width -= 2;
    return width > 0 ? width : 0;
}

