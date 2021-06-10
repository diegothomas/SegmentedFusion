#include "TSDFManager.h"



TSDFManager::TSDFManager()
{
	size_x = 1;
	size_y = 1;
	size_z = 1;

	c_x = size_x / 2;
	c_y = size_y / 2;
	c_z = size_z / 2;

	dim_x = 1.0;
	dim_y = 1.0;
	dim_z = 1.0;
}



TSDFManager::TSDFManager(cl_context _context, cl_device_id _device, Vec3 size, Vec3 Voxsize, float *intrinsic, float *pose, cl_mem depthCL, cl_mem CLcolor, cv::Mat depthimage, int bodynum)
{
	cl_int ret;	
	// resolution
	float VoxelSize = 0.005;

	size_x = size.x;
	size_y = size.y;
	size_z = size.z;
	c_x = size_x / 2;
	c_y = size_y / 2;
	c_z = size_z / 2;
	dim_x = size_x / Voxsize.x;
	dim_y = size_y / Voxsize.y;
	dim_z = size_z / Voxsize.z;
	context = _context;
	device = _device;

	queue_MC = clCreateCommandQueue(_context, device, 0, &ret);
	checkErr(ret, "CommandQueue::MarchingCubes");
	queue_TSDF= clCreateCommandQueue(_context, _device, 0, &ret);
	checkErr(ret, "CommandQueue::CommandQueue()");
	queue_Transform = clCreateCommandQueue(context, device, 0, &ret);
	checkErr(ret, "CommandQueue::Transform()");

	sizes[0] = size_x;
	sizes[1] = size_y;
	sizes[2] = size_z;

	myBodynum = bodynum;
	///////////arrocate array//////////////////
	int sizes_vox[3] = { size_x ,size_y ,size_z };
	int a = sizes_vox[0]* sizes_vox[1] * sizes_vox[2];
	tsdf_num = a;
	stride = tsdf_num * 9;
	_TSDF = new float[tsdf_num];
	_Weight = new float[tsdf_num];

	// initial value of opencl kernel
	// Param = [c_x, dim_x, c_y, dim_y, c_z, dim_z]
	Param[0] = c_x;
	Param[1] = dim_x;
	Param[2] = c_y;
	Param[3] = dim_y;
	Param[4] = c_z;
	Param[5] = dim_z;

	//Pose
	for (int i = 0; i < 16; i++)
	{
		_Pose[i] = pose[i];
		AllPose[i] = pose[i];
		_PoseD2C[i] = 0.0;
	}
	_PoseD2C[0] = 1.0;
	_PoseD2C[5] = 1.0;
	_PoseD2C[10] = 1.0;
	_PoseD2C[15] = 1.0;

	for (int i = 0; i < tsdf_num; i++)
	{
		_TSDF[i] = 1.0;
		_Weight[i] = 0.0;
	}

	planeF[0] = 1.0;
	planeF[1] = 1.0;
	planeF[2] = 1.0;
	planeF[3] = 1.0;

	boneDQ[0] = 1.0;
	boneDQ[1] = 0.0;
	boneDQ[2] = 0.0;
	boneDQ[3] = 0.0;

	boneDQ[4] = 0.0;
	boneDQ[5] = 0.0;
	boneDQ[6] = 0.0;
	boneDQ[7] = 0.0;

	jointDQ[0] = 1.0;
	jointDQ[1] = 0.0;
	jointDQ[2] = 0.0;
	jointDQ[3] = 0.0;

	jointDQ[4] = 0.0;
	jointDQ[5] = 0.0;
	jointDQ[6] = 0.0;
	jointDQ[7] = 0.0;
	
	//masked_depth = Mat::zeros(cDepthHeight, cDepthWidth, CV_32FC4);
	depth = Mat(cDepthHeight, cDepthWidth, CV_16UC4);
	//color = Mat(cDepthHeight, cDepthWidth, CV_32FC4);
	color =	Mat(cDepthHeight, cDepthWidth, CV_8UC4);

	///////////OpenCL memoory/////////////	
	cl_image_format format = { CL_RGBA, CL_UNSIGNED_INT16 };
	cl_image_format format2 = { CL_RGBA, CL_UNSIGNED_INT8 };
	cl_image_format format3 = { CL_RGBA, CL_FLOAT };
	cl_image_desc  desc;
	desc.image_type = CL_MEM_OBJECT_IMAGE2D;
	desc.image_width = cDepthWidth;
	desc.image_height = cDepthHeight;
	desc.image_depth = 0;
	desc.image_array_size = 0;
	desc.image_row_pitch = 0;
	desc.image_slice_pitch = 0;
	desc.num_mip_levels = 0;
	desc.num_samples = 0;
	desc.buffer = NULL;

	iso = 0.0;
	FacesCounter[0] = 0;

	glGenBuffers(1, &VBO_buf);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_buf);
	glBufferData(GL_ARRAY_BUFFER, 3 * stride * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	VboCL = clCreateFromGLBuffer(_context, CL_MEM_READ_WRITE, VBO_buf, &ret);
	checkErr(ret, "binding to OpenCL face");

	glGenBuffers(1, &FaceBuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FaceBuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, stride * sizeof(int), NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	FacesCL = clCreateFromGLBuffer(_context, CL_MEM_READ_WRITE, FaceBuf, &ret);
	checkErr(ret, "binding to OpenCL face");
	
	_depthCL = depthCL;
	colorCL = CLcolor;

	_TSDFCL = clCreateBuffer(_context, CL_MEM_READ_WRITE, tsdf_num * sizeof(float),_TSDF,&ret);
	checkErr(ret, "_TSDFCL::Buffer()");
	_WeightCL = clCreateBuffer(_context, CL_MEM_READ_WRITE, tsdf_num * sizeof(float), _Weight, &ret);
	checkErr(ret, "weight::Buffer()");

	_intrinsicCL = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 11 * sizeof(float), _intrinsic, &ret);
	checkErr(ret, "_intrinsics::Buffer()");
	_PoseCL = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 16 * sizeof(float), _Pose, &ret);
	checkErr(ret, "_PoseCL::Buffer()");
	ParamCL = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 6*sizeof(float), Param, &ret);
	checkErr(ret, "paramCL::Buffer()");
	boneDQCL = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 8 * sizeof(float), boneDQ, &ret);
	checkErr(ret, "boneCL::Buffer()");
	jointDQCL = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 8 * sizeof(float), jointDQ, &ret);
	checkErr(ret, "jointCL::Buffer()");
	planeFCL = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 4 * sizeof(float), planeF, &ret);
	checkErr(ret, "planeCL::Buffer()");
	calibCL = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 11* sizeof(float), Calib, &ret);
	checkErr(ret, "calibCL::Buffer()");
	FacesCounterCL = clCreateBuffer(_context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(int), &FacesCounter, &ret);
	checkErr(ret, "FacesCounterCL::Buffer()");
	dimCL = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 3 * sizeof(int), sizes, &ret);

	_PoseD2CCL = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 16 * sizeof(float), _PoseD2C, &ret);
	checkErr(ret, "_PoseCL::Buffer()");

	kernel_TSDF = LoadKernel(string(srcOpenCL + "TSDF.cl"), string("FuseTSDF"), _context, _device);
	ret = clSetKernelArg(kernel_TSDF, 0, sizeof(_TSDFCL), &_TSDFCL);
	ret = clSetKernelArg(kernel_TSDF, 1, sizeof(_WeightCL), &_WeightCL);
	ret = clSetKernelArg(kernel_TSDF, 2, sizeof(ParamCL), &ParamCL);
	ret = clSetKernelArg(kernel_TSDF, 3, sizeof(dimCL), &dimCL);
	ret = clSetKernelArg(kernel_TSDF, 4, sizeof(_PoseCL), &_PoseCL);
	ret = clSetKernelArg(kernel_TSDF, 5, sizeof(boneDQCL), &boneDQCL);
	ret = clSetKernelArg(kernel_TSDF, 6, sizeof(jointDQCL), &jointDQCL);
	ret = clSetKernelArg(kernel_TSDF, 7, sizeof(planeFCL), &planeFCL);
	ret = clSetKernelArg(kernel_TSDF, 8, sizeof(calibCL), &calibCL);
	ret = clSetKernelArg(kernel_TSDF, 9, sizeof(cDepthHeight), &cDepthHeight);
	ret = clSetKernelArg(kernel_TSDF, 10, sizeof(cDepthWidth), &cDepthWidth);
	ret = clSetKernelArg(kernel_TSDF, 11, sizeof(_depthCL), &_depthCL);


	kernel_Transform = LoadKernel(string(srcOpenCL + "TSDF.cl"), string("Transform"), _context, _device);
	ret = clSetKernelArg(kernel_Transform, 0, sizeof(VboCL), &VboCL);
	ret = clSetKernelArg(kernel_Transform, 1, sizeof(_PoseCL), &_PoseCL);
	ret = clSetKernelArg(kernel_Transform, 2, sizeof(boneDQCL), &boneDQCL);
	ret = clSetKernelArg(kernel_Transform, 3, sizeof(jointDQCL), &jointDQCL);
	ret = clSetKernelArg(kernel_Transform, 4, sizeof(planeFCL), &planeFCL);
	ret = clSetKernelArg(kernel_Transform, 5, sizeof(dimCL), &dimCL);
	ret = clSetKernelArg(kernel_Transform, 6, sizeof(cl_mem), &FacesCounterCL);


	kernel_Test = LoadKernel(string(srcOpenCL + "TSDF.cl"), string("Transform_color"), _context, _device);
	ret = clSetKernelArg(kernel_Test, 0, sizeof(VboCL), &VboCL);
	ret = clSetKernelArg(kernel_Test, 1, sizeof(_PoseCL), &_PoseCL);
	ret = clSetKernelArg(kernel_Test, 2, sizeof(boneDQCL), &boneDQCL);
	ret = clSetKernelArg(kernel_Test, 3, sizeof(jointDQCL), &jointDQCL);
	ret = clSetKernelArg(kernel_Test, 4, sizeof(planeFCL), &planeFCL);
	ret = clSetKernelArg(kernel_Test, 5, sizeof(dimCL), &dimCL);
	ret = clSetKernelArg(kernel_Test, 6, sizeof(cl_mem), &FacesCounterCL);
	ret = clSetKernelArg(kernel_Test, 7, sizeof(colorCL), &colorCL);
	ret = clSetKernelArg(kernel_Test, 8, sizeof(_PoseD2CCL), &_PoseD2CCL);



	kernel_MarchingCubes = LoadKernel(string(srcOpenCL + "MarchingCube.cl"), string("MarchingCubes_oneFunc"), _context, _device);
	ret = clSetKernelArg(kernel_MarchingCubes, 0, sizeof(_TSDFCL), &_TSDFCL);
	ret = clSetKernelArg(kernel_MarchingCubes, 1, sizeof(FacesCL), &FacesCL);
	ret = clSetKernelArg(kernel_MarchingCubes, 2, sizeof(ParamCL), &ParamCL);
	ret = clSetKernelArg(kernel_MarchingCubes, 3, sizeof(FacesCounterCL), &FacesCounterCL);
	ret = clSetKernelArg(kernel_MarchingCubes, 4, sizeof(iso), &iso);
	ret = clSetKernelArg(kernel_MarchingCubes, 5, sizeof(dimCL), &dimCL);
	ret = clSetKernelArg(kernel_MarchingCubes, 6, sizeof(VboCL), &VboCL);



	//kernel_MarchingCubeIndexing = LoadKernel(string(srcOpenCL + "MarchingCube.cl"), string("MarchingCubesIndexing"), _context, _device);
	//ret = clSetKernelArg(kernel_MarchingCubeIndexing, 0, sizeof(_TSDFCL), &_TSDFCL);
	//ret = clSetKernelArg(kernel_MarchingCubeIndexing, 1, sizeof(OffsetCL), &OffsetCL);
	//ret = clSetKernelArg(kernel_MarchingCubeIndexing, 2, sizeof(IndexCL), &IndexCL);
	//ret = clSetKernelArg(kernel_MarchingCubeIndexing, 3, sizeof(dimCL), &dimCL);
	//ret = clSetKernelArg(kernel_MarchingCubeIndexing, 4, sizeof(iso), &iso);
	//ret = clSetKernelArg(kernel_MarchingCubeIndexing, 5, sizeof(FacesCounterCL), &FacesCounterCL);
	//kernel_MarchingCubes_ = LoadKernel(string(srcOpenCL + "MarchingCube.cl"), string("MarchingCubes_normal"), _context, _device);
	//ret = clSetKernelArg(kernel_MarchingCubes_, 0, sizeof(_TSDFCL), &_TSDFCL);
	//ret = clSetKernelArg(kernel_MarchingCubes_, 1, sizeof(OffsetCL), &OffsetCL);
	//ret = clSetKernelArg(kernel_MarchingCubes_, 2, sizeof(IndexCL), &IndexCL);
	//ret = clSetKernelArg(kernel_MarchingCubes_, 3, sizeof(VboCL), &VboCL);
	//ret = clSetKernelArg(kernel_MarchingCubes_, 4, sizeof(FacesCL), &FacesCL);
	//ret = clSetKernelArg(kernel_MarchingCubes_, 5, sizeof(ParamCL), &ParamCL);
	//ret = clSetKernelArg(kernel_MarchingCubes_, 6, sizeof(dimCL), &dimCL);
}


