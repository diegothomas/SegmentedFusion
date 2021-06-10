#pragma once

#include"Header.h"
#include"TSDFManager.h"
#include "Functions.h"

class BodyPart
{
public:
	BodyPart();
	BodyPart(cl_context,cl_device_id, vector<Vec3>, cv::Mat,cv::Mat, cl_mem,cl_mem,int bdynum,int miss=0);
	BodyPart(cl_context _context, cl_device_id _device, vector<Vec3d> _points, cv::Mat depth, cl_mem _depthCL, cl_mem _colorCL, int bdynum=BODY_PART_NUMBER, int miss=0);
	~BodyPart();

	void Initialize(vector<Vec3d>& skeVec);

	void Running(cv::Mat depth, Mat color,float boneDQ[8], float jointDQ[8], vector<Vec3d>& skeVec);
	void Running(cv::Mat depth, Mat color,float boneDQ[8], float jointDQ[8], vector<Vec3>& skeVec);

	void Running(float boneDQ[8], float jointDQ[8], vector<Vec3d>& skeVec);
	void Running(float boneDQ[8], float jointDQ[8], vector<Vec3>& skeVec);

	void getOrientation();
	void getOrientation_Bone();
	void setPts(vector<Vec3>);
	void DrawBox();
	void setCenter(Vec3);
	void MinMaxVal();
	void setPlaneF();

	void TransfomMesh(float pose[16]);
	void TransfomMesh(Eigen::Matrix4d);

	void SetD2C(float pose[16]);

	vector<Vec3>* GetPts(int num);

	void SavePLY(string filename);

	void DrawMesh();
	void DrawTransfomedMesh();
	//void AddPoseMat(Eigen::Matrix4f bonePose);

private:
	Vec3 courners[8];
	Vec3 EigenVec[3];
	Vec3 eigen_vecs[3];
	double eigen_val[3];
	Vec3 p1, p2, p3;
	Vec3 boneV, point;

	vector<Vec3> pts;
	vector<Vec3> skeletonVec3;
	//cv::Mat maskimage;
	Mat depthimage;
	int PartNum;
	Vec3 center;

	Vec3 ax_x, ax_y, ax_z;

	unique_ptr<TSDFManager> TSDF;
	cl_context context;
	cl_device_id device;
	cl_mem colorCL, depthCL;

	float min_x, min_y, min_z;
	float max_x, max_y, max_z;
	Vec3 BodySize;
	float intrinsic[11];
	float planeFunction[4];

	Eigen::Matrix4d allPose;

	int myBodyNumber,miss;
	static const int num_oflist = 19;

	int bonelist[num_oflist][2] = { { SpineShoulder,Head },{ ShoulderRight,ElbowRight },{ ShoulderLeft,ElbowLeft },
	{ ElbowRight,WristRight },{ ElbowLeft,WristLeft },{ HipRight,KneeRight },{ HipLeft,KneeLeft },
	{ KneeRight,AnkleRight },{ KneeLeft,AnkleLeft },{ SpineBase,SpineShoulder },
	{ ElbowRight,HandRight },{ ElbowLeft,HandLeft },{ KneeRight,FootRight },{ KneeLeft,FootLeft },//0~13
	{ SpineShoulder,ShoulderRight },{ SpineShoulder,ShoulderLeft },
	{ SpineMid, HipRight },{ SpineMid,HipLeft },{ SpineBase, SpineShoulder } };//~18


	int boneParent[num_oflist] = { BODY_PART_TORSOR,14,15,
		BODY_PART_UPPER_ARM_R,BODY_PART_UPPER_ARM_L,16,17,
		BODY_PART_UP_LEG_R,BODY_PART_UP_LEG_L,18,
		BODY_PART_FOREARM_R,BODY_PART_FOREARM_L,BODY_PART_DOWN_LEG_R,BODY_PART_DOWN_LEG_L,//~13
		18, 18,BODY_PART_TORSOR,BODY_PART_TORSOR , BODY_PART_TORSOR };

};

