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
	//初始化DLL
	int Intiliaze();

	//釋放DLL
	int Release();

	//啟動Kinect
	//return -1：Kinect已開啟
	//return -2：Kinect相關錯誤
	//return -3：無法預期的錯誤
	int StartKinect();

	//關閉Kinect
	//return -1：Kinect已關閉
	//return -2：無法預期的錯誤
	int StopKinect();

	//設定顯示模式
	//1 = Color
	//2 = Gray
	//3 = Depth
	//return -1：輸入參數錯誤
	int SetDisplayMode(int);

	//擷取標準
	//_name: 標準名稱
	//_OverWrite: 是否開啟覆蓋檔案功能
	//return -1：檔案名稱重複
	//return -2：Kinect未啟動
	int SaveStandard(string _name, bool _OverWrite);

	//載入標準
	//return -1：無法開啟xml檔
	int LoadStandard(string _name);

	//掃描標準檔
	//return -1：檔案資料夾不存在
	int ScanStandard(vector<string>&);

	//刪除標準
	int RemoveStandard(string _name);

	//設定誤差容許範圍
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
	//取得誤差容許範圍
	int GetErrorRange(vector<double>& _ranges);

	//開始坐姿評分
	//return -1：Kinect未啟動
	//return -2：未載入標準
	//return -3：評分已啟動
	int StartEvaluationSit();

	//停止坐姿評分
	//return -1：Kinect未啟動
	//return -2：未載入標準
	//return -3：評分已停止
	int StopEvaluationSit();

	//取得評分狀態
	//ture正確(全綠)
	//false 錯誤(紅)
	bool GetErrorState();

	//test
	void OutputFunc();

	//=================OpenCV相關==================//

	//調整對比
	//_Contrast 0~2
	int SetContrast(double _Contrast);

	//調整亮度
	//_Brightness -255~255
	int SetBrightness(double _Brightness);

private:
	boost::dll::shared_library CDS_Sit_DLL;
	bool m_bIntiliaze;

public:
	//設定QT-CV視窗
	int SetQCvWidget(QCvWidget*);

private:
	QCvWidget* pQCvWidget;

};
