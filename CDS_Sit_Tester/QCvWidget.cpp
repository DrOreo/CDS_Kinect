#include "QCvWidget.h"  
#include <QtGui\QPainter> 
#include <QtCore\QPoint> 

QCvWidget::QCvWidget(QWidget *parent) : QWidget()
{	
	this->m_bStart = false;
	this->m_iW = 1280;
	this->m_iH = 900;

	this->timer = new QTimer(this);
	this->timer->setInterval(1);
	connect(this->timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
}

QCvWidget::~QCvWidget()
{
	cout << "~QCvWidget()" << endl;
	if (this->m_bStart)
		this->Stop();

	delete timer;
	mat_image.release();
}

void QCvWidget::Start()
{
	timer->start();
	this->m_bStart = true;
}

void QCvWidget::Stop()
{
	timer->stop();
	this->m_bStart = false;

	if (this->CDS_Sit_DLL->has("CVWindowClose"))
		this->CDS_Sit_DLL->get<int()>("CVWindowClose")();
}

void QCvWidget::resize(int w, int h)
{
	this->m_iW = w;
	this->m_iH = h;

	if (this->m_iW % 8 != 0)
	{
		this->m_iW = (w / 8 + 1) * 8;
	}

	QWidget::resize(w, h);

}

void QCvWidget::closeEvent(QCloseEvent * e)
{
	if (this->m_bStart)
	{
		this->Stop();
	}
}

void QCvWidget::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);

	const uchar *pSrc = (const uchar*)this->mat_image.data;
	QImage image(pSrc, this->mat_image.cols, this->mat_image.rows, this->mat_image.step, QImage::Format_RGB32);
	
	painter.drawImage(QPoint(0, 0), image);
}

void QCvWidget::nextFrame()
{
	if (this->CDS_Sit_DLL->has("GetImage"))
	{
		if (this->CDS_Sit_DLL->get<int(cv::Mat&,int,int)>("GetImage")(this->mat_image, this->m_iH, this->m_iW) == 0)
		{
			const uchar *pSrc = (const uchar*)this->mat_image.data;
			this->update();
		}
	}
}

void QCvWidget::SetDLL(boost::dll::shared_library* in) 
{ 
	this->CDS_Sit_DLL = in; 
}

void QCvWidget::RealseDLL() 
{ 
	if (this->m_bStart)
	{
		this->Stop();
	}
	this->CDS_Sit_DLL = NULL; 
}



