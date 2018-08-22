/*=========================================================================

Program:   
Module:    CDS_Sit.cpp

OREO 2016/11/29
=========================================================================*/
#include "CDS_Sit.h"

CDS_Sit::CDS_Sit()
{
	cout << "Creat CDS_Sit.DLL" << endl;

	//================初始化資料庫資料夾==================//

	this->WorkPath = boost::filesystem::current_path();
	this->LibPath = this->WorkPath / "StandardLib";

	while (!is_directory(this->LibPath))
	{
		if (!exists(this->LibPath))
		{
			cout << "Have no StandardLib folder" << endl;
			cout << "Crate StandardLib folder: " << endl << this->LibPath << endl;
			boost::filesystem::create_directory(this->LibPath);
		}
		else
		{
			this->LibPath += "1";
		}
	}

	//=================載入設定檔==================//

	this->INIPath = this->LibPath.string() + "\\CDSSit_ErrorRange.ini";

	if ( !INIer.ReadINI(this->INIPath.string()) )
	{
		this->InitilizeINI();
	}

	//==================評分相關====================//

	this->m_bFinishCaptureStandard = false;
	this->m_bStandardIsReady = false;
	this->m_bEvaluationSit = false;
	this->m_bPositionCorrect = true;

	int source, target;
	this->JointNumber = INIer.GetValue<int>("System.Joint Number", 8);

	this->m_vdErrorRange.resize(this->JointNumber);
	this->Standard.RefJoints.resize(this->JointNumber);
	this->Standard.Pos.resize(JointType_Count);

	string JointIndex;
	for (size_t i = 0; i < this->JointNumber; i++)
	{
		JointIndex = "Joint" + boost::lexical_cast<string>(i);
		this->Standard.RefJoints[i].resize(2);
		this->Standard.RefJoints[i][RefJoint] = INIer.GetValue<int>(JointIndex + ".Ref", 0);
		this->Standard.RefJoints[i][TargetJoint] = INIer.GetValue<int>(JointIndex + ".Target", 0);
		this->m_vdErrorRange[i] = INIer.GetValue<int>(JointIndex + ".Range", 15);
	}

	//=================Kinect相關==================//

	this->pSensor = nullptr;

	this->pColorFrameReader = nullptr;
	this->pColorFrameSource = nullptr;
	this->pColorFrame = nullptr;

	this->pDepthFrameSource = nullptr;
	this->pDepthFrameReader = nullptr;
	this->pDepthFrame = nullptr;
	this->m_iDepthWidth = 0;
	this->m_iDepthHeight = 0;
	this->m_uDepthMin = 0;
	this->m_uDepthMax = 0;

	this->pBodyFrameReader = nullptr;
	this->aBodyData = nullptr;
	this->iBodyCount = 0;
	this->pBodyFrameSource = nullptr;
	this->pCoordinateMapper = nullptr;
	this->pBodyFrame = nullptr;


	this->m_bTracking = false;
	this->m_bCVWindowOpen = false;
	this->m_bGetImageIng = true;

	//=================OpenCV相關==================//

	this->Dmode = DisplayMode_Color;
	this->m_dBrightness = 0;
	this->m_dContrast = 1;

	//===============BoostLOG相關===================//

	//g_InitLog();
	//BOOST_LOG_SEV(this->lg, Log_Debug) << "TEST";

	//==================TEST========================//
}

//=================Kinect相關==================//

CDS_Sit::~CDS_Sit()
{
	cout << "Delete CDS_Sit.DLL" << endl;
}

int CDS_Sit::Release()
{
	this->StopKinect();

	return 0;
}

