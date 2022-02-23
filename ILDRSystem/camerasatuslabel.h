#ifndef CAMERASATUSLABEL_H
#define CAMERASATUSLABEL_H

#include <QLabel>
#include <QTimer>
//
enum e_ItenColor
{
	Green = 0,
	Red,
	Blue,
	Yellow,
	Black,
	Gray
};
 
class CameraStatusLabel : public QLabel
{
	Q_OBJECT

public:
	CameraStatusLabel(QWidget *parent);
	~CameraStatusLabel();

// 	void paintEvent(QPaintEvent *event);
	void BlinkCameraStatus(int iMode);//mode:0 ²ÉÍ¼£¬1´íÍ¼£¬£¨À¶ºì£©
	void SetCameraStatus(int iMode);//mode:0:Õý³££¬1£º³õÊ¼»¯Ê§°Ü,2:Í£Ö¹¡£(»ÒºÚ)
private:
	QTimer *blinkTimer;
	e_ItenColor eColor;
	bool m_showState;
private slots:
	void slots_reStore();
};

#endif // CAMERASATUSLABEL_H
