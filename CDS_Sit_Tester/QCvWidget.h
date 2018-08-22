#ifndef QCvWidget_H
#define QCvWidget_H  

//QT Header
#include <QtWidgets\QWidget> 
#include <QtGui\QPaintEvent> 
#include <QtGui\QImage> 
#include <QtCore\QTimer> 
#include <QThread>

// OpenCV Header
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#pragma comment(lib,"opencv_core2413.lib")
#pragma comment(lib,"opencv_video2413.lib")
#pragma comment(lib,"opencv_highgui2413.lib")
#pragma comment(lib,"opencv_imgproc2413.lib")

// Other Header
#include <boost/dll/shared_library.hpp>
#include <iostream>
using namespace std;


class QCvWidget : public QWidget
{
	Q_OBJECT
public:
	QCvWidget(QWidget *parent = 0);
	~QCvWidget();

	void Start();
	void Stop();
	void resize(int, int);
	void SetDLL(boost::dll::shared_library* in);
	void RealseDLL();

protected:
	void closeEvent(QCloseEvent *e);
	void paintEvent(QPaintEvent *e);
	private slots:
	void nextFrame();
private:
	//Boost DLL 
	boost::dll::shared_library* CDS_Sit_DLL;

	//
	QTimer *timer;
	cv::Mat mat_image;
	cv::Mat* mat_image2;
	bool m_bStart;
	int m_iW;
	int m_iH;
};


#endif  