void CDS_Sit::InitilizeINI()
{
	cout << "InitilizeINI" << endl;

	INIer.PutValue("System.Joint Number", 8);
	INIer.PutValue("System.Index 0", "SpineBase");
	INIer.PutValue("System.Index 1", "SpineMid");
	INIer.PutValue("System.Index 2", "Neck");
	INIer.PutValue("System.Index 3", "Head");
	INIer.PutValue("System.Index 4", "ShoulderLeft");
	INIer.PutValue("System.Index 5", "ElbowLeft");
	INIer.PutValue("System.Index 6", "WristLeft");
	INIer.PutValue("System.Index 7", "HandLeft");
	INIer.PutValue("System.Index 8", "ShoulderRight");
	INIer.PutValue("System.Index 9", "ElbowRight");
	INIer.PutValue("System.Index 10", "WristRight");
	INIer.PutValue("System.Index 11", "HandRight");
	INIer.PutValue("System.Index 12", "HipLeft");
	INIer.PutValue("System.Index 13", "KneeLeft");
	INIer.PutValue("System.Index 14", "AnkleLeft");
	INIer.PutValue("System.Index 15", "FootLeft");
	INIer.PutValue("System.Index 16", "HipRight");
	INIer.PutValue("System.Index 17", "KneeRight");
	INIer.PutValue("System.Index 18", "AnkleRight");
	INIer.PutValue("System.Index 19", "FootRight");
	INIer.PutValue("System.Index 20", "SpineShoulder");
	INIer.PutValue("System.Index 21", "HandTipLeft");
	INIer.PutValue("System.Index 22", "ThumbLeft");
	INIer.PutValue("System.Index 23", "HandTipRight");
	INIer.PutValue("System.Index 24", "ThumbRight");

	string JointIndex;
	JointIndex = "Joint" + boost::lexical_cast<string>(0);
	INIer.PutValue<string>(JointIndex + ".JointType", "_Head");
	INIer.PutValue<int>(JointIndex + ".Ref", JointType_Neck);
	INIer.PutValue<int>(JointIndex + ".Target", JointType_Head);
	INIer.PutValue<double>(JointIndex + ".Range", 15);

	JointIndex = "Joint" + boost::lexical_cast<string>(1);
	INIer.PutValue<string>(JointIndex + ".JointType", "_Neck");
	INIer.PutValue<int>(JointIndex + ".Ref", JointType_SpineShoulder);
	INIer.PutValue<int>(JointIndex + ".Target", JointType_Neck);
	INIer.PutValue<double>(JointIndex + ".Range", 5);

	JointIndex = "Joint" + boost::lexical_cast<string>(2);
	INIer.PutValue<string>(JointIndex + ".JointType", "_ShoulderLeft");
	INIer.PutValue<int>(JointIndex + ".Ref", JointType_SpineShoulder);
	INIer.PutValue<int>(JointIndex + ".Target", JointType_ShoulderLeft);
	INIer.PutValue<double>(JointIndex + ".Range", 5);

	JointIndex = "Joint" + boost::lexical_cast<string>(3);
	INIer.PutValue<string>(JointIndex + ".JointType", "_ShoulderRight");
	INIer.PutValue<int>(JointIndex + ".Ref", JointType_SpineShoulder);
	INIer.PutValue<int>(JointIndex + ".Target", JointType_ShoulderRight);
	INIer.PutValue<double>(JointIndex + ".Range", 5);

	JointIndex = "Joint" + boost::lexical_cast<string>(4);
	INIer.PutValue<string>(JointIndex + ".JointType", "_SpineMid");
	INIer.PutValue<int>(JointIndex + ".Ref", JointType_SpineShoulder);
	INIer.PutValue<int>(JointIndex + ".Target", JointType_SpineMid);
	INIer.PutValue<double>(JointIndex + ".Range", 5);

	JointIndex = "Joint" + boost::lexical_cast<string>(5);
	INIer.PutValue<string>(JointIndex + ".JointType", "_SpineBase");
	INIer.PutValue<int>(JointIndex + ".Ref", JointType_SpineMid);
	INIer.PutValue<int>(JointIndex + ".Target", JointType_SpineBase);
	INIer.PutValue<double>(JointIndex + ".Range", 5);

	JointIndex = "Joint" + boost::lexical_cast<string>(6);
	INIer.PutValue<string>(JointIndex + ".JointType", "_ElbowLeft");
	INIer.PutValue<int>(JointIndex + ".Ref", JointType_ShoulderLeft);
	INIer.PutValue<int>(JointIndex + ".Target", JointType_ElbowLeft);
	INIer.PutValue<double>(JointIndex + ".Range", 20);

	JointIndex = "Joint" + boost::lexical_cast<string>(7);
	INIer.PutValue<string>(JointIndex + ".JointType", "_ElbowRight");
	INIer.PutValue<int>(JointIndex + ".Ref", JointType_ShoulderRight);
	INIer.PutValue<int>(JointIndex + ".Target", JointType_ElbowRight);
	INIer.PutValue<double>(JointIndex + ".Range", 20);

	INIer.WriteINI(this->INIPath.string());
}

int CDS_Sit::StartKinect()
{
	// 0. Check Track state
	if (this->m_bTracking)
	{
		cout << "Kinect is already start" << endl;
		return -1;
	}

	// 1a. Get default Sensor
	cout << "Try to get default sensor" << endl;
	if (GetDefaultKinectSensor(&this->pSensor) != S_OK)
	{
		cerr << "Get Sensor failed" << endl;
		return -2;
	}

	// 1b. Open sensor
	cout << "Try to open sensor" << endl;
	if (this->pSensor->Open() != S_OK)
	{
		cerr << "Can't open sensor" << endl;
		return -2;
	}

	this->PrepareColorFrame();
	this->PrepareDepthFrame();
	this->PrepareBodyFrame();

	// 4. get CoordinateMapper
	if (this->pSensor->get_CoordinateMapper(&this->pCoordinateMapper) != S_OK)
	{
		cout << "Can't get coordinate mapper" << endl;
		return -2;
	}

	this->m_bTracking = true;
	try
	{
		this->m_KinectThread = boost::thread(boost::bind(&CDS_Sit::Run, this));
	}
	catch (std::exception& e)
	{
		cout << e.what() << endl;
		return -3;
	}

	return 0;
}

