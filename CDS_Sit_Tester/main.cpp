// STD Headers
#include <iostream>
using namespace std;
// Boost Headers
#include "CDS_Sit_Commu.h"
#include <QtCore/QCoreApplication>
#include <QtWidgets//QApplication>
#include "QCvWidget.h"

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

void test(CDS_Sit_Commu * ptheCDS_Sit_Commu)
{
	string tt;
	int pe;
	while (true)
	{
		cin >> tt;

		if (tt == "start")
		{
			ptheCDS_Sit_Commu->StartKinect();
		}
		else if (tt == "stop")
		{
			ptheCDS_Sit_Commu->StopKinect();
		}
		else if (tt == "1")
		{
			ptheCDS_Sit_Commu->SetDisplayMode(1);
		}
		else if (tt == "2")
		{
			ptheCDS_Sit_Commu->SetDisplayMode(2);
		}
		else if (tt == "3")
		{
			ptheCDS_Sit_Commu->SetDisplayMode(3);
		}
		else if (tt == "save")
		{
			ptheCDS_Sit_Commu->SaveStandard("Oreon Jiang",0);
			ptheCDS_Sit_Commu->SaveStandard("Oreon Jiang",1);
		}
		else if (tt == "load")
		{
			ptheCDS_Sit_Commu->LoadStandard("Oreon Jiang");
		}
		else if (tt == "remove")
		{
			ptheCDS_Sit_Commu->RemoveStandard("Oreon Jiang");
		}
		else if (tt == "s1")
		{
			ptheCDS_Sit_Commu->StartEvaluationSit();
		}
		else if (tt == "s2")
		{
			ptheCDS_Sit_Commu->StopEvaluationSit();
		}
		else if (tt == "setb")
		{
			double _in;
			cout << "SetBrightness: ";
			cin >> _in;
			ptheCDS_Sit_Commu->SetBrightness(_in);
		}
		else if (tt == "setc")
		{
			double _in;
			cout << "SetContrast: ";
			cin >> _in;
			ptheCDS_Sit_Commu->SetContrast(_in);
		}
		else if (tt == "scan")
		{
			vector<string>Names;
			ptheCDS_Sit_Commu->ScanStandard(Names);
			for (size_t i = 0; i < Names.size(); i++)
			{
				cout << Names[i] << endl;
			}
		}
		else if (tt == "sete")
		{
			int _index;
			double _degree;
			cout << "Index: ";
			cin >> _index;
			cout << "Degree: ";
			cin >> _degree;
			ptheCDS_Sit_Commu->SetErrorRange(_index, _degree);
		}
		else if (tt == "gete")
		{
			vector<double>_ranges;
			ptheCDS_Sit_Commu->GetErrorRange(_ranges);
			for (size_t i = 0; i < _ranges.size(); i++)
			{
				cout << _ranges[i] << endl;
			}
		}
		else
		{
			cout << "input error" << endl;
		}
	}

}

void test2(CDS_Sit_Commu * ptheCDS_Sit_Commu)
{
	while (true)
	{
		cout << ptheCDS_Sit_Commu->GetErrorState() << endl;
		cv::waitKey(100);
	}
}


int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	CDS_Sit_Commu theCDS_Sit_Commu;
	theCDS_Sit_Commu.Intiliaze();

	//==============================================================//

	QCvWidget left;
	left.setWindowTitle("left");
	left.setGeometry(460 * 1, 35 + 520, 480, 480);
	left.resize(479, 481);
	left.show();
	left.Start();
	theCDS_Sit_Commu.SetQCvWidget(&left);

	//==============================================================//

	theCDS_Sit_Commu.StartKinect();

	//==============================================================//

	boost::thread mThread(test, &theCDS_Sit_Commu);
	
	//boost::thread mThread2(test2, &theCDS_Sit_Commu);
	
	app.exec();
	theCDS_Sit_Commu.Release();
	system("pause");
	return 0;
}