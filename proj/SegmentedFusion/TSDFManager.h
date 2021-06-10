#pragma once
#include"Header.h"
#include "Functions.h"

class TSDFManager
{
public:
	TSDFManager();
	TSDFManager(cl_context _context, cl_device_id _device, Vec3 size, Vec3 Voxsize, float * intrinsic, float * pose, cl_mem depthCL, cl_mem CLcolor, cv::Mat depthimage, int bodynum);
	~TSDFManager();
	void Fuse_RGBD(cv::Mat, cv::Mat);
	void Fuse_RGBD_GPU(cv::Mat _depth, Mat _color, float bone[8], float joint[8], float plane[4]);
	void Fuse_RGBD_GPU(float bone[8], float joint[8], float plane[4]);
	void DrawMesh(Vec3);
	void SetAllPose(float pose[]);
	void VtxTransform(float pose[16]);
	void TSDF_Update(cv::Mat _depth, Mat _color, float bone[8], float joint[8], float plane[4]);
	void MeshGenerate();
	void SavePLY(string filename);
	//void MargeVtx();
	//void SetPose(float pose[]);
	//Vec3 GetGloPos(Vec3 pos);
	//Vec3 GetGloPos_Nmle(Vec3 pos);
private:
	int size_x, size_y, size_z;
	int c_x, c_y, c_z;
	float dim_x, dim_y, dim_z;
	float intrinsic[11];
	int sizes[3];
	int roop = 0;
	int stride;
	int tsdf_num;
	int myBodynum;

	//cv::Mat Transform;
	cv::Mat depth,color;
	//cv::Mat masked_depth;
	//cv::Mat maskImage;

	float _intrinsic[11];
	float _Pose[16], AllPose[16], _PoseD2C[16];
	float boneDQ[8], jointDQ[8], Param[6], planeF[4];

	Eigen::Quaternionf boneReal, boneDual, jointReal, jointDual;

	float *_TSDF = NULL;
	float *_Weight = NULL;
	float *VtxNmle = NULL;
	//int  *Offset = NULL;
	//int *Index = NULL;
	//int *Faces = NULL;
	//float *Vertices = NULL;
	//float *Normales = NULL;
	//float *FacesIdx = NULL;
	//float * trVertices = NULL;
	//float *trNormales = NULL;

	float iso = NULL;;
	int FacesCounter[1];


	/////////OpenCL Memory//////////
	cl_mem _depthCL, _jointsCL, _DQCL, _WeightCL, maskCL;
	cl_mem _TSDFCL;
	cl_mem ParamCL, boneDQCL, jointDQCL, planeFCL, calibCL;
	cl_mem dimCL;
	cl_mem colorCL;

	cl_mem _intrinsicCL, _PoseCL, _AllPoseCL, _PoseD2CCL;

	cl_device_id device;
	cl_context context;
	cl_command_queue queue_TSDF, queue_Test, queue_MC, queue_MCIndexing, queue_Transform, queue_Normal;


	cl_kernel kernel_TSDF, kernel_Test,kernel_Transform;

	//about MC
	cl_mem OffsetCL, IndexCL;
	cl_mem Size_Vol, VerticesCL, FacesCL, FacesCounterCL, NormalesCL;

	cl_kernel kernel_MarchingCubeIndexing, kernel_MarchingCubes, kernel_Normal , kernel_MarchingCubes_;
	cl_mem TrVtxCL, VboCL;

	cl_int ret;
	size_t origin[3] = { 0 ,0,0 };
	size_t region[3] = { size_t(cDepthWidth), size_t(cDepthHeight), size_t(1) };

	//GL allocate
	GLuint VBO_buf, FaceBuf,NmleBuf, VtxBuf;

};

