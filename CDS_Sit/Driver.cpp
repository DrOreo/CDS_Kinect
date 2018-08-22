
// STD Headers
#include <string>
#include <vector>
// Boost Headers
#include <boost/config.hpp>

#include <iostream>
using namespace std;

#include "CDS_Sit.h"

#define API extern "C" BOOST_SYMBOL_EXPORT
CDS_Sit theApp;

API int Release()
{
	return theApp.Release();
}

API int StartKinect()
{
	return theApp.StartKinect();
}

API int StopKinect()
{
	return theApp.StopKinect();
}

API int SetDisplayMode(int _mode)
{
	return theApp.SetDisplayMode(_mode);
}

API int SaveStandard(string _name, bool _OverWrite)
{
	return theApp.SaveStandard(_name, _OverWrite);
}

API int LoadStandard(string _name)
{
	return theApp.LoadStandard(_name);
}

API int ScanStandard(vector<string>& _Names)
{
	return theApp.ScanStandard(_Names);
}

API int RemoveStandard(string _name)
{
	return theApp.RemoveStandard(_name);
}

API int SetErrorRange(int _index, double _degree)
{
	return theApp.SetErrorRange(_index, _degree);
}

API int GetErrorRange(vector<double>& _ranges)
{
	return theApp.GetErrorRange(_ranges);
}

API int StartEvaluationSit()
{
	return theApp.StartEvaluationSit();
}

API int StopEvaluationSit()
{
	return theApp.StopEvaluationSit();
}

API bool GetErrorState()
{
	return theApp.GetErrorState();
}

//=============================================================================//

API int GetImage(cv::Mat& in, int H, int W)
{
	return theApp.GetImage(in,H,W);
}

API int CVWindowClose()
{
	return theApp.CVWindowClose();
}

API int SetContrast(double _Contrast)
{
	return theApp.SetContrast(_Contrast);
}

API int SetBrightness(double _Brightness)
{
	return theApp.SetBrightness(_Brightness);
}

