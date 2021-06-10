#pragma once
#include "Header.h"
#include "Functions.h"


class Stitching
{
public:
	Stitching();
	Stitching(cv::Mat depth);
	~Stitching();


	//void GetVBonesTrans(vector<Vec2> skeVtx_cur, vector<Vec2> skeVtx_prev);
	void GetVBonesTrans(vector<Vec3> skeVtx_cur, vector<Vec3> skeVtx_prev);
	void GetVBonesTrans(vector<Vec3d> skeVtx_cur, vector<Vec3d> skeVtx_prev);
	
	void SetDepth(cv::Mat depth, cv::Mat VMap, bool init = false);
	void SetDepth(cv::Mat depth, bool init=false);

	void GetJointInfo(int bp, float resBone[8], float resJoint[8]);

	Eigen::Matrix4f getJointMatrix(int bodynum) { return boneTrans[bodynum]; }

private:
	Eigen::Matrix4f boneTrans[20];
	Eigen::Matrix4f boneSubTrans[20];

	Eigen::Matrix4f boneTransAll[20];
	Eigen::Matrix4f boneSubTransAll[20];

	Eigen::Matrix4f RTr, STr;
	cv::Mat depth, VMap, prev_Vmap;

	bool init[BODY_PART_NUMBER+4];

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

	//// swap left and right
	//int bonelist[num_oflist][2] = { { SpineShoulder,Head },{ ShoulderLeft,ElbowLeft },{ ShoulderRight,ElbowRight },
	//{ ElbowLeft,WristLeft },{ ElbowRight,WristRight },{ HipLeft,KneeLeft },{ HipRight,KneeRight },
	//{ KneeLeft,AnkleLeft },{ KneeRight,AnkleRight },{ SpineBase,SpineShoulder },
	//{ ElbowLeft,HandLeft },{ ElbowRight,HandRight },{ KneeLeft,FootLeft },{ KneeRight,FootRight },//0~13
	//{ SpineShoulder,ShoulderLeft },{ SpineShoulder,ShoulderRight },
	//{ SpineMid, HipLeft },{ SpineMid,HipRight },{ SpineBase, SpineShoulder } };//~18

	//int boneParent[num_oflist] = { BODY_PART_TORSOR,14,15,
	//	BODY_PART_UPPER_ARM_L,BODY_PART_UPPER_ARM_R,16,17,
	//	BODY_PART_UP_LEG_L,BODY_PART_UP_LEG_R,18,
	//	BODY_PART_FOREARM_L,BODY_PART_FOREARM_R,BODY_PART_DOWN_LEG_L,BODY_PART_DOWN_LEG_R,//~13
	//	18, 18,BODY_PART_TORSOR,BODY_PART_TORSOR , BODY_PART_TORSOR };

};