TSDFManager::~TSDFManager()
{
	if (_TSDF != NULL)delete[] _TSDF;
	if (_Weight != NULL)delete[] _Weight;

	//if(Offset != NULL)delete[] Offset;
	//if (Index != NULL)delete[] Index;
	//if (Faces != NULL)delete[] Faces;
	//if (Vertices != NULL)delete[] Vertices;
	//if (Normales != NULL)delete[] Normales;


	clReleaseMemObject(_TSDFCL);
	clReleaseMemObject(FacesCL);
	clReleaseMemObject(_WeightCL);
	clReleaseMemObject(VboCL);
	//clReleaseMemObject(colorCL);
	//clReleaseMemObject(_depthCL);
	//clReleaseMemObject(OffsetCL);
	//clReleaseMemObject(IndexCL);
}



void TSDFManager::Fuse_RGBD_GPU(cv::Mat _depth, Mat _color,float bone[8], float joint[8], float plane[4])
{
	for (int i = 0; i < cDepthHeight; i++) {
		for (int j = 0; j < cDepthWidth; j++) {
			depth.at<cv::Vec4w>(i, j)[0] = _depth.at<unsigned short>(i, j);
			depth.at<cv::Vec4w>(i, j)[1] = _depth.at<unsigned short>(i, j);
			depth.at<cv::Vec4w>(i, j)[2] = _depth.at<unsigned short>(i, j);
			depth.at<cv::Vec4w>(i, j)[3] = 0;

			color.at<cv::Vec4b>(i, j)[0] = _color.at<cv::Vec3b>(i, j)[0];
			color.at<cv::Vec4b>(i, j)[1] = _color.at<cv::Vec3b>(i, j)[1];
			color.at<cv::Vec4b>(i, j)[2] = _color.at<cv::Vec3b>(i, j)[2];
			color.at<cv::Vec4b>(i, j)[3] = 1;
		}
	}

	ret = clEnqueueWriteImage(queue_MC, _depthCL, false, origin, region, cDepthWidth * 4 * sizeof(unsigned short), 0, depth.data, 0, NULL, NULL);
	checkErr(ret, "Unable to write input");
	ret = clEnqueueWriteImage(queue_MC, colorCL, false, origin, region, cDepthWidth * 4 * sizeof(unsigned char), 0, color.data, 0, NULL, NULL);
	checkErr(ret, "Unable to write input");
	Fuse_RGBD_GPU(bone, joint, plane);
}