int CDS_Sit::StopKinect()
{
	if (!this->m_bTracking)
	{
		cout << "Kinect is already stop" << endl;
		return -1;
	}

	this->m_bTracking = false;

	try
	{
		this->m_KinectThread.join();
	}
	catch (std::exception& e)
	{
		cout << e.what() << endl;
		this->m_bTracking = true;
		return -2;
	}

	//====================================================================

	// 3. delete body data array
	delete[] this->aBodyData;

	// 3. release frame reader
	cout << "Release body frame reader" << endl;
	this->pBodyFrameReader->Release();
	this->pBodyFrameReader = nullptr;

	// 2. release color & depth frame reader
	cout << "Release color & depth frame reader" << endl;
	if (this->pColorFrameReader != nullptr)
	{
		this->pColorFrameReader->Release();
		this->pColorFrameReader = nullptr;
	}
	if (this->pDepthFrameReader != nullptr)
	{
		this->pDepthFrameReader->Release();
		this->pDepthFrameReader = nullptr;
	}

	// 1c. Close Sensor
	cout << "close sensor" << endl;
	this->pSensor->Close();

	// 1d. Release Sensor
	cout << "Release sensor" << endl;
	this->pSensor->Release();
	this->pSensor = nullptr;

	//====================================================================

	return 0;
}

int CDS_Sit::PrepareColorFrame()
{
	// 2. Color Related code
	this->uBufferSize = 0;

	// 2a. Get color frame source
	cout << "Try to get color source" << endl;
	if (this->pSensor->get_ColorFrameSource(&this->pColorFrameSource) != S_OK)
	{
		cerr << "Can't get color frame source" << endl;
		return -1;
	}

	// 2b. Get frame description
	cout << "get color frame description" << endl;
	int		iWidth = 0;
	int		iHeight = 0;
	IFrameDescription* pFrameDescription = nullptr;
	if (this->pColorFrameSource->get_FrameDescription(&pFrameDescription) == S_OK)
	{
		pFrameDescription->get_Width(&iWidth);
		pFrameDescription->get_Height(&iHeight);
	}
	pFrameDescription->Release();
	pFrameDescription = nullptr;

	// 2c. get frame reader
	cout << "Try to get color frame reader" << endl;
	if (this->pColorFrameSource->OpenReader(&this->pColorFrameReader) != S_OK)
	{
		cerr << "Can't get color frame reader" << endl;
		return -1;
	}

	// 2d. release Frame source
	cout << "Release frame source" << endl;
	this->pColorFrameSource->Release();
	this->pColorFrameSource = nullptr;

	// Prepare OpenCV data
	this->mColorImg = cv::Mat(iHeight, iWidth, CV_8UC4);
	this->uBufferSize = iHeight * iWidth * 4 * sizeof(BYTE);

	return 0;
}

int CDS_Sit::PrepareDepthFrame()
{
	// 2a. Get color frame source
	cout << "Try to get depth source" << endl;
	if (this->pSensor->get_DepthFrameSource(&this->pDepthFrameSource) != S_OK)
	{
		cerr << "Can't get depth frame source" << endl;
		return -1;
	}

	// 2b. Get frame description
	cout << "get depth frame description" << endl;
	IFrameDescription* pFrameDescription = nullptr;
	if (this->pDepthFrameSource->get_FrameDescription(&pFrameDescription) == S_OK)
	{
		pFrameDescription->get_Width(&this->m_iDepthWidth);
		pFrameDescription->get_Height(&this->m_iDepthHeight);
	}
	pFrameDescription->Release();
	pFrameDescription = nullptr;

	// 2c. get some dpeth only meta
	this->pDepthFrameSource->get_DepthMinReliableDistance(&this->m_uDepthMin);
	this->pDepthFrameSource->get_DepthMaxReliableDistance(&this->m_uDepthMax);
	cout << "Reliable Distance: "
		<< this->m_uDepthMin << " – " << this->m_uDepthMax << endl;


	// 2d. get frame reader
	cout << "Try to get depth frame reader" << endl;
	if (this->pDepthFrameSource->OpenReader(&this->pDepthFrameReader) != S_OK)
	{
		cerr << "Can't get depth frame reader" << endl;
		return -1;
	}

	// 2e. release Frame source
	cout << "Release frame source" << endl;
	this->pDepthFrameSource->Release();
	this->pDepthFrameSource = nullptr;

	// Prepare OpenCV data
	this->mDepthImg = cv::Mat(this->m_iDepthHeight, this->m_iDepthWidth, CV_16UC1);
	this->mImg8bit = cv::Mat(this->m_iDepthHeight, this->m_iDepthWidth, CV_8UC1);

	return 0;
}

