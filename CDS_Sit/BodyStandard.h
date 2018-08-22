/*=========================================================================

Program:
Module:    BodyStandard.h

OREO 2016/12/01
=========================================================================*/
#pragma once
#include <string>
#include <vector>
#include <Eigen/Dense>

// Kinect for Windows SDK Header
#include <Kinect.h>
#pragma comment(lib,"Kinect20.lib")

using namespace std;

#define RefJoint 0
#define TargetJoint 1

class BodyStandard
{
public:
	Joint Joints[JointType::JointType_Count];
	JointOrientation JointOris[JointType::JointType_Count];

	vector<Eigen::Vector3d>Pos;
	vector<vector<int>> RefJoints;

	string name;
private:

};


