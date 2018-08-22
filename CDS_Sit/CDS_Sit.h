/*=========================================================================

Program:
Module:    CDS_Sit.h

OREO 2016/11/29
=========================================================================*/

#pragma once
// OpenCV Header
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#pragma comment(lib,"opencv_core2413.lib")
#pragma comment(lib,"opencv_video2413.lib")
#pragma comment(lib,"opencv_highgui2413.lib")
#pragma comment(lib,"opencv_imgproc2413.lib")

// Kinect for Windows SDK Header
#include <Kinect.h>
#pragma comment(lib,"Kinect20.lib")

// Boost Header
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include "boost.log.init.hpp"

// Other Header
#include <iostream>
#include <Eigen/Dense>
#include "BodyStandard.h"
#include "XMLConfig.hpp"
#include "INIConfig.hpp"

using namespace std;
using namespace boost::filesystem;

enum DisplayMode { DisplayMode_Color, DisplayMode_Gray, DisplayMode_Depth};
class CDS_Sit
{
public:
	CDS_Sit();
	~CDS_Sit();

	//釋放
	int Release();

	//執行路徑
	path WorkPath;

	//資料庫路徑
	path LibPath;

	//INI路徑
	path INIPath;

	//XML
	XMLConfig XMLer;

	//INI
	INIConfig INIer;
private:
	//納入評估的節點數
	int JointNumber;

	void InitilizeINI();
	//=================Kinect相關==================//
public:
	//啟動Kinect
	int StartKinect();

	//關閉Kinect
	int StopKinect();

private:
	//Kinect
	IKinectSensor* pSensor;

	//彩色影像相關
	IColorFrameReader* pColorFrameReader;
	IColorFrameSource* pColorFrameSource;
	IColorFrame* pColorFrame;
	UINT uBufferSize;

	//深度影像相關
	IDepthFrameSource* pDepthFrameSource;
	IDepthFrameReader* pDepthFrameReader;
	IDepthFrame* pDepthFrame;
	int m_iDepthWidth;
	int m_iDepthHeight;
	UINT16 m_uDepthMin;
	UINT16 m_uDepthMax;

	//骨架相關
	IBodyFrameReader* pBodyFrameReader;
	IBody** aBodyData;
	INT32 iBodyCount = 0;
	IBodyFrameSource* pBodyFrameSource;
	ICoordinateMapper* pCoordinateMapper;
	IBodyFrame* pBodyFrame;
	Joint aJoints[JointType::JointType_Count];
	JointOrientation aOrientations[JointType::JointType_Count];

	//擷取骨架執行序
	boost::thread m_KinectThread;

	//判斷追蹤骨架是否開啟之Boolean
	bool m_bTracking;

	//彩色影像擷取設定
	int PrepareColorFrame();

	//深度影像擷取設定
	int PrepareDepthFrame();

	//骨架擷取設定
	int PrepareBodyFrame();

	//執行擷取骨架迴圈
	void Run();

	//描繪骨架
	void DrawLine(cv::Mat&, const Joint&, const Joint&, ICoordinateMapper*, bool);

	//==================評分相關===================//
	//---------擷取標準---------//
public:
	//擷取標準
	int SaveStandard(string _name, bool _OverWrite);

	//載入標準
	int LoadStandard(string _name);

	//掃描標準檔
	int ScanStandard(vector<string>&);

	//刪除標準檔
	int RemoveStandard(string _name);

	//設定誤差容許範圍
	int SetErrorRange(int _index, double _degree);

	//取得誤差容許範圍
	int GetErrorRange(vector<double>& _ranges);
		
private:
	//呼叫擷取圖片之信號槽
	boost::signals2::signal<void()> mSignal_SaveStandard;

	//標準坐姿類別
	BodyStandard Standard;

	//誤差容許範圍
	vector<double> m_vdErrorRange;

	//暫存圖片
	cv::Mat saveImg;

	//判斷是否完成擷取標準之Boolean
	bool m_bFinishCaptureStandard;

	//判斷標準是否已載入之Boolean
	bool m_bStandardIsReady;

	//刷新標準
	void CaptureStandard();

	//---------坐姿評分---------//
public:
	//開始評分模式
	int StartEvaluationSit();

	//結束評分模式
	int StopEvaluationSit();

	//取得評分狀態
	//ture正確(全綠)
	//false 錯誤(紅)
	bool GetErrorState();

private:
	//判斷是否開始評分模式之Boolean
	bool m_bEvaluationSit;

	//評分狀態
	//ture正確(全綠)
	//false 錯誤(紅)
	bool m_bPositionCorrect;

	//評分模式計算_主迴圈
	void EvaluationSit();

	//評分模式計算_誤差
	double CalculateDegree(int ref, int target);

	//計算相對座標轉換後之點
	//Pos目標點、Pos0參考點、Ori參考點四元數
	Eigen::Vector3d TransR(JointOrientation Ori, Joint Pos0, Joint Pos);
	Eigen::Vector3d TransR(Joint Pos0, Joint Pos);

	//=================OpenCV相關==================//
public:
	//外部CV視窗關閉
	int CVWindowClose();

	//外部CV視窗取像
	int GetImage(cv::Mat&, int H, int W);

	//設定影像顯示模式
	int SetDisplayMode(int);

	//調整對比
	//_Contrast 0~2
	int SetContrast(double _Contrast);

	//調整亮度
	//_Brightness -255~255
	int SetBrightness(double _Brightness);

private:
	//影像顯示模式
	DisplayMode Dmode;

	//判斷外部CV視窗是否開啟之Boolean
	bool m_bCVWindowOpen;

	//判斷Kinect是否正在取像中Boolean
	bool m_bGetImageIng;

	//對比
	double m_dContrast;

	//亮度
	double m_dBrightness;

	//CV相關
	cv::Mat	mColorImg;
	cv::Mat mImg;

	cv::Mat mDepthImg;
	cv::Mat mImg8bit;

	//===============BoostLOG相關===================//
private:
	Bsrc::severity_logger< SeverityLevel > lg;
};