int CDS_Sit::PrepareBodyFrame()
{
	// 3. Body related code
	{
		// 3a. Get frame source
		cout << "Try to get body source" << endl;
		if (this->pSensor->get_BodyFrameSource(&this->pBodyFrameSource) != S_OK)
		{
			cerr << "Can't get body frame source" << endl;
			return -1;
		}

		// 3b. Get the number of body
		if (this->pBodyFrameSource->get_BodyCount(&this->iBodyCount) != S_OK)
		{
			cerr << "Can't get body count" << endl;
			return -1;
		}
		cout << " > Can trace " << this->iBodyCount << " bodies" << endl;
		this->aBodyData = new IBody*[this->iBodyCount];
		for (int i = 0; i < this->iBodyCount; ++i)
			this->aBodyData[i] = nullptr;

		// 3c. get frame reader
		cout << "Try to get body frame reader" << endl;
		if (this->pBodyFrameSource->OpenReader(&this->pBodyFrameReader) != S_OK)
		{
			cerr << "Can't get body frame reader" << endl;
			return -1;
		}

		// 3d. release Frame source
		cout << "Release frame source" << endl;
		this->pBodyFrameSource->Release();
		this->pBodyFrameSource = nullptr;
	}
}

void CDS_Sit::Run()
{
	//暫存第一個得到的骨架序號
	int FirstBody_index = -1;
	bool FirstBody = true;

	// Enter main loop
	while (m_bTracking)
	{
		while (!this->m_bGetImageIng)
		{
			(double)cv::getTickCount();
		}

		// 4a. Get last frame
		switch (this->Dmode)
		{
		case DisplayMode_Color:
			if (this->pColorFrameReader->AcquireLatestFrame(&this->pColorFrame) == S_OK)
			{
				if (this->pColorFrame->CopyConvertedFrameDataToArray(this->uBufferSize, this->mColorImg.data, ColorImageFormat_Bgra) != S_OK)
				{
					cerr << "Data copy error" << endl;
				}
				this->pColorFrame->Release();
			}
			this->mImg = this->mColorImg.clone();
			break;

		case DisplayMode_Gray:
			if (this->pColorFrameReader->AcquireLatestFrame(&this->pColorFrame) == S_OK)
			{
				if (this->pColorFrame->CopyConvertedFrameDataToArray(this->uBufferSize, this->mColorImg.data, ColorImageFormat_Bgra) != S_OK)
				{
					cerr << "Data copy error" << endl;
				}
				this->pColorFrame->Release();
			}
			this->mImg = this->mColorImg.clone();
			cv::cvtColor(this->mImg, this->mImg, cv::COLOR_RGBA2GRAY);
			cv::cvtColor(this->mImg, this->mImg, cv::COLOR_GRAY2RGBA);
			break;

		case DisplayMode_Depth:
			if (this->pDepthFrameReader->AcquireLatestFrame(&this->pDepthFrame) == S_OK)
			{
				if (this->pDepthFrame->CopyFrameDataToArray(this->m_iDepthWidth * this->m_iDepthHeight,
					reinterpret_cast<UINT16*>(mDepthImg.data)) != S_OK)
				{
					cerr << "Data copy error" << endl;
				}
				else
				{
					this->mDepthImg.convertTo(this->mImg8bit, CV_8U, 255.0f / this->m_uDepthMax);
				}
				this->pDepthFrame->Release();
			}
			this->mImg = this->mImg8bit.clone();
			cv::cvtColor(this->mImg, this->mImg, cv::COLOR_GRAY2RGBA);
			break;
		}
		//調整對比
		this->mImg.convertTo(mImg, -1, this->m_dContrast, this->m_dBrightness);

		// 4b. Get body data
		if (this->pBodyFrameReader->AcquireLatestFrame(&this->pBodyFrame) == S_OK)
		{
			// 4b. get Body data
			if (this->pBodyFrame->GetAndRefreshBodyData(this->iBodyCount, this->aBodyData) == S_OK)
			{
				// 4c. for each body
				for (int i = 0; i < this->iBodyCount; ++i)
				{
					//若不是第一個擷取到的骨架，則continue
					if (!FirstBody)
					{
						if (i != FirstBody_index)
						{
							continue;
						}
					}

					IBody* pBody = this->aBodyData[i];

					// check if is tracked
					BOOLEAN bTracked = false;
					if ((pBody->get_IsTracked(&bTracked) == S_OK) && bTracked)
					{
						// get joint position
						if (pBody->GetJoints(JointType::JointType_Count, this->aJoints) == S_OK)
						{
							if (FirstBody)
							{
								FirstBody_index = i;
								FirstBody = false;
							}
							pBody->GetJointOrientations(JointType::JointType_Count, this->aOrientations);

							this->DrawLine(this->mImg, aJoints[JointType_Neck], aJoints[JointType_Head], pCoordinateMapper,0);
							this->DrawLine(this->mImg, aJoints[JointType_SpineShoulder], aJoints[JointType_Neck], pCoordinateMapper,0);
							this->DrawLine(this->mImg, aJoints[JointType_SpineShoulder], aJoints[JointType_ShoulderLeft], pCoordinateMapper, 0);
							this->DrawLine(this->mImg, aJoints[JointType_SpineShoulder], aJoints[JointType_ShoulderRight], pCoordinateMapper, 0);
							this->DrawLine(this->mImg, aJoints[JointType_SpineShoulder], aJoints[JointType_SpineMid], pCoordinateMapper,0);
							this->DrawLine(this->mImg, aJoints[JointType_ShoulderLeft], aJoints[JointType_ElbowLeft], pCoordinateMapper,0);
							this->DrawLine(this->mImg, aJoints[JointType_ShoulderRight], aJoints[JointType_ElbowRight], pCoordinateMapper,0);
							this->DrawLine(this->mImg, aJoints[JointType_SpineBase], aJoints[JointType_SpineMid], pCoordinateMapper,0);

							//紀錄標準
							this->mSignal_SaveStandard();

							//坐姿評分
							if (this->m_bEvaluationSit)
								this->EvaluationSit();
						}
					}
					else
					{
						FirstBody_index = -1;
						FirstBody = true;
					}
				}
			}
			else
			{
				std::cout << "Can't read body data" << endl;
			}

			// 4e. release frame
			this->pBodyFrame->Release();
		}

		//完成取像，通知左攝影機畫面刷新
		if (this->m_bCVWindowOpen)
		{
			this->m_bGetImageIng = false;
		}
		Sleep(30);
	}
}

void CDS_Sit::DrawLine(cv::Mat& rImg, const Joint& rJ1, const Joint& rJ2, ICoordinateMapper* pCMapper, bool red)
{
	if (rJ1.TrackingState == TrackingState_NotTracked || rJ2.TrackingState == TrackingState_NotTracked)
		return;

	if (this->Dmode == DisplayMode_Depth)
	{
		DepthSpacePoint ptJ1, ptJ2;
		pCMapper->MapCameraPointToDepthSpace(rJ1.Position, &ptJ1);
		pCMapper->MapCameraPointToDepthSpace(rJ2.Position, &ptJ2);

		if (red)
		{
			cv::line(rImg, cv::Point(ptJ1.X, ptJ1.Y), cv::Point(ptJ2.X, ptJ2.Y), cv::Scalar(0.0, 0.0, 255.0), 20);
		}
		else
		{
			cv::line(rImg, cv::Point(ptJ1.X, ptJ1.Y), cv::Point(ptJ2.X, ptJ2.Y), cv::Scalar(0.0, 255.0, 0.0), 20);
		}
	}
	else
	{
		ColorSpacePoint ptJ1, ptJ2;
		pCMapper->MapCameraPointToColorSpace(rJ1.Position, &ptJ1);
		pCMapper->MapCameraPointToColorSpace(rJ2.Position, &ptJ2);

		if (red)
		{
			cv::line(rImg, cv::Point(ptJ1.X, ptJ1.Y), cv::Point(ptJ2.X, ptJ2.Y), cv::Scalar(0.0, 0.0, 255.0), 20);
		}
		else
		{
			cv::line(rImg, cv::Point(ptJ1.X, ptJ1.Y), cv::Point(ptJ2.X, ptJ2.Y), cv::Scalar(0.0, 255.0, 0.0), 20);
		}
	}

}

//==================評分相關===================//
//---------擷取標準---------//