void TSDFManager::Fuse_RGBD_GPU(float bone[8], float joint[8], float plane[4])
{
	// Compute Vertex map //change the value
	int gws_x = divUp(size_x, THREAD_SIZE_X);
	int gws_y = divUp(size_y, THREAD_SIZE_Y);
	size_t gws[2] = { gws_x*THREAD_SIZE_X, gws_y*THREAD_SIZE_Y };
	size_t lws[2] = { THREAD_SIZE_X, THREAD_SIZE_Y };
	
	// GL buffer acquire
	clEnqueueAcquireGLObjects(queue_MC, 1, &FacesCL, 0, NULL, 0);
	clEnqueueAcquireGLObjects(queue_MC, 1, &VboCL, 0, NULL, 0);



	if (roop >= 0) {
		for (int i = 0; i < 8; i++)
		{
			boneDQ[i] = bone[i];
			jointDQ[i] = joint[i];
		}
	}
	if (roop == 0) {
		planeF[0] = plane[0];
		planeF[1] = plane[1];
		planeF[2] = plane[2];
		planeF[3] = plane[3];
	}

	ret = clEnqueueWriteBuffer(queue_MC, boneDQCL, CL_TRUE, 0, 8 * sizeof(float), boneDQ, 0, NULL, NULL);
	checkErr(ret, "Unable to write input bone");
	ret = clEnqueueWriteBuffer(queue_MC, jointDQCL, CL_TRUE, 0, 8 * sizeof(float), jointDQ, 0, NULL, NULL);
	checkErr(ret, "Unable to write input joint");
	if (roop >= 0) {
		//TSDF Fuse
		cl_event evtVMap[3];
		ret = clEnqueueNDRangeKernel(queue_MC, kernel_TSDF, 2, NULL, gws, NULL, 0, NULL, &evtVMap[0]);
		checkErr(ret, "EnqueKernelError::kernel_TSDF()");
		clFinish(queue_MC);
	}
	roop++;

	//MarchingCube
	FacesCounter[0] = 0;
	ret = clEnqueueWriteBuffer(queue_MC, FacesCounterCL, CL_TRUE, 0, sizeof(int), &FacesCounter, 0, NULL, NULL);
	checkErr(ret, "Unable to write input FacenumMC");

	// resize
	gws_x = divUp(size_x*size_y, THREAD_SIZE_X);
	gws_y = divUp(size_z, THREAD_SIZE_Y);
	gws[0] = gws_x*THREAD_SIZE_X;
	gws[1] = gws_y * THREAD_SIZE_Y;

	// TSDF -> Vtx
	ret = clEnqueueNDRangeKernel(queue_MC, kernel_MarchingCubes, 2, NULL, gws, NULL, 0, NULL, NULL);
	//ret = clEnqueueNDRangeKernel(queue_MC, kernel_MarchingCubes_, 2, NULL, gws, NULL, 0, NULL, NULL);
	checkErr(ret, "EnqueKernelError::kernel_MC()");
	ret = clEnqueueReadBuffer(queue_MC, FacesCounterCL, CL_TRUE, 0, sizeof(int), &FacesCounter, 0, NULL, NULL);
	checkErr(ret, "Unable to read output Facenum");
	
	// compute transformed Vtx
	// 
	ret = clEnqueueNDRangeKernel(queue_MC, kernel_Test, 2, NULL, gws, NULL, 0, NULL, NULL);
	checkErr(ret, "EnqueKernelError::Transform()");
	clFinish(queue_MC);
	clEnqueueReleaseGLObjects(queue_MC, 1, &FacesCL, 0, NULL, 0);
	clEnqueueReleaseGLObjects(queue_MC, 1, &VboCL, 0, NULL, 0);
}




