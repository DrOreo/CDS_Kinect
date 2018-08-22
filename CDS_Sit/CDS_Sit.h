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

	//����
	int Release();

	//������|
	path WorkPath;

	//��Ʈw���|
	path LibPath;

	//INI���|
	path INIPath;

	//XML
	XMLConfig XMLer;

	//INI
	INIConfig INIer;
private:
	//�ǤJ�������`�I��
	int JointNumber;

	void InitilizeINI();
	//=================Kinect����==================//
public:
	//�Ұ�Kinect
	int StartKinect();

	//����Kinect
	int StopKinect();

private:
	//Kinect
	IKinectSensor* pSensor;

	//�m��v������
	IColorFrameReader* pColorFrameReader;
	IColorFrameSource* pColorFrameSource;
	IColorFrame* pColorFrame;
	UINT uBufferSize;

	//�`�׼v������
	IDepthFrameSource* pDepthFrameSource;
	IDepthFrameReader* pDepthFrameReader;
	IDepthFrame* pDepthFrame;
	int m_iDepthWidth;
	int m_iDepthHeight;
	UINT16 m_uDepthMin;
	UINT16 m_uDepthMax;

	//���[����
	IBodyFrameReader* pBodyFrameReader;
	IBody** aBodyData;
	INT32 iBodyCount = 0;
	IBodyFrameSource* pBodyFrameSource;
	ICoordinateMapper* pCoordinateMapper;
	IBodyFrame* pBodyFrame;
	Joint aJoints[JointType::JointType_Count];
	JointOrientation aOrientations[JointType::JointType_Count];

	//�^�����[�����
	boost::thread m_KinectThread;

	//�P�_�l�ܰ��[�O�_�}�Ҥ�Boolean
	bool m_bTracking;

	//�m��v���^���]�w
	int PrepareColorFrame();

	//�`�׼v���^���]�w
	int PrepareDepthFrame();

	//���[�^���]�w
	int PrepareBodyFrame();

	//�����^�����[�j��
	void Run();

	//�yø���[
	void DrawLine(cv::Mat&, const Joint&, const Joint&, ICoordinateMapper*, bool);

	//==================��������===================//
	//---------�^���з�---------//
public:
	//�^���з�
	int SaveStandard(string _name, bool _OverWrite);

	//���J�з�
	int LoadStandard(string _name);

	//���y�з���
	int ScanStandard(vector<string>&);

	//�R���з���
	int RemoveStandard(string _name);

	//�]�w�~�t�e�\�d��
	int SetErrorRange(int _index, double _degree);

	//���o�~�t�e�\�d��
	int GetErrorRange(vector<double>& _ranges);
		
private:
	//�I�s�^���Ϥ����H����
	boost::signals2::signal<void()> mSignal_SaveStandard;

	//�зǧ������O
	BodyStandard Standard;

	//�~�t�e�\�d��
	vector<double> m_vdErrorRange;

	//�Ȧs�Ϥ�
	cv::Mat saveImg;

	//�P�_�O�_�����^���зǤ�Boolean
	bool m_bFinishCaptureStandard;

	//�P�_�зǬO�_�w���J��Boolean
	bool m_bStandardIsReady;

	//��s�з�
	void CaptureStandard();

	//---------��������---------//
public:
	//�}�l�����Ҧ�
	int StartEvaluationSit();

	//���������Ҧ�
	int StopEvaluationSit();

	//���o�������A
	//ture���T(����)
	//false ���~(��)
	bool GetErrorState();

private:
	//�P�_�O�_�}�l�����Ҧ���Boolean
	bool m_bEvaluationSit;

	//�������A
	//ture���T(����)
	//false ���~(��)
	bool m_bPositionCorrect;

	//�����Ҧ��p��_�D�j��
	void EvaluationSit();

	//�����Ҧ��p��_�~�t
	double CalculateDegree(int ref, int target);

	//�p��۹�y���ഫ�ᤧ�I
	//Pos�ؼ��I�BPos0�Ѧ��I�BOri�Ѧ��I�|����
	Eigen::Vector3d TransR(JointOrientation Ori, Joint Pos0, Joint Pos);
	Eigen::Vector3d TransR(Joint Pos0, Joint Pos);

	//=================OpenCV����==================//
public:
	//�~��CV��������
	int CVWindowClose();

	//�~��CV��������
	int GetImage(cv::Mat&, int H, int W);

	//�]�w�v����ܼҦ�
	int SetDisplayMode(int);

	//�վ���
	//_Contrast 0~2
	int SetContrast(double _Contrast);

	//�վ�G��
	//_Brightness -255~255
	int SetBrightness(double _Brightness);

private:
	//�v����ܼҦ�
	DisplayMode Dmode;

	//�P�_�~��CV�����O�_�}�Ҥ�Boolean
	bool m_bCVWindowOpen;

	//�P�_Kinect�O�_���b������Boolean
	bool m_bGetImageIng;

	//���
	double m_dContrast;

	//�G��
	double m_dBrightness;

	//CV����
	cv::Mat	mColorImg;
	cv::Mat mImg;

	cv::Mat mDepthImg;
	cv::Mat mImg8bit;

	//===============BoostLOG����===================//
private:
	Bsrc::severity_logger< SeverityLevel > lg;
};