int CDS_Sit::SaveStandard(string _name, bool _OverWrite)
{
	cout << "Save Standard" << endl;
	if (!this->m_bTracking)
	{
		cout << "Is not tracking" << endl;
		return -2;
	}

	if (!_OverWrite || !this->m_bFinishCaptureStandard)
	{
		this->m_bFinishCaptureStandard = false;
		this->mSignal_SaveStandard.connect(boost::bind(&CDS_Sit::CaptureStandard, this));
	}

	int count = 0;
	while (!this->m_bFinishCaptureStandard)
	{
		count++;
		Sleep(10);
		if (count > 200)
		{
			this->mSignal_SaveStandard.disconnect_all_slots();
			cout << "No track body" << endl;
			return -3;
		}
	}

	path filename = this->LibPath.string() + "\\CDSSit_" + _name + ".jpg";
	if ( exists(filename) && !_OverWrite)
	{
		cout <<_name<< " is exist" << endl;
		return -1;
	}

	//================================儲存到檔案===================================//
	//JPG
	this->Standard.name = _name;
	filename = this->LibPath.string() + "\\CDSSit_" + this->Standard.name + ".jpg";
	cout << filename << endl;
	cv::flip(this->saveImg, this->saveImg, 1);
	cv::imwrite(filename.string(), this->saveImg);

	//XML
	filename = this->LibPath.string() + "\\CDSSit_" + this->Standard.name + ".xml";
	cout << filename << endl;

	XMLer.PutValue("Name", this->Standard.name);

	string temp;
	for (size_t i = 0; i < JointType_Count; i++)
	{
		temp = boost::lexical_cast<string>(i);
		XMLer.PutValue("Joint" + temp + ".Postion.X", this->Standard.Joints[i].Position.X);
		XMLer.PutValue("Joint" + temp + ".Postion.Y", this->Standard.Joints[i].Position.Y);
		XMLer.PutValue("Joint" + temp + ".Postion.Z", this->Standard.Joints[i].Position.Z);
		XMLer.PutValue("Joint" + temp + ".Orientation.X", this->Standard.JointOris[i].Orientation.x);
		XMLer.PutValue("Joint" + temp + ".Orientation.Y", this->Standard.JointOris[i].Orientation.y);
		XMLer.PutValue("Joint" + temp + ".Orientation.Z", this->Standard.JointOris[i].Orientation.z);
		XMLer.PutValue("Joint" + temp + ".Orientation.W", this->Standard.JointOris[i].Orientation.w);
		XMLer.PutValue("Joint" + temp + ".RefPostion.X", this->Standard.Pos[i](0));
		XMLer.PutValue("Joint" + temp + ".RefPostion.Y", this->Standard.Pos[i](1));
		XMLer.PutValue("Joint" + temp + ".RefPostion.Z", this->Standard.Pos[i](2));
	}
	XMLer.PutValue("Joint0.<xmlattr>.type", "SpineBase");
	XMLer.PutValue("Joint1.<xmlattr>.type", "SpineMid");
	XMLer.PutValue("Joint2.<xmlattr>.type", "Neck");
	XMLer.PutValue("Joint3.<xmlattr>.type", "Head");
	XMLer.PutValue("Joint4.<xmlattr>.type", "ShoulderLeft");
	XMLer.PutValue("Joint5.<xmlattr>.type", "ElbowLeft");
	XMLer.PutValue("Joint6.<xmlattr>.type", "WristLeft");
	XMLer.PutValue("Joint7.<xmlattr>.type", "HandLeft");
	XMLer.PutValue("Joint8.<xmlattr>.type", "ShoulderRight");
	XMLer.PutValue("Joint9.<xmlattr>.type", "ElbowRight");
	XMLer.PutValue("Joint10.<xmlattr>.type", "WristRight");
	XMLer.PutValue("Joint11.<xmlattr>.type", "HandRight");
	XMLer.PutValue("Joint12.<xmlattr>.type", "HipLeft");
	XMLer.PutValue("Joint13.<xmlattr>.type", "KneeLeft");
	XMLer.PutValue("Joint14.<xmlattr>.type", "AnkleLeft");
	XMLer.PutValue("Joint15.<xmlattr>.type", "FootLeft");
	XMLer.PutValue("Joint16.<xmlattr>.type", "HipRight");
	XMLer.PutValue("Joint17.<xmlattr>.type", "KneeRight");
	XMLer.PutValue("Joint18.<xmlattr>.type", "AnkleRight");
	XMLer.PutValue("Joint19.<xmlattr>.type", "FootRight");
	XMLer.PutValue("Joint20.<xmlattr>.type", "SpineShoulder");
	XMLer.PutValue("Joint21.<xmlattr>.type", "HandTipLeft");
	XMLer.PutValue("Joint22.<xmlattr>.type", "ThumbLeft");
	XMLer.PutValue("Joint23.<xmlattr>.type", "HandTipRight");
	XMLer.PutValue("Joint24.<xmlattr>.type", "ThumbRight");

	XMLer.WriteXML(filename.string());

	return 0;
}

int CDS_Sit::LoadStandard(string _name)
{
	path name_temp = this->LibPath.string() + "\\CDSSit_" + _name + ".xml";

	if (!XMLer.ReadXML(name_temp.string()))
		return -1;

	XMLer.PutValue("Name", this->Standard.name);
	this->Standard.name = XMLer.GetValue<string>("Name");
	string temp;
	for (size_t i = 0; i < JointType_Count; i++)
	{
		temp = boost::lexical_cast<string>(i);
		this->Standard.Joints[i].Position.X = XMLer.GetValue<float>("Joint" + temp + ".Postion.X");
		this->Standard.Joints[i].Position.Y = XMLer.GetValue<float>("Joint" + temp + ".Postion.Y");
		this->Standard.Joints[i].Position.Z = XMLer.GetValue<float>("Joint" + temp + ".Postion.Z");
		this->Standard.JointOris[i].Orientation.x = XMLer.GetValue<float>("Joint" + temp + ".Orientation.X");
		this->Standard.JointOris[i].Orientation.y = XMLer.GetValue<float>("Joint" + temp + ".Orientation.Y");
		this->Standard.JointOris[i].Orientation.z = XMLer.GetValue<float>("Joint" + temp + ".Orientation.Z");
		this->Standard.JointOris[i].Orientation.w = XMLer.GetValue<float>("Joint" + temp + ".Orientation.W");
		this->Standard.Pos[i](0) = XMLer.GetValue<float>("Joint" + temp + ".RefPostion.X");
		this->Standard.Pos[i](1) = XMLer.GetValue<float>("Joint" + temp + ".RefPostion.Y");
		this->Standard.Pos[i](2) = XMLer.GetValue<float>("Joint" + temp + ".RefPostion.Z");
	}
	cout << "Load Success: "<< name_temp << endl;
	this->m_bStandardIsReady = true;
}