void TSDFManager::VtxTransform(float pose[16])
{
	cl_int ret;
	int gws_x = divUp(size_x*size_y, THREAD_SIZE_X);
	int gws_y = divUp(size_z, THREAD_SIZE_Y);
	size_t gws[2] = { gws_x*THREAD_SIZE_X, gws_y*THREAD_SIZE_Y };

	// GL buffer acquire
	clEnqueueAcquireGLObjects(queue_MC, 1, &FacesCL, 0, NULL, 0);
	clEnqueueAcquireGLObjects(queue_MC, 1, &VboCL, 0, NULL, 0);

	// Set transfoamtion matrix
	ret = clEnqueueWriteBuffer(queue_MC, _PoseCL, CL_TRUE, 0, 16 * sizeof(float), pose, 0, NULL, NULL);
	checkErr(ret, "Unable to write input bone");

	// There is no transforrmation
	float bone[8] = {
		1.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0
	};
	float joint[8] = {
		1.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0
	};

	ret = clEnqueueWriteBuffer(queue_MC, boneDQCL, CL_TRUE, 0, 8 * sizeof(float), bone, 0, NULL, NULL);
	checkErr(ret, "Unable to write input bone");
	ret = clEnqueueWriteBuffer(queue_MC, jointDQCL, CL_TRUE, 0, 8 * sizeof(float), joint, 0, NULL, NULL);
	checkErr(ret, "Unable to write input joint");

	// compute transformed Vtx
	ret = clEnqueueNDRangeKernel(queue_MC, kernel_Transform, 2, NULL, gws, NULL, 0, NULL, NULL);
	checkErr(ret, "EnqueKernelError::Transform()");
	clFinish(queue_MC);

	clEnqueueReleaseGLObjects(queue_MC, 1, &FacesCL, 0, NULL, 0);
	clEnqueueReleaseGLObjects(queue_MC, 1, &VboCL, 0, NULL, 0);

	// Reset transfoamtion matrix
	ret = clEnqueueWriteBuffer(queue_Transform, _PoseCL, CL_TRUE, 0, 16 * sizeof(float), _Pose, 0, NULL, NULL);
	checkErr(ret, "Unable to write input bone");
}

