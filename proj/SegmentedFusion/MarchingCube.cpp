#include "MarchingCube.h"



MarchingCube::MarchingCube()
{
}

MarchingCube::MarchingCube(Vec3 size, float * param, float iso, cl_context _context, cl_device_id _device)
{
	cl_int ret;




	kernel_MarcingCubes = LoadKernel(string(srcOpenCL + "MarchingCube.cl"), string("MarchingCubes"), _context, _device);
	ret = clSetKernelArg(kernel_MarcingCubes, 0, sizeof(TSDFCL), &TSDFCL);
	ret = clSetKernelArg(kernel_MarcingCubes, 1, sizeof(OffsetCL), &OffsetCL);
	ret = clSetKernelArg(kernel_MarcingCubes, 2, sizeof(IndexCL), &IndexCL);
	ret = clSetKernelArg(kernel_MarcingCubes, 3, sizeof(VerticesCL), &VerticesCL);
	ret = clSetKernelArg(kernel_MarcingCubes, 4, sizeof(FacesCL), &FacesCL);
	ret = clSetKernelArg(kernel_MarcingCubes, 5, sizeof(ParamCL), &ParamCL);
	ret = clSetKernelArg(kernel_MarcingCubes, 6, sizeof(Size_Vol), &Size_Vol);
	


	kernel_MarchingCubeIndexing = LoadKernel(string(srcOpenCL + "MarchingCube.cl"), string("MarchingCubesIndexing"), _context, _device);


}


MarchingCube::~MarchingCube()
{
}
