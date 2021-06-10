#pragma once
#include "Header.h"
#include "Stitching.h"
#include "BodyPart.h"
#include "Segmentatnion.h"

class HumanBody
{
public:
	HumanBody();
	HumanBody(cl_context _context, cl_device_id _device, cl_mem depthCL,cl_mem CLColor);
	~HumanBody();

	void SetParams(Mat &depth, vector<Vec2i> &_skeleton_vec2, vector<Vec3d> &_skeleton_vec3, Mat colorMat);
	void SetParams(Segmentation &seg,vector<Vec2i>& _skeleton_vec2, vector<Vec3d>& _skeleton_vec3, Mat colorMat);
	void SetPosParams(Eigen::Matrix4d position, Eigen::Matrix4d D2C, int c = 0);
	void Running();

	void Draw();

	bool Init(int i);

	void DrawSkeleton();
	void DrawSkeleton(vector<Vec3d> skeleton);
	void InitilizeHumanBody();

	bool isHumanBody = false;
	bool isInitialized = false;

private:

	// my body data
	Mat human_depth,raw_depth,color;
	vector<Vec2i> skeleton_vec2,skeleton_vec2_prev;
	vector<Vec3d> skeleton_vec3,skeleton_vec3_prev, skeleton_vec3_world, skeleton_vec3_world_prev;
	bool isBodyPart[BODY_PART_NUMBER], existBodyPart[BODY_PART_NUMBER];
	Mat BodyPartMask[BODY_PART_NUMBER];
	Eigen::Matrix4d myBodyPosition, PoseD2C;// camera position

	// OpenCL context etc
	cl_context context;
	cl_device_id device;
	cl_mem depthCL,colorCL;

	// functions
	unique_ptr<Stitching> StitchBody;
	unique_ptr<BodyPart> BodyParts[BODY_PART_NUMBER];

	int cameranumber;

	inline Vec3d getWorldPosition(Vec3d p) {
		auto tmp = myBodyPosition * Eigen::Vector4d(p(0), p(0), p(0), 1.0);
		return Vec3d(tmp(0), tmp(1), tmp(2));
	}
};