void TSDFManager::TSDF_Update(cv::Mat _depth, Mat _color, float bone[8], float joint[8], float plane[4])
{
	//imshow("input to TSDF"+to_string(myBodynum), _depth * 10);
	//waitKey(1);
	// Set Input parameters
	for (int i = 0; i < cDepthHeight; i++) {
		for (int j = 0; j < cDepthWidth; j++) {
			depth.at<cv::Vec4w>(i, j)[0] = _depth.at<unsigned short>(i, j);
			depth.at<cv::Vec4w>(i, j)[1] = _depth.at<unsigned short>(i, j);
			depth.at<cv::Vec4w>(i, j)[2] = _depth.at<unsigned short>(i, j);
			depth.at<cv::Vec4w>(i, j)[3] = 0;

			color.at<cv::Vec4b>(i, j)[0] = _color.at<cv::Vec3b>(i, j)[0];
			color.at<cv::Vec4b>(i, j)[1] = _color.at<cv::Vec3b>(i, j)[1];
			color.at<cv::Vec4b>(i, j)[2] = _color.at<cv::Vec3b>(i, j)[2];
			color.at<cv::Vec4b>(i, j)[3] = 1;
		}
	}
	for (int i = 0; i < 8; i++)
	{
		boneDQ[i] = bone[i];
		jointDQ[i] = joint[i];
	}	
	if (roop == 0) {
		planeF[0] = plane[0];
		planeF[1] = plane[1];
		planeF[2] = plane[2];
		planeF[3] = plane[3];
	}

	// set parameter to OpenCL Memory
	ret = clEnqueueWriteImage(queue_MC, _depthCL, false, origin, region, cDepthWidth * 4 * sizeof(unsigned short), 0, depth.data, 0, NULL, NULL);
	checkErr(ret, "Unable to write input");
	ret = clEnqueueWriteImage(queue_MC, colorCL, false, origin, region, cDepthWidth * 4 * sizeof(unsigned char), 0, color.data, 0, NULL, NULL);
	checkErr(ret, "Unable to write input");
	ret = clEnqueueWriteBuffer(queue_MC, boneDQCL, CL_TRUE, 0, 8 * sizeof(float), boneDQ, 0, NULL, NULL);
	checkErr(ret, "Unable to write input bone");
	ret = clEnqueueWriteBuffer(queue_MC, jointDQCL, CL_TRUE, 0, 8 * sizeof(float), jointDQ, 0, NULL, NULL);
	checkErr(ret, "Unable to write input joint");

	// Compute Vertex map //change the value
	int gws_x = divUp(size_x, THREAD_SIZE_X);
	int gws_y = divUp(size_y, THREAD_SIZE_Y);
	size_t gws[2] = { gws_x*THREAD_SIZE_X, gws_y*THREAD_SIZE_Y };
	size_t lws[2] = { THREAD_SIZE_X, THREAD_SIZE_Y };

	// GL buffer acquire
	clEnqueueAcquireGLObjects(queue_MC, 1, &FacesCL, 0, NULL, 0);
	clEnqueueAcquireGLObjects(queue_MC, 1, &VboCL, 0, NULL, 0);

	if (roop >= 0) {
		//TSDF Fuse
		cl_event evtVMap[3];
		ret = clEnqueueNDRangeKernel(queue_MC, kernel_TSDF, 2, NULL, gws, NULL, 0, NULL, &evtVMap[0]);
		checkErr(ret, "EnqueKernelError::kernel_TSDF()");
		clFinish(queue_MC);
	}
	roop++;

	clFinish(queue_MC);
	clEnqueueReleaseGLObjects(queue_MC, 1, &FacesCL, 0, NULL, 0);
	clEnqueueReleaseGLObjects(queue_MC, 1, &VboCL, 0, NULL, 0);
}

