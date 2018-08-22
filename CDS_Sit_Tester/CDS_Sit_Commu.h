/*=========================================================================

Program:
Module:    CDS_Sit_Commu.h

OREO 2016/11/29
=========================================================================*/
#pragma once
#include <boost/dll/shared_library.hpp>
#include "QCvWidget.h"

class CDS_Sit_Commu
{
public:
	CDS_Sit_Commu();
	~CDS_Sit_Commu();

public:
	//��l��DLL
	int Intiliaze();

	//����DLL
	int Release();

	//�Ұ�Kinect
	//return -1�GKinect�w�}��
	//return -2�GKinect�������~
	//return -3�G�L�k�w�������~
	int StartKinect();

	//����Kinect
	//return -1�GKinect�w����
	//return -2�G�L�k�w�������~
	int StopKinect();

	//�]�w��ܼҦ�
	//1 = Color
	//2 = Gray
	//3 = Depth
	//return -1�G��J�Ѽƿ��~
	int SetDisplayMode(int);

	//�^���з�
	//_name: �зǦW��
	//_OverWrite: �O�_�}���л\�ɮץ\��
	//return -1�G�ɮצW�٭���
	//return -2�GKinect���Ұ�
	int SaveStandard(string _name, bool _OverWrite);

	//���J�з�
	//return -1�G�L�k�}��xml��
	int LoadStandard(string _name);

	//���y�з���
	//return -1�G�ɮ׸�Ƨ����s�b
	int ScanStandard(vector<string>&);

	//�R���з�
	int RemoveStandard(string _name);

	//�]�w�~�t�e�\�d��
	//_index 0~8, _degree 0~180
	//_index 0: Head
	//_index 1: Neck
	//_index 2: ShoulderLeft
	//_index 3: ShoulderRight
	//_index 4: SpineMid
	//_index 5: SpineBase
	//_index 6: ElbowLeft
	//_index 7: ElbowRight
	int SetErrorRange(int _index, double _degree);

	//_index 0: Head
	//_index 1: Neck
	//_index 2: ShoulderLeft
	//_index 3: ShoulderRight
	//_index 4: SpineMid
	//_index 5: SpineBase
	//_index 6: ElbowLeft
	//_index 7: ElbowRight
	//���o�~�t�e�\�d��
	int GetErrorRange(vector<double>& _ranges);

	//�}�l��������
	//return -1�GKinect���Ұ�
	//return -2�G�����J�з�
	//return -3�G�����w�Ұ�
	int StartEvaluationSit();

	//���������
	//return -1�GKinect���Ұ�
	//return -2�G�����J�з�
	//return -3�G�����w����
	int StopEvaluationSit();

	//���o�������A
	//ture���T(����)
	//false ���~(��)
	bool GetErrorState();

	//test
	void OutputFunc();

	//=================OpenCV����==================//

	//�վ���
	//_Contrast 0~2
	int SetContrast(double _Contrast);

	//�վ�G��
	//_Brightness -255~255
	int SetBrightness(double _Brightness);

private:
	boost::dll::shared_library CDS_Sit_DLL;
	bool m_bIntiliaze;

public:
	//�]�wQT-CV����
	int SetQCvWidget(QCvWidget*);

private:
	QCvWidget* pQCvWidget;

};