int CDS_Sit::ScanStandard(vector<string>& _Names)
{
	_Names.clear();
	if (exists(this->LibPath))
	{
		if (is_directory(this->LibPath))
		{
			vector<path> vList;
			copy(directory_iterator(this->LibPath), directory_iterator(), back_inserter(vList));
			for (vector<path>::const_iterator it = vList.begin(); it != vList.end(); ++it)
			{
				if (!is_directory(*it))
				{
					if ((*it).extension() == ".xml")
					{
						if (XMLer.ReadXML((*it).string()))
						{
							_Names.push_back(XMLer.GetValue<string>("Name"));
						}
					}
				}
			}
		}
	}
	else
	{
		return -1;
	}

	return 0;
}

int CDS_Sit::RemoveStandard(string _name)
{
	path name_temp = this->LibPath.string() + "\\CDSSit_" + _name + ".xml";
	try
	{
		boost::filesystem::remove(name_temp);
	}
	catch (const std::exception& e)
	{
		cout << e.what();
		return -1;
	}

	name_temp = this->LibPath.string() + "\\CDSSit_" + _name + ".jpg";
	try
	{
		boost::filesystem::remove(name_temp);
	}
	catch (const std::exception& e)
	{
		cout << e.what();
		return -2;
	}

	return 0;
}

int CDS_Sit::SetErrorRange(int _index, double _degree)
{
	cout << "SetErrorRange, Index = " << _index << ", degree = " << _degree << endl;
	if (_index < 0 || _index > this->JointNumber)
	{
		cout << "The _index input error" << endl;
		return -1;
	}

	this->m_vdErrorRange[_index] = _degree;

	string JointIndex;
	JointIndex = "Joint" + boost::lexical_cast<string>(_index);
	INIer.PutValue<double>(JointIndex + ".Range", _degree);
	INIer.WriteINI(this->INIPath.string());

	return 0;
}

int CDS_Sit::GetErrorRange(vector<double>& _ranges)
{
	_ranges.resize(this->JointNumber);
	_ranges = this->m_vdErrorRange;

	return 0;
}

void CDS_Sit::CaptureStandard()
{
	cout << "Capture Standard" << endl;
	this->mSignal_SaveStandard.disconnect_all_slots();

	for (size_t i = 0; i < JointType_Count; i++)
	{
		this->Standard.Joints[i] = this->aJoints[i];
		this->Standard.JointOris[i] = this->aOrientations[i];
	}

	int ref, target;
	for (size_t i = 0; i < this->Standard.RefJoints.size(); i++)
	{
		ref = this->Standard.RefJoints[i][RefJoint];
		target = this->Standard.RefJoints[i][TargetJoint];
		if ( (target== JointType_SpineMid) || (target == JointType_SpineBase) )
		{
			this->Standard.Pos[target] = this->TransR(this->Standard.Joints[ref], this->Standard.Joints[target]);
		}
		else
		{
			this->Standard.Pos[target] = this->TransR(this->Standard.JointOris[ref], this->Standard.Joints[ref], this->Standard.Joints[target]);
		}
	}

	//備份影像
	this->mImg.copyTo(this->saveImg);

	this->m_bStandardIsReady = true;
	this->m_bFinishCaptureStandard = true;
}

//---------坐姿評分---------//

int CDS_Sit::StartEvaluationSit()
{
	if (!this->m_bTracking)
	{
		cout << "Is not tracking" << endl;
		return -1;
	}

	if (!this->m_bStandardIsReady)
	{
		cout << "Starndatd is not ready" << endl;
		return -2;
	}

	if (this->m_bEvaluationSit)
	{
		cout << "Already start evaluation sit" << endl;
		return -3;
	}
	cout << "Start evaluation sit" << endl;
	this->m_bEvaluationSit = true;

	return 0;
}

int CDS_Sit::StopEvaluationSit()
{
	if (!this->m_bTracking)
	{
		cout << "Is not tracking" << endl;
		return -1;
	}

	if (!this->m_bStandardIsReady)
	{
		cout << "Starndatd is not ready" << endl;
		return -2;
	}

	if (!this->m_bEvaluationSit)
	{
		cout << "Already stop evaluation sit" << endl;
		return -3;
	}
	cout << "Stop evaluation sit" << endl;
	this->m_bEvaluationSit = false;
	this->m_bPositionCorrect = true;

	return 0;
}

