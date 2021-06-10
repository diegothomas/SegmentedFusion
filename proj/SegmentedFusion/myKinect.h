#pragma once

#include<iostream>
#include<fstream>
#include<sstream>
#include<math.h>
#include<direct.h>
#include<vector>
#include<time.h> // 一部のパソコンでインクルードしないと怒られたため(iostreamに入ってると思うんだけど)


//opencv 
#include<opencv2/core.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/imgcodecs.hpp>
//#include<opencv2/viz.hpp>
#include<opencv2/opencv.hpp>

//Kinect SDK
#include<Kinect.h>


//set datadir path
#include"DirectoryConfig.h"


#define BODYCOUNT 6

template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL) {
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}


class KINECT
{
public:
	KINECT();
	~KINECT();

	void initialize();
	void end();
	void update();
	void show();
	void bufferImg();
	void clearBuffer();
	void saveImgs(std::string folderpath);
	void showProfile();

	void getCurrentData(cv::Mat& _color, cv::Mat& _depth, std::vector<cv::Vec2i> &skeleton2d, std::vector<cv::Vec3d> &skeleton3d);
	
	cv::Mat depthImage;
	cv::Mat depthImage_forshow;
	cv::Mat colorImage;
	cv::Mat colorImage_forshow;
	std::vector<Vector4> skeletonJoints;
	std::vector<cv::Vec2i> skeletonJoints2d;

	std::vector<cv::Mat> colorBuffer;	//image matrix buffer
	std::vector<cv::Mat> depthBuffer;
	std::vector<std::vector<Vector4>> skeletonBuffer;

	std::string serialno;

	float color_resizescale = 2;

	BOOLEAN skeleton_tracked = false;
	BOOLEAN show_skeleton = true;

	// Intrinsics from https://www.facebook.com/1529875020577252/posts/kinect-v2-キャリブレーションcamera-calibration-toolbox-for-matlabによるkinect-v2の内部パラメータ外部パラ/1532859940278760/
	cv::Mat K_color = (cv::Mat_<double>(3, 3) << 1051.79688 / color_resizescale, 0.0, 981.68696 / color_resizescale, 0.0, 1048.55410 / color_resizescale, 544.68978 / color_resizescale, 0.0, 0.0, 1.0);
	cv::Mat D_color = (cv::Mat_<double>(1, 5) << 0.04229 / color_resizescale, -0.05348 / color_resizescale, -0.00024 / color_resizescale, 0.00335 / color_resizescale, 0.00000 / color_resizescale);
	cv::Mat K_depth = (cv::Mat_<double>(3, 3) << 364.37130, 0.0, 258.66617, 0.0, 362.97195, 210.25354, 0.0, 0.0, 1.0);
	cv::Mat D_depth = (cv::Mat_<double>(1, 5) << 0.05778, -0.16775, -0.00040, 0.00084, 0.00000);
	cv::Mat Rt_color2depth = (cv::Mat_<double>(4, 4) << 0.99997554, -0.00643662, -0.0027378, 41.05705, 0.00644326, 0.99997631, 0.00242119, 1.23825, 0.00272215, -0.00243877, 0.99999332, 1.41714, 0.0, 0.0, 0.0, 1.0);


private:
	IKinectSensor* pKinectSensor;
	ICoordinateMapper* pCoordinateMapper;

	IColorFrameReader* pColorFrameReader;
	UINT colorWidth;
	UINT colorHeight;
	UINT colorBufferSize;


	IDepthFrameReader* pDepthFrameReader;
	UINT depthWidth;
	UINT depthHeight;
	UINT depthBufferSize;

	IBodyFrameReader* pBodyFrameReader;



	std::vector<ColorSpacePoint> colorSpacePoints;
	std::vector<CameraSpacePoint> cameraSpacePoints;


};