void TSDFManager::MeshGenerate()
{
	// Compute Vertex map //change the value
	int gws_x = divUp(size_x, THREAD_SIZE_X);
	int gws_y = divUp(size_y, THREAD_SIZE_Y);
	size_t gws[2] = { gws_x*THREAD_SIZE_X, gws_y*THREAD_SIZE_Y };
	size_t lws[2] = { THREAD_SIZE_X, THREAD_SIZE_Y };

	// GL buffer acquire
	clEnqueueAcquireGLObjects(queue_MC, 1, &FacesCL, 0, NULL, 0);
	clEnqueueAcquireGLObjects(queue_MC, 1, &VboCL, 0, NULL, 0);

	//MarchingCube
	FacesCounter[0] = 0;
	ret = clEnqueueWriteBuffer(queue_MC, FacesCounterCL, CL_TRUE, 0, sizeof(int), &FacesCounter, 0, NULL, NULL);
	checkErr(ret, "Unable to write input FacenumMC");

	// resize
	gws_x = divUp(size_x*size_y, THREAD_SIZE_X);
	gws_y = divUp(size_z, THREAD_SIZE_Y);
	gws[0] = gws_x*THREAD_SIZE_X;
	gws[1] = gws_y * THREAD_SIZE_Y;

	// TSDF -> Vtx
	ret = clEnqueueNDRangeKernel(queue_MC, kernel_MarchingCubes, 2, NULL, gws, NULL, 0, NULL, NULL);
	//ret = clEnqueueNDRangeKernel(queue_MC, kernel_MarchingCubes_, 2, NULL, gws, NULL, 0, NULL, NULL);
	checkErr(ret, "EnqueKernelError::kernel_MC()");
	ret = clEnqueueReadBuffer(queue_MC, FacesCounterCL, CL_TRUE, 0, sizeof(int), &FacesCounter, 0, NULL, NULL);
	checkErr(ret, "Unable to read output Facenum");

	// compute transformed Vtx
	ret = clEnqueueNDRangeKernel(queue_MC, kernel_Test, 2, NULL, gws, NULL, 0, NULL, NULL);
	checkErr(ret, "EnqueKernelError::Transform()");

	clFinish(queue_MC);
	clEnqueueReleaseGLObjects(queue_MC, 1, &FacesCL, 0, NULL, 0);
	clEnqueueReleaseGLObjects(queue_MC, 1, &VboCL, 0, NULL, 0);

	//cout << "Body Part" << myBodynum << " is : " << FacesCounter[0] << endl;
}

void TSDFManager::SetAllPose(float pose[])
{
	for (int i = 0; i < 16; i++)
	{
		_PoseD2C[i] = pose[i];
		//cout << pose[i] << " ";
	}
	ret = clEnqueueWriteBuffer(queue_MC, _PoseD2CCL, CL_TRUE, 0, 16 * sizeof(float), _PoseD2C, 0, NULL, NULL);
	checkErr(ret, "Unable to write input PoseD2C");
	return;
}

void TSDFManager::SavePLY(string filename)
{
	ofstream  filestr;

	filestr.open(filename, fstream::out);
	while (!filestr.is_open()) {
		std::cout << "Could not open MappingList" << endl;
		return;
	}

	// aquire GL memory? & read out cpu memory

	filestr << ("ply") << endl;
	filestr << ("format ascii 1.0") << endl;
	filestr << "comment ply file created " << endl;
	filestr << "element vertex " << FacesCounter[0] * 3 << endl;
	filestr<<("property float x\n");
	filestr << ("property float y\n");
	filestr << ("property float z\n");
	filestr << ("property float nx\n");
	filestr << ("property float ny\n");
	filestr << ("property float nz\n");
	filestr << "element face " << FacesCounter[0] << endl;
	filestr << ("property list uchar int vertex_indices\n");
	filestr << ("end_header\n");

	for (auto i = 0; i < FacesCounter[0]; i++)
	{
		for (int k = 0; k < 3; k++)
		{
			//int it = Faces[i * 3 + k];
			//Vec3 tmp(Vertices[it * 3 + 0], Vertices[it * 3 + 1], Vertices[it * 3 + 2]);
			//Vec3 tmp(trVertices[it * 3 + 0], trVertices[it * 3 + 1], trVertices[it * 3 + 2]);
			//tmp = GetGloPos(tmp);
			//GLdouble tmpNmle[3] = { Normales[it * 3 + 0], Normales[it * 3 + 1], Normales[it * 3 + 2] };

			//filestr << tmp.x << " " << tmp.y << " " << tmp.z << endl;
			//filestr << tmpNmle[0] << " " << tmpNmle[1] << " " << tmpNmle[2] << endl;
		}
	}



	for (int i = 0; i < FacesCounter[0]; i++)
	{
		//filestr << "3 " << Faces[i * 3 + 0] << " " << Faces[i * 3 + 1] << " " << Faces[i * 3 + 2] << endl;
	}


	filestr.close();
}

