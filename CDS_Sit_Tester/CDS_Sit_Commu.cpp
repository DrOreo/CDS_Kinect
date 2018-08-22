/*=========================================================================

Program:
Module:    CDS_Sit_Commu.cpp

OREO 2016/11/29
=========================================================================*/
#include "CDS_Sit_Commu.h"
#include <iostream>
using namespace std;

CDS_Sit_Commu::CDS_Sit_Commu()
{
	this->m_bIntiliaze = false;
	this->pQCvWidget = nullptr;
}

CDS_Sit_Commu::~CDS_Sit_Commu()
{
	this->CDS_Sit_DLL.unload();
}

int CDS_Sit_Commu::Intiliaze()
{

#ifdef _WIN32
	boost::filesystem::path WorkPath = boost::filesystem::current_path();
	boost::filesystem::path pathDLL = WorkPath/"CDS_Sit.dll";
#else
	return -1;//boost::filesystem::path pathDLL = "libDLLa.so";
#endif

	boost::system::error_code err;
	this->CDS_Sit_DLL.load(pathDLL, err);
	if (err.value() != 0)
	{
		cout << "DLL¸ü¤J¿ù»~¡A" << err.message() << " ¿ù»~¥N½X: " << err.value() << endl;
		this->m_bIntiliaze = false;
		return err.value();
	}

	this->m_bIntiliaze = true;
	return 0;
}

int CDS_Sit_Commu::Release()
{
	if (!this->CDS_Sit_DLL.has("Release"))
		return 1;

	this->pQCvWidget->RealseDLL();
	this->CDS_Sit_DLL.get<int()>("Release")();
	this->CDS_Sit_DLL.unload();

	this->m_bIntiliaze = false;
	return 0;
}

int CDS_Sit_Commu::StartKinect()
{
	if (this->CDS_Sit_DLL.has("StartKinect"))
	{
		return this->CDS_Sit_DLL.get<int()>("StartKinect")();
	}

	return 1;
}

int CDS_Sit_Commu::StopKinect()
{
	if (this->CDS_Sit_DLL.has("StopKinect"))
	{
		return this->CDS_Sit_DLL.get<int()>("StopKinect")();
	}

	return 1;
}

int CDS_Sit_Commu::SetDisplayMode(int _mode)
{
	if (this->CDS_Sit_DLL.has("SetDisplayMode"))
	{
		return this->CDS_Sit_DLL.get<int(int)>("SetDisplayMode")(_mode);
	}

	return 1;
}

int CDS_Sit_Commu::SaveStandard(string _name, bool _OverWrite)
{
	if (this->CDS_Sit_DLL.has("SaveStandard"))
	{
		return this->CDS_Sit_DLL.get< int(string,bool)>("SaveStandard")(_name, _OverWrite);
	}

	return 1;
}

int CDS_Sit_Commu::LoadStandard(string _name)
{
	if (this->CDS_Sit_DLL.has("LoadStandard"))
	{
		return this->CDS_Sit_DLL.get< int(string)>("LoadStandard")(_name);
	}

	return 1;
}

int CDS_Sit_Commu::ScanStandard(vector<string>& _Names)
{
	if (this->CDS_Sit_DLL.has("ScanStandard"))
	{
		return this->CDS_Sit_DLL.get<int(vector<string>&)>("ScanStandard")(_Names);
	}

	return 1;
}

int CDS_Sit_Commu::RemoveStandard(string _name)
{
	if (this->CDS_Sit_DLL.has("RemoveStandard"))
	{
		return this->CDS_Sit_DLL.get< int(string)>("RemoveStandard")(_name);
	}

	return 1;
}

int CDS_Sit_Commu::SetErrorRange(int _index, double _degree)
{
	if (this->CDS_Sit_DLL.has("SetErrorRange"))
	{
		return this->CDS_Sit_DLL.get<int(int,double)>("SetErrorRange")(_index, _degree);
	}

	return 0;
}

int CDS_Sit_Commu::GetErrorRange(vector<double>& _ranges)
{
	if (this->CDS_Sit_DLL.has("GetErrorRange"))
	{
		return this->CDS_Sit_DLL.get<int(vector<double>&)>("GetErrorRange")(_ranges);
	}

	return 0;
}

int CDS_Sit_Commu::StartEvaluationSit()
{
	if (this->CDS_Sit_DLL.has("StartEvaluationSit"))
	{
		return this->CDS_Sit_DLL.get<int()>("StartEvaluationSit")();
	}
}

int CDS_Sit_Commu::StopEvaluationSit()
{
	if (this->CDS_Sit_DLL.has("StopEvaluationSit"))
	{
		return this->CDS_Sit_DLL.get<int()>("StopEvaluationSit")();
	}
}

bool CDS_Sit_Commu::GetErrorState()
{
	if (this->CDS_Sit_DLL.has("GetErrorState"))
	{
		return this->CDS_Sit_DLL.get<bool()>("GetErrorState")();
	}
}

void CDS_Sit_Commu::OutputFunc()
{
	std::cout << "ok ";
}

//==========================================================================//

int CDS_Sit_Commu::SetContrast(double _Contrast)
{
	if (this->CDS_Sit_DLL.has("SetContrast"))
	{
		return this->CDS_Sit_DLL.get< int(double)>("SetContrast")(_Contrast);
	}

	return 1;
}

int CDS_Sit_Commu::SetBrightness(double _Brightness)
{
	if (this->CDS_Sit_DLL.has("SetBrightness"))
	{
		return this->CDS_Sit_DLL.get< int(double)>("SetBrightness")(_Brightness);
	}

	return 1;
}

int CDS_Sit_Commu::SetQCvWidget(QCvWidget* in)
{
	if (!this->m_bIntiliaze)
	{
		return 1;
	}

	this->pQCvWidget = in;
	this->pQCvWidget->SetDLL(&this->CDS_Sit_DLL);

	return 0;
}

