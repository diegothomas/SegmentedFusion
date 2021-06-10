#pragma once

#include "Header.h"
#include "Functions.h"

class InputManager
{
public:
	InputManager(string datafilename="");
	~InputManager();

	//void LoadData(int length, int firstFrame, string type, string cameraname);

	void LoadData(int length, int firstFrame = 0, string type = "camera_00");

	//void LoadData(int length = 1, string type = "tiff");
	bool GetImage_color(cv::Mat & out);
	bool GetImage_depth(cv::Mat & out);
	bool GetImages(cv::Mat & colorout, Mat & depthout);
	bool GetSkeletonVector(vector<Vec2> &out);

	bool GetSkeletonVector_kinect(vector<Vec2>& out);

	void DrawColorSkeleton();

	void StreamSkeletonVideo();

	void ComputeMappingFunction(Mat & depth, vector<Point>& output);

	bool GetCurrentSkeleton(vector<Vec2i>& output);
	bool GetCurrentSkeleton(vector<Eigen::Vector2i>& output);
	bool GetCurrentSkeleton(vector<Vec2i>& output, vector<Vec3d>& output3d);
	bool GetCurrentSkeleton(vector<Vec2>& output, vector<Vec3>& output3d);

	bool GetData(Mat & color, Mat & depth, vector<Eigen::Vector2i>& skeleton);
	bool GetData(Mat &color, Mat &depth, vector<Vec2i> &skeleton);
	bool GetData(Mat &color, Mat &depth, vector<Vec2i> &skeleton, vector<Vec3d> &skeleton_vec3);
	bool GetData(Mat &color, Mat &depth, vector<Vec2> &skeleton, vector<Vec3> &skeleton_vec3);

	int fileCount, depthImageCount, skeletonCount, skeletonCount_kinect;
	void ResetFilecount() { fileCount = 0; depthImageCount = 0; skeletonCount = 0; skeletonCount_kinect = 0; }


	////sample
	void sampleSolvePnP();

	void load_openpose_joints();

	Eigen::Matrix4d exColor2Depth;

private:
	vector<cv::Mat> RGBImages;
	vector<cv::Mat> DepthImages;
	vector<vector<Vec2f>> skeleton_vec;
	vector<vector<Vec2>> skeleton_vec_kinect;
	vector<vector<Vec3f>> skeleton_vec3;

	vector<vector<Point2i>> Mapping;

	string filename;
	string path;
	cv::Mat exColor2Depth_R, exColor2Depth_t;

	bool isHumanBody;
};

