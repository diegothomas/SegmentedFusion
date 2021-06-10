#pragma once

#include"Header.h"
#include "Functions.h"

class MarchingCube
{
public:
	MarchingCube();
	MarchingCube(Vec3 size, float *param, float iso, cl_context _context, cl_device_id _device);
	~MarchingCube();
private:
	cl_mem TSDFCL, OffsetCL, IndexCL;
	cl_mem Size_Vol, ParamCL, VerticesCL, FacesCL;
	cl_context context;
	cl_device_id device;

	cl_kernel kernel_MarchingCubeIndexing, kernel_MarcingCubes;

};

