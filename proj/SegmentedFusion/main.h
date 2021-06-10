#pragma once

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <sstream>
#include <iterator>
#include <cmath>
#include <memory>
#include <direct.h>

#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
//#include <opencv2/core/eigen.hpp>

//Eigen
#include<Eigen\Sparse>
#include<Eigen\Geometry>


using namespace std;
using namespace cv;
//using namespace Eigen;

#define	THREAD_SIZE_X 8
#define THREAD_SIZE_Y 8
#define STRIDE 256
//#define NB_BS 49
#define NB_BS 28

#define TSDF_VOX_SIZE 64


/////////CL Kernel number//////////////////
#define VMAP_KER 0
#define NMAP_KER 1
#define BILATERAL_KER 2


////////////Body Part Number/////////////////////
#define BODY_PART_HEAD 0
#define BODY_PART_UPPER_ARM_R 1
#define BODY_PART_UPPER_ARM_L 2
#define BODY_PART_FOREARM_R 3
#define BODY_PART_FOREARM_L 4
#define BODY_PART_UP_LEG_R 5
#define BODY_PART_UP_LEG_L 6
#define BODY_PART_DOWN_LEG_R 7
#define BODY_PART_DOWN_LEG_L 8
#define BODY_PART_TORSOR 9

#define BODY_PART_HAND_R 10
#define BODY_PART_HAND_L 11
#define BODY_PART_FOOT_R 12
#define BODY_PART_FOOT_L 13

#define BODY_PART_NUMBER 14



///////////////Skeleton pos Number////////////////
#define SpineBase  0
#define SpineMid  1
#define Neck  2
#define Head  3
#define ShoulderLeft  4
#define ElbowLeft  5
#define WristLeft  6
#define HandLeft  7
#define ShoulderRight  8
#define ElbowRight  9
#define WristRight  10
#define HandRight  11
#define HipLeft  12
#define KneeLeft  13
#define AnkleLeft  14
#define FootLeft  15
#define HipRight  16
#define KneeRight  17
#define AnkleRight  18
#define FootRight  19
#define SpineShoulder  20
#define HandTipLeft  21
#define ThumbLeft  22
#define HandTipRight  23
#define ThumbRight  24

#define SkeletonNumber 21


// Result for BODY_25 (25 body parts consisting of COCO + foot)
const std::map<unsigned int, std::string> POSE_BODY_25_BODY_PARTS{
	{ 0,  "Nose" },
	{ 1,  "Neck" },
	{ 2,  "RShoulder" },
	{ 3,  "RElbow" },
	{ 4,  "RWrist" },
	{ 5,  "LShoulder" },
	{ 6,  "LElbow" },
	{ 7,  "LWrist" },
	{ 8,  "MidHip" },
	{ 9,  "RHip" },
	{ 10, "RKnee" },
	{ 11, "RAnkle" },
	{ 12, "LHip" },
	{ 13, "LKnee" },
	{ 14, "LAnkle" },
	{ 15, "REye" },
	{ 16, "LEye" },
	{ 17, "REar" },
	{ 18, "LEar" },
	{ 19, "LBigToe" },
	{ 20, "LSmallToe" },
	{ 21, "LHeel" },
	{ 22, "RBigToe" },
	{ 23, "RSmallToe" },
	{ 24, "RHeel" },
	{ 25, "Background" }
};


/////////////////color/////////////////////////////
#define COLOR_HEAD cv::Scalar(255, 0, 0)
#define COLOR_BODY cv::Scalar(255, 255, 255)

#define COLOR_UPPER_ARM_R cv::Scalar(200, 255, 200)
#define COLOR_UPPER_ARM_L cv::Scalar(200, 200, 255)
#define COLOR_FOREARM_R cv::Scalar(0, 255, 0)
#define COLOR_FOREARM_L cv::Scalar(0, 0, 255)
#define COLOR_HAND_R cv::Scalar(0, 191, 255)
#define COLOR_HAND_L cv::Scalar(0, 100, 0)

#define COLOR_THIGH_R cv::Scalar(255, 0, 255)
#define COLOR_THIGH_L cv::Scalar(255, 255, 0)
#define COLOR_CALF_R cv::Scalar(255, 180, 255)
#define COLOR_CALF_L cv::Scalar(255, 255, 180)
#define COLOR_FOOT_R cv::Scalar(191, 21, 133)
#define COLOR_FOOT_L cv::Scalar(255, 165, 0)

#define BUFFER_OFFSET(a) ((char*)NULL + (a))