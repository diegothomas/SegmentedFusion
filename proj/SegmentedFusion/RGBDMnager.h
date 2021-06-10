#pragma once

#include "Header.h"
//#include "Segment.h"
#include "BodyPart.h"
#include "Stitching.h"
#include "Functions.h"
#include "InputManager.h"
#include "Segmentatnion.h"

class RGBDMnager
{
public:
	cl_context _context;
	cl_device_id _device;
	std::vector<cl_kernel> _kernels;
	std::vector<cl_command_queue> _queue;
	int _idx_curr;

	bool _Kinect1;
	bool _Kinect2;
	int _KinectVersion;
	bool _landmarkOK;

	cv::Mat _depth_in[2];
	cv::Mat _color_in[2];
	int _idx_thread[2];

	void SetParam(float *Calib, char *path, int KinectVersion = 0);

	inline bool Push() {
		if (_depth.size() == FRAME_BUFFER_SIZE) {
			return false;
		}
		_depth.push(_imgD);
		_color.push(_imgC);
		_segmented_color.push(_imgS);
		//depth_curr = _imgD;
		return true;
	}

	RGBDMnager(cl_context context, cl_device_id device);
	~RGBDMnager();

	void Init();

	// Load current depth & skeleton
	int Load();
	// Draw Manager
	void Draw();
	//manage compute?
	int Running();


	// Draw depth 
	void DrawDepth();
	// Draw secound screen
	void Draw2ndScreen();
	// Draw skeleton point
	void DrawSkeleton();
	// compute 3D map
	bool Compute3D();
	// Create croped body depth image
	void CropBody(cv::Mat &);

	void SavePly();

	//PCA function
	double getOrientation(const vector<Vec3> &pts, cv::Mat &img);

	void PCA();
	Vec3 RGBDMnager::pt23D(Vec2 pt);
	vector<Vec3d> GetVecPt(int num);

	void Reset();
	void getBackImages(cv::Mat & depth, cv::Mat & color);

	void getImages(cv::Mat &depth, cv::Mat & color);

private:
	// some flags
	int _idx;
	char *_path;

	int _max_iter[3];
	int _max_iterPR[3];

	bool isHumanBody;

	// 3D input data
	cv::Mat _VMap;
	cv::Mat _NMap;

	cv::Mat _imgD;
	cv::Mat _imgC;
	cv::Mat _imgS;


	cv::Mat depth_curr;
	cv::Mat masked_depth, background_depth;
	cv::Mat color_curr;

	//////////////// OpenCL memory ////////////////////
	cl_mem _depthCL; cl_mem colorCL;
	cl_mem _depthBuffCL;
	cl_mem _VMapCL;
	cl_mem _NMapCL;
	
	cl_mem _RGBMapCL;
	cl_mem _SegmentedCL;
	
	cl_mem _intrinsicCL;
	cl_mem _PoseCL;

	//////////////////////////////////////////////////

	static const int		FIRST_FRAME_NUMBER = 0;
	static const int		NUMBER_OF_FRAMES = 90;


	float _intrinsic[11];
	float _Pose[16];
	float *_Qinv;
	int _nbMatches;


	// RGB-D data
	queue<cv::Mat> _depth;
	queue<cv::Mat> _color;
	queue<cv::Mat> _segmented_color;

	cv::Mat _LabelsMask;


	Eigen::Vector3f _Translation;
	Eigen::Matrix3f _Rotation;

	Eigen::Vector3f _Translation_inv;
	Eigen::Matrix3f _Rotation_inv;

	vector<Eigen::Vector3f> _TranslationWindow;
	vector<Eigen::Matrix3f> _RotationWindow;
	vector<float> _BSCoeff[NB_BS];


	//skeleton 2D-pos
	vector<pair<float, float>> skeleton_pos, prev_skeleton_pos;
	vector<pair<float, float>> cropped_skeleton_pos;

	vector<Vec2> skeleton_vec, prev_skeleton_vec;
	vector<Vec3> skeletonVec3, prev_skeletonVec3;

	//segmentation manager
	unique_ptr<Stitching> StitchBody;
	unique_ptr<InputManager> Input;
	unique_ptr<Segmentation> Segment_part;

	//crop rect
	Vec2 cropleft;
	Vec2 cropright;
	cv::Mat Cropped_Box,Mask_BG;

	//Vec3 planesF[BODY_PART_NUMBER];

	float planesF[BODY_PART_NUMBER][4];

	BodyPart* BodyPartManager[BODY_PART_NUMBER];
	cv::Point3d corners[8];

	bool checkBodyPart[BODY_PART_NUMBER];
	Mat maskImages[BODY_PART_NUMBER];

 };