void TSDFManager::DrawMesh(Vec3 center)
{

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glColor4d(1.0, 1.0, 1.0, 1.0);
	/* on passe en mode VBO */
	glBindBuffer(GL_ARRAY_BUFFER, VBO_buf);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(stride * sizeof(float)));
	glColorPointer(3,GL_FLOAT, 0,BUFFER_OFFSET(2 * stride * sizeof(float)));

	int allsize = size_x*size_y*size_z;
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FaceBuf);
	glDrawElements(GL_TRIANGLES, FacesCounter[0]*3, GL_UNSIGNED_INT, (void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);	
	glDisableClientState(GL_COLOR_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return;
}














//void TSDFManager::Fuse_RGBD(cv::Mat depth_image, cv::Mat tr)
//{
//	/*
//	// Update TSDF Volume with image & DQ
//
//	//by  TSDF, Weight, DQ, Depth, pose, joint, 
//
//	//Transform = Pose;
//	float convVal = 32767.0;
//	float nu = 0.05; 
//	int line_index = 0;
//	int	column_index = 0;
//	int nb_vert = 0;
//	int nb_miss = 0;
//	float pix[3]{ 0., 0., 1. };
//	float pt[4] = { 0., 0., 0., 1. };
//	int s = 1;
//
//	Transform = tr.clone();
//
//	for (int x = 0; x < size_x / s; x++) {// : # line index(i.e.vertical y axis)
//		//# put the middle of the volume in the middle of the image for axis x
//		pt[0] = (x - c_x) / dim_x;
//		for (int y = 0; size_y / s; y++) {
//			//# put the middle of the volume in the middle of the image for axis y
//			pt[1] = (y - c_y) / dim_y;
//			Vec3 ptT;
//			//# Transofrm in current camera pose
//			ptT.x = Transform.at<unsigned char>(0, 0) * pt[0] + Transform.at<unsigned char>(0, 1) * pt[1] + Transform.at<unsigned char>(0, 3);
//			ptT.y = Transform.at<unsigned char>(1, 0) * pt[0] + Transform.at<unsigned char>(1, 1) * pt[1] + Transform.at<unsigned char>(1, 3);
//			ptT.z = Transform.at<unsigned char>(2, 0) * pt[0] + Transform.at<unsigned char>(2, 1) * pt[1] + Transform.at<unsigned char>(2, 3);
//			for (int z = 0; size_z / s; z++) {
//				nb_vert += 1;
//				//# Project each voxel into  the Image
//				pt[2] = (z - c_z) / dim_z;
//				Vec3 pt_T;
//				pt_T.x = ptT.x + Transform.at<unsigned char>(0, 2) * pt[2];
//				pt_T.y = ptT.y + Transform.at<unsigned char>(1, 2) * pt[2];
//				pt_T.z = ptT.z + Transform.at<unsigned char>(2, 2) * pt[2];
//
//				//# Project onto Image
//				pix[0] = pt_T.x / pt_T.z;
//				pix[1] = pt_T.y / pt_T.z;
//				//pix = np.dot(Image.intrinsic, pix);
//				
//				column_index = int(round(pix[0]));
//				line_index = int(round(pix[1]));
//
//				//# check if the pix is in the frame
//				if (column_index < 0 || column_index > cDepthHeight - 1 || line_index < 0 || line_index > cDepthWidth - 1) {
//					//#print "column_index : %d , line_index : %d" % (column_index, line_index)s
//					nb_miss += 1;
//					if (Weight.at<unsigned char>(x,y,z) == 0) {
//						TSDF.at<unsigned char>(x,y,z) = int(convVal);
//						continue;
//					}
//				}
//
//				//# get corresponding depth
//				double depth = depth_image.at<unsigned short>(line_index,column_index);
//
//				//# Check depth value
//				if (depth == 0) {
//					nb_miss += 1;
//					if (Weight.at<unsigned char>(x,y,z) == 0) {
//						TSDF.at<unsigned char>(x,y,z) = int(convVal);
//						continue;
//					}
//				}
//
//				//# compute distance between voxel and surface
//				double dist = double(-(pt_T.z - depth) / nu);
//				dist = min(1.0, max(-1.0, dist));
//
//				if (dist > 1.0) {
//					TSDF.at<unsigned char>(x,y,z) = 1.0;
//					//print "x %d, y %d, z %d" % (x, y, z);
//				}
//				else {
//					TSDF.at<unsigned char>(x,y,z) = max(-1.0, dist);
//				}
//				//#running average
//				float prev_tsdf = float(TSDF.at<unsigned char>(x,y,z)) / convVal;
//				float prev_weight = float(TSDF.at<unsigned char>(x,y,z));
//
//				TSDF.at<unsigned char>(x,y,z) = int(round(((prev_tsdf*prev_weight + dist) / (prev_weight + 1.0))*convVal));
//				if (dist < 1.0 && dist > -1.0) {
//					Weight.at<unsigned char>(x,y,z) = 1000> Weight.at<unsigned char>(x,y,z)+1 ? Weight.at<unsigned char>(x,y,z) + 1:1000;
//				}
//			}
//		}
//	}*/
//}
//
//void TSDFManager::MargeVtx()
//{
//	/*
//	def MergeVtx(self) :
//		'''
//		This function avoid having duplicate in the lists of vertexes or normales
//		THIS METHOD HAVE A GPU VERSION BUT THE GPU HAVE NOT ENOUGH MEMORY TO RUN IT
//		'''
//		VtxArray_x = np.zeros(self.Size)
//		VtxArray_y = np.zeros(self.Size)
//		VtxArray_z = np.zeros(self.Size)
//		VtxWeights = np.zeros(self.Size)
//		VtxIdx = np.zeros(self.Size)
//		FacesIdx = np.zeros((self.nb_faces[0], 3, 3), dtype = np.uint16)
//		self.Normales = np.zeros((3 * self.nb_faces[0], 3), dtype = np.float32)
//
//		*/
//	int a = TSDF_VOX_SIZE*TSDF_VOX_SIZE*TSDF_VOX_SIZE;
//
//	//# Compute normales
//	for (int i = 0; i < FacesCounter[0]; i++) {
//		Vec3 pt[3];
//		for (int k = 0; k < 3; k++) {
//			int it = Faces[i * 3 + k];
//			//Faces[i, k] = VtxIdx[(FacesIdx[i, k, 0], FacesIdx[i, k, 1], FacesIdx[i, k, 2])];
//			//if (
//			//	myBodynum == BODY_PART_FOOT_L || myBodynum == BODY_PART_FOOT_R
//			//	//||
//			//	//myBodynum == BODY_PART_FOREARM_R || myBodynum == BODY_PART_UPPER_ARM_R
//			//	//|| myBodynum == BODY_PART_DOWN_LEG_L || myBodynum == BODY_PART_DOWN_LEG_R
//			//	) {
//			//		pt[k] = Vec3(trVertices[it * 3 + 0], trVertices[it * 3 + 1], trVertices[it * 3 + 2]);
//			//	}else if (
//			//		//myBodynum == BODY_PART_FOREARM_R 
//			//		//|| myBodynum == BODY_PART_UPPER_ARM_R 
//			//		//||
//			//		//myBodynum == BODY_PART_HAND_R
//			//		false
//			//		) 
//			//	{
//			//		pt[k] = Vec3(trVertices[it * 3 + 0], trVertices[it * 3 + 1], -trVertices[it * 3 + 2]);
//			//	//}
//			//}else {
//			//	pt[k] = Vec3(Vertices[it * 3 + 0], Vertices[it * 3 + 1], Vertices[it * 3 + 2]);
//			//pt[k] = Vec3(trVertices[it * 3 + 0], trVertices[it * 3 + 1], trVertices[it * 3 + 2]);
//			//}
//			pt[k] = Vec3(Vertices[it * 3 + 0], Vertices[it * 3 + 1], Vertices[it * 3 + 2]);
//		}
//		auto v1 = pt[1] - pt[0];
//		auto v2 = pt[2] - pt[0];
//		Vec3 nmle = { v1.y * v2.z - v1.z * v2.y,
//			-v1.x * v2.z + v1.z * v2.x,
//			v1.x * v2.y - v1.y * v2.x };
//
//		for (int k = 0; k < 3; k++) {
//			int it = Faces[i * 3 + k];
//			Normales[it * 3 + 0] = Normales[it * 3 + 0] + nmle.x;
//			Normales[it * 3 + 1] = Normales[it * 3 + 1] + nmle.y;
//			Normales[it * 3 + 2] = Normales[it * 3 + 2] + nmle.z;
//		}
//	}
//	// Normalized normales
//	for (int i = 0; i < FacesCounter[0]; i++) {
//		for (int k = 0; k < 3; k++)
//		{
//			float mag = Vec3(Normales[i * 9 + k * 3 + 0], Normales[i * 9 + k * 3 + 1], Normales[i * 9 + k * 3 + 2]).length();
//			if (mag == 0.0) {
//				Normales[i * 9 + k * 3 + 0] = 0.0;
//				Normales[i * 9 + k * 3 + 1] = 0.0;
//				Normales[i * 9 + k * 3 + 2] = 0.0;
//			}
//			else {
//				Normales[i * 9 + k * 3 + 0] = Normales[i * 9 + k * 3 + 0] / mag;
//				Normales[i * 9 + k * 3 + 1] = Normales[i * 9 + k * 3 + 1] / mag;
//				Normales[i * 9 + k * 3 + 2] = Normales[i * 9 + k * 3 + 2] / mag;
//			}
//		}
//	}
//}
//
//void TSDFManager::SetPose(float pose[])
//{
//	cl_int ret = clEnqueueWriteBuffer(queue_MC, _PoseCL, CL_TRUE, 0, 16 * sizeof(float), pose, 0, NULL, NULL);
//	checkErr(ret, "Unable to write input bone");
//}
//
//Vec3 TSDFManager::GetGloPos(Vec3 pos)
//{
//	Vec3 res(pos), tr(_Pose[3],_Pose[7],_Pose[11]);
//	res.x = _Pose[0] * pos.x + _Pose[1] * pos.y + _Pose[2] * pos.z + _Pose[3];
//	res.y = _Pose[4] * pos.x + _Pose[5] * pos.y + _Pose[6] * pos.z + _Pose[7];
//	res.z = _Pose[8] * pos.x + _Pose[9] * pos.y + _Pose[10] * pos.z + _Pose[11];
//	return res;
//}
//
//Vec3 TSDFManager::GetGloPos_Nmle(Vec3 pos)
//{
//	Vec3 res(pos), tr(_Pose[3], _Pose[7], _Pose[11]);
//	res.x = _Pose[0] * pos.x + _Pose[1] * pos.y + _Pose[2] * pos.z;
//	res.y = _Pose[4] * pos.x + _Pose[5] * pos.y + _Pose[6] * pos.z;
//	res.z = _Pose[8] * pos.x + _Pose[9] * pos.y + _Pose[10] * pos.z;
//	return res;
//}
//
