#pragma once

//include 
#include "Header.h"
#include "Functions.h"
#include "HumanBody.h"
#include "InputManager.h"
#include "ITMManager.h"
#include "myKinect.h"


class SceneManager
{
public:
	SceneManager();
	SceneManager(cl_context _context, cl_device_id _device);
	~SceneManager();
	enum Scene
	{
		ONEFRAME,
		CONTFRAME,
		WAITFRMAE,
		CAPTURE,
		RESET,
		KINECT_INPUT
	};

	void Initialize_Extrinsic();
	void Initialize_Extrinsic_D2D();

	int InitOpenCLMemory();
	void ProcessFrame();
	void Draw();
	
	bool FrameProjection();
	void Capture();
	void Reset();
	bool FrameKinectProjection();

	void SaveSkeletonTr();

	void Processing(Mat & color, Mat & depth, vector<Vec2i>& skeletonVec2, vector<Vec3d>& skeletonVec3);

	//void Comupre3D(Mat depthImage);

	void Comupre3D(int cameranumber, Mat depthImage, Mat colorImage);

	void Draw3D();

	//void Comupre3D();

	void ProcessNextFrame() { scene = ONEFRAME; }
	void ProcessContFrame() { scene = CONTFRAME; }
	void ProcessCapture() { scene = CAPTURE; }
	void ProcessReset() { scene = RESET; }
	void ProcessKINECT() { scene = KINECT_INPUT; }
private:

	//////////////////OpenCL Devices//////////////////////
	std::vector< cl_device_id > lDeviceIds;
	cl_context context;
	cl_device_id device;
	std::vector<cl_kernel> _kernels;
	std::vector<cl_command_queue> _queue;

	/////////////////segmentation manager////////////////////
	unique_ptr<Segmentation> Segment_part;
	vector<unique_ptr<MyITMNamespase::ITMManager>> ITMMng;

	// num of camer
	#define num_PC 1

	// input
	unique_ptr<InputManager> InputMng[num_PC];
	
	// human body manager
	vector<unique_ptr<HumanBody>> HumanBodies;;

	// camera position 
	Mat extrinsic_0to1, extrinsic_2to1;
	Mat extrinsicsD2D[3];
	vector<Eigen::Matrix4d> extrinsicsD2DEigen;
	
	Mat masked_depth, background_depth,curr_depth;
	//vector<vector<Vec3d>> PointClouds;
	vector<vector<Eigen::Vector4d>> PointClouds;
	vector<vector<Vec3b>> PointClouds_Color;
	Mat backGroundBuffer[num_PC], colorBuffer[num_PC];

	vector<Vec3d> skeletonVector3DCurrent;

	//Current Scene
	Scene scene;

	bool isComputed;

	vector<string> vecFolder;
	string folder,path;

	bool runITM = false; bool runBackground = false;


	//////////////// OpenCL memory ////////////////////
	cl_mem _depthCL;
	cl_mem _depthBuffCL; cl_mem colorCL;
	cl_mem _VMapCL;
	cl_mem _NMapCL;

	cl_mem _RGBMapCL;
	cl_mem _SegmentedCL;

	cl_mem _intrinsicCL;
	cl_mem _PoseCL;
	float _intrinsic[11];
	float _Pose[16];
	//////////////////////////////////////////////////

	////////////////KINECT Buffer etc///////////////////////////
	bool isKinect = true;
	KINECT cam;

};