bool CDS_Sit::GetErrorState()
{
	return this->m_bPositionCorrect;
}

void CDS_Sit::EvaluationSit()
{
	int ref, target;

	bool m_bIsErroePosition = false;
	for (size_t i = 0; i < this->Standard.RefJoints.size(); i++)
	{
		ref = this->Standard.RefJoints[i][RefJoint];
		target = this->Standard.RefJoints[i][TargetJoint];

		if (CalculateDegree(ref, target) > (this->m_vdErrorRange[i]) )
		{
			m_bIsErroePosition = true;
			this->DrawLine(this->mImg, aJoints[ref], aJoints[target], pCoordinateMapper, 1);
		}
	}

	if (m_bIsErroePosition)
	{
		this->m_bPositionCorrect = false;
	}
	else
	{
		this->m_bPositionCorrect = true;
	}
}

double CDS_Sit::CalculateDegree(int ref, int target)
{
	Eigen::Vector3d PointReal;

	if ((target == JointType_SpineMid) || (target == JointType_SpineBase))
	{
		PointReal = this->TransR(this->aJoints[ref], this->aJoints[target]);
	}
	else
	{
		PointReal = this->TransR(this->aOrientations[ref], this->aJoints[ref], this->aJoints[target]);
	}

	double costhi = PointReal(0)*this->Standard.Pos[target](0) + PointReal(1)*this->Standard.Pos[target](1) + PointReal(2)*this->Standard.Pos[target](2);
	double thi = std::acos(costhi) / 3.1415926 * 180;

	return thi;
}

Eigen::Vector3d CDS_Sit::TransR(JointOrientation Ori, Joint Pos0, Joint Pos)
{
	Eigen::Quaterniond eRotation(Ori.Orientation.w, Ori.Orientation.x, Ori.Orientation.y, Ori.Orientation.z);
	Eigen::Translation3d eTRanslate(Ori.Orientation.x, Ori.Orientation.y, Ori.Orientation.z);
	Eigen::Affine3d mTransform = eRotation.inverse() * eTRanslate.inverse();

	Eigen::Vector3d ePos0, ePos1, ePos;

	ePos0 = mTransform * Eigen::Vector3d(Pos0.Position.X, Pos0.Position.Y, Pos0.Position.Z);
	ePos1 = mTransform * Eigen::Vector3d(Pos.Position.X, Pos.Position.Y, Pos.Position.Z);
	ePos << ePos1(0) - ePos0(0), ePos1(1) - ePos0(1), ePos1(2) - ePos0(2);
	ePos.normalize();
	return ePos;
}

Eigen::Vector3d CDS_Sit::TransR(Joint Pos0, Joint Pos)
{
	Eigen::Vector3d ePos0, ePos1, ePos;

	ePos0 = Eigen::Vector3d(Pos0.Position.X, Pos0.Position.Y, Pos0.Position.Z);
	ePos1 = Eigen::Vector3d(Pos.Position.X, Pos.Position.Y, Pos.Position.Z);
	ePos << ePos1(0) - ePos0(0), ePos1(1) - ePos0(1), ePos1(2) - ePos0(2);
	ePos.normalize();
	return ePos;
}

//=================OpenCV相關==================//

int CDS_Sit::CVWindowClose()
{
	this->m_bCVWindowOpen = false;
	return 0;
}

int CDS_Sit::GetImage(cv::Mat& in,int H, int W)
{
	this->m_bCVWindowOpen = true;

	if (this->m_bGetImageIng)
		return -2;
	if (!this->m_bTracking)
		return -3;

	cv::resize(mImg, mImg, cv::Size(W, H));
	cv::flip(mImg, mImg, 1);
	mImg.copyTo(in);
	//in = this->mImg.clone();
	//in = this->mImg;
	
	this->m_bCVWindowOpen = false;
	this->m_bGetImageIng = true;

	return 0;
}

int CDS_Sit::SetDisplayMode(int _mode)
{
	if (_mode > 3 || _mode < 1)
		return -1;
	_mode -= 1;
	this->Dmode = (DisplayMode)_mode;
	return 0;
}

int CDS_Sit::SetContrast(double _Contrast)
{
	if ( _Contrast < 0 )
	{
		this->m_dContrast = 0;
	}
	else if( _Contrast > 2 )
	{
		this->m_dContrast = 2;
	}
	else
	{
		this->m_dContrast = _Contrast;
	}


	return 0;
}

int CDS_Sit::SetBrightness(double _Brightness)
{
	if (_Brightness < -255)
	{
		this->m_dBrightness = -255;
	}
	else if (_Brightness > 255)
	{
		this->m_dBrightness = 255;
	}
	else
	{
		this->m_dBrightness = _Brightness;
	}

	return 0;
}



