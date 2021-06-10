#include "RGBDMnager.h"

using namespace std;
cv::Scalar BLUE(255, 0, 0);
cv::Scalar GREEN(0, 255, 0);
cv::Scalar RED(0, 0, 255);
cv::Scalar FACECOLOR(50, 255, 50);

cv::Scalar LEFTEYECOLORDOWN(255, 0, 0);
cv::Scalar LEFTEYECOLORUP(255, 255, 0);
cv::Scalar RIGHTEYECOLORDOWN(0, 255, 0);
cv::Scalar RIGHTEYECOLORUP(0, 255, 255);
cv::Scalar MOUTHCOLORDOWN(0, 0, 255);
cv::Scalar MOUTHCOLORUP(255, 0, 255);

cv::Vec3b LEFTEYEUP(255, 0, 0);
cv::Vec3b LEFTEYEDOWN(0, 255, 0);
cv::Vec3b RIGHTEYEUP(0, 0, 255);
cv::Vec3b RIGHTEYEDOWN(255, 0, 255);
cv::Vec3b LIPUP(255, 255, 0);
cv::Vec3b LIPDOWN(0, 255, 255);




RGBDMnager::RGBDMnager(cl_context context, cl_device_id device) : _context(context), _device(device), _idx(0), _idx_curr(0), _landmarkOK(false)
{
	_idx = FIRST_FRAME_NUMBER;
	_path = "";

	isHumanBody = false;

	_imgD = cv::Mat(cDepthHeight, cDepthWidth, CV_16UC4);
	_imgC = cv::Mat(cDepthHeight, cDepthWidth, CV_8UC4);
	_imgS = cv::Mat(cDepthHeight, cDepthWidth, CV_8UC4);

	_VMap = cv::Mat(cDepthHeight, cDepthWidth, CV_32FC4);
	_NMap = cv::Mat(cDepthHeight, cDepthWidth, CV_32FC4);

	///////Some param initialize//////////
	StitchBody = make_unique<Stitching>();
	Segment_part = make_unique<Segmentation>();

	////////// OPENCL memory ////////////
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


	cl_int ret;
	_depthCL = clCreateImage(_context, CL_MEM_READ_WRITE, &format, &desc, NULL, &ret);
	checkErr(ret, "_depthCL::Buffer()");
	_depthBuffCL = clCreateImage(_context, CL_MEM_READ_ONLY, &format, &desc, NULL, &ret);
	checkErr(ret, "_depthBuffCL::Buffer()");
	_VMapCL = clCreateImage(_context, CL_MEM_READ_WRITE, &format3, &desc, NULL, &ret);
	checkErr(ret, "_VMapCL::Buffer()");
	_NMapCL = clCreateImage(_context, CL_MEM_READ_WRITE, &format3, &desc, NULL, &ret);
	checkErr(ret, "_NMapCL::Buffer()");

	_SegmentedCL = clCreateImage(_context, CL_MEM_READ_ONLY, &format2, &desc, NULL, &ret);
	checkErr(ret, "_SegmentedCL::Buffer()");

	_intrinsicCL = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 11 * sizeof(float), _intrinsic, &ret);
	checkErr(ret, "_intrinsics::Buffer()");
	_PoseCL = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 16 * sizeof(float), _Pose, &ret);
	checkErr(ret, "_PoseCL::Buffer()");


	//////////////////////////////////////////////////////

	_Rotation = Eigen::Matrix3f::Identity();
	_Translation = Eigen::Vector3f::Zero();
	_Rotation_inv = Eigen::Matrix3f::Identity();
	_Translation_inv = Eigen::Vector3f::Zero();

	////////// LOAD OPENCL KERNELS ////////////////////////////
	int Kversion = 1;

	//VMap
	_kernels.push_back(LoadKernel(string(srcOpenCL + "VMap.cl"), string("VmapKernel"), _context, _device));
	ret = clSetKernelArg(_kernels[VMAP_KER], 0, sizeof(_depthCL), &_depthCL);
	ret = clSetKernelArg(_kernels[VMAP_KER], 1, sizeof(_VMapCL), &_VMapCL);
	ret = clSetKernelArg(_kernels[VMAP_KER], 2, sizeof(_intrinsicCL), &_intrinsicCL);
	ret = clSetKernelArg(_kernels[VMAP_KER], 3, sizeof(int), &Kversion);
	ret = clSetKernelArg(_kernels[VMAP_KER], 4, sizeof(cDepthHeight), &cDepthHeight);
	ret = clSetKernelArg(_kernels[VMAP_KER], 5, sizeof(cDepthWidth), &cDepthWidth);
	checkErr(ret, "kernelVMap::setArg()");
	_queue.push_back(clCreateCommandQueue(_context, _device, 0, &ret));
	checkErr(ret, "CommandQueue::CommandQueue()");


	//NMap
	_kernels.push_back(LoadKernel(string(srcOpenCL + "Nmap.cl"), string("NmapKernel"), _context, _device));
	ret = clSetKernelArg(_kernels[NMAP_KER], 0, sizeof(_VMapCL), &_VMapCL);
	ret = clSetKernelArg(_kernels[NMAP_KER], 1, sizeof(_NMapCL), &_NMapCL);
	ret = clSetKernelArg(_kernels[NMAP_KER], 2, sizeof(cDepthHeight), &cDepthHeight);
	ret = clSetKernelArg(_kernels[NMAP_KER], 3, sizeof(cDepthWidth), &cDepthWidth);
	checkErr(ret, "kernelNMAP_KER::setArg()");
	_queue.push_back(clCreateCommandQueue(_context, _device, 0, &ret));
	checkErr(ret, "CommandQueue::CommandQueue()");


	//Birateral Filter
	float sigma_d = 1.0;
	float sigma_r = 500.0;
	int size = 0;
	_kernels.push_back(LoadKernel(string(srcOpenCL + "Bilateral.cl"), string("BilateralKernel"), _context, _device));
	ret = clSetKernelArg(_kernels[BILATERAL_KER], 0, sizeof(_depthBuffCL), &_depthBuffCL);
	ret = clSetKernelArg(_kernels[BILATERAL_KER], 1, sizeof(_depthCL), &_depthCL);
	ret = clSetKernelArg(_kernels[BILATERAL_KER], 2, sizeof(sigma_d), &sigma_d);
	ret = clSetKernelArg(_kernels[BILATERAL_KER], 3, sizeof(sigma_r), &sigma_r);
	ret = clSetKernelArg(_kernels[BILATERAL_KER], 4, sizeof(size), &size);
	ret = clSetKernelArg(_kernels[BILATERAL_KER], 5, sizeof(cDepthHeight), &cDepthHeight);
	ret = clSetKernelArg(_kernels[BILATERAL_KER], 6, sizeof(cDepthWidth), &cDepthWidth);
	checkErr(ret, "BilateralKernel::setArg()");
	_queue.push_back(clCreateCommandQueue(_context, _device, 0, &ret));
	checkErr(ret, "CommandQueue::CommandQueue()");
	///////////////////////////////////////////////////////
	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		checkBodyPart[i] = false;
		maskImages[i] = Mat::zeros(cDepthHeight, cDepthWidth, CV_8UC1);
		BodyPartManager[i] = NULL;
	}
}


RGBDMnager::~RGBDMnager()
{

	clReleaseMemObject(_depthCL);
	clReleaseMemObject(_VMapCL);
	clReleaseMemObject(_NMapCL);
	clReleaseMemObject(_RGBMapCL);
	clReleaseMemObject(_SegmentedCL);
	clReleaseMemObject(_intrinsicCL);
	clReleaseMemObject(_PoseCL);

}

void RGBDMnager::Init()
{
	Compute3D();
	if (!isHumanBody)return;
	PCA();

	StitchBody->SetDepth(masked_depth,_VMap);
	StitchBody->GetVBonesTrans(skeletonVec3, skeletonVec3);

	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		vector<Vec3d> tmp = GetVecPt(i);
		auto mask = maskImages[i].clone();
		float bone[8],joint[8];
		StitchBody->GetJointInfo(i, bone, joint);
		if (!checkBodyPart[i])continue;
		if (tmp.size() > 0) {
			BodyPartManager[i]->Running(bone, joint, skeletonVec3);
		}
	}


}

int RGBDMnager::Load()
{
	// Load Current Image
	cv::Mat depth;
	cv::Mat color;
	clock_t start = clock();
	clock_t end = clock();
	
	//////////////////////////Depth Load//////////////////////////

	vector<Vec2i>vec2;
	vector<Vec3d>vec3;
	isHumanBody = Input->GetData(color, depth, vec2, vec3);
	if (!isHumanBody) {
		return 3;
	}
	depth_curr = depth.clone();//16UC1
	color_curr = color.clone();
	for (int i = 0; i < cDepthHeight; i++) {
		for (int j = 0; j < cDepthWidth; j++) {
			unsigned short d = depth.at<unsigned short>(i, j);
			//_imgD.at<cv::Vec4w>(i, j)[0] = d;
			//_imgD.at<cv::Vec4w>(i, j)[1] = d;
			//_imgD.at<cv::Vec4w>(i, j)[2] = d;
			//_imgD.at<cv::Vec4w>(i, j)[3] = 0;

			_imgD.at<cv::Vec4w>(i, j)[0] = d;
			_imgD.at<cv::Vec4w>(i, j)[1] = d;
			_imgD.at<cv::Vec4w>(i, j)[2] = d;
			_imgD.at<cv::Vec4w>(i, j)[3] = 0;

			_imgS.at<cv::Vec4b>(i, j)[0] = color.at<Vec3b>(i, j)[0];
			_imgS.at<cv::Vec4b>(i, j)[1] = color.at<Vec3b>(i, j)[1];
			_imgS.at<cv::Vec4b>(i, j)[2] = color.at<Vec3b>(i, j)[2];
			_imgS.at<cv::Vec4b>(i, j)[3] = 0;
		}
	}
	_imgS.copyTo(_imgC);
	Push();

	///////////////////skeleton load/////////////////////////////	
	prev_skeleton_vec.clear();
	prev_skeletonVec3.clear();
	if (!skeleton_vec.empty()) {
		for (auto & pt : skeleton_vec)prev_skeleton_vec.push_back(pt);
		for (auto & pt : skeletonVec3)prev_skeletonVec3.push_back(pt);
		skeleton_vec.clear();
		skeletonVec3.clear();
		for (auto & pt : vec2)skeleton_vec.push_back(pt);
		for (auto & pt : vec3)skeletonVec3.push_back(pt);
	}
	else {
		for (auto & pt : vec2)skeleton_vec.push_back(pt);
		for (auto & pt : vec3)skeletonVec3.push_back(pt);
		for (auto & pt : skeleton_vec)prev_skeleton_vec.push_back(pt);
		for (auto & pt : skeletonVec3)prev_skeletonVec3.push_back(pt);
	}
	
	for (int i = 0; i < SkeletonNumber; i++)
	{
		if (skeleton_vec[i].x == 0.0 && skeleton_vec[i].y == 0.0) {
			skeleton_vec[i] = prev_skeleton_vec[i];
			skeletonVec3[i] = prev_skeletonVec3[i];
			cout << "cannot find joint :" << i << endl;
		}
		else if (skeletonVec3[i].z == 0.0) {
			skeletonVec3[i] = prev_skeletonVec3[i];
		}
	}

	/////////////////////Segment Human area//////////////////////
	if (isHumanBody) {
		CropBody(depth_curr);
		Segment_part->Setparam(masked_depth, skeleton_vec);
		Segment_part->Run();
		Segment_part->Draw();
		for (int i = 0; i < BODY_PART_NUMBER; i++){maskImages[i] = Segment_part->GetMaskImage(i).clone();}
		masked_depth.copyTo(masked_depth, Segment_part->GetHumanDeoth() > 0);
		depth_curr.copyTo(background_depth, Segment_part->GetHumanbodyMask()==0);
		return 0;
	}
	else {
		depth_curr.copyTo(background_depth);
		_idx++;
		cout << "no human body" << endl;
		return 2;
	}
	return 0;
}

void RGBDMnager::Draw()
{
	DrawDepth();	
	if (!isHumanBody)return;
	DrawSkeleton();
}

int RGBDMnager::Running()
{
	Compute3D();
	if (!isHumanBody)return 0;

	StitchBody->SetDepth(masked_depth, _VMap);
	StitchBody->GetVBonesTrans(skeletonVec3, prev_skeletonVec3);
	
	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		if (!checkBodyPart[i]) { 
			PCA();
			if(!checkBodyPart[i])continue; 
		}
		float bone[8], joint[8];
		StitchBody->GetJointInfo(i, bone, joint);
		BodyPartManager[i]->Running(bone, joint, skeletonVec3);
	}
	return 0;
}

void RGBDMnager::SetParam(float * Calib, char * path, int KinectVersion)
{
	_path = path;

	for (int i = 0; i < 11; i++)
		_intrinsic[i] = Calib[i];
	_KinectVersion = KinectVersion;

	_Kinect1 = false;
	_Kinect2 = false;
	if (KinectVersion == 1) { // Run with Live data from Kinect Version 1.8
		_Kinect1 = true;
	}
	if (KinectVersion == 2) { // Run with Live data from Kinect Version 1.8
		_Kinect2 = true;
	}

	clSetKernelArg(_kernels[VMAP_KER], 4, sizeof(int), &KinectVersion);

	Input = make_unique<InputManager>(_path);

	Input->LoadData(NUMBER_OF_FRAMES,FIRST_FRAME_NUMBER);
}

void RGBDMnager::DrawDepth()
{
	float pt[3];
	float nmle[3];

	for (int i = 0; i < cDepthHeight - 1; i++) {
		for (int j = 0; j < cDepthWidth - 1; j++) {
			
			pt[0] = _VMap.at<cv::Vec4f>(i, j)[0];
			pt[1] = _VMap.at<cv::Vec4f>(i, j)[1];
			pt[2] = _VMap.at<cv::Vec4f>(i, j)[2];			
				
			nmle[0] = _NMap.at<cv::Vec4f>(i, j)[0];
			nmle[1] = _NMap.at<cv::Vec4f>(i, j)[1];
			nmle[2] = _NMap.at<cv::Vec4f>(i, j)[2];

			glPointSize(1.0);
			glColor4f(GLfloat((nmle[0] + 1.0) / 2.0), GLfloat((nmle[1] + 1.0) / 2.0), GLfloat((nmle[2] + 1.0) / 2.0), GLfloat(1.0));
			glBegin(GL_POINTS);					
			glNormal3f(nmle[0], nmle[1], nmle[2]);
			glVertex3f(pt[0], pt[1], pt[2]);
			glEnd();
		}
	}
}

void RGBDMnager::Draw2ndScreen()
{
	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		if (!checkBodyPart[i])continue;
		BodyPartManager[i]->DrawMesh();
		BodyPartManager[i]->DrawBox();
	}
	if (isHumanBody)DrawSkeleton();
}

void RGBDMnager::DrawSkeleton() {
	float pt[3];
	for (auto& i : skeletonVec3)
	{
		pt[0] = i.x;
		pt[1] = i.y;
		pt[2] = i.z;
		glPointSize(8.0);
		glColor4f(1.0, 0.0, 0.0, 1.0);
		glBegin(GL_POINTS);
		glVertex3f(pt[0], pt[1], pt[2]);
		glEnd();
	}
	for (auto& i : prev_skeletonVec3)
	{
		pt[0] = i.x;
		pt[1] = i.y;
		pt[2] = i.z;
		glPointSize(8.0);
		glColor4f(1.0, 0.0, 1.0, 1.0);
		glBegin(GL_POINTS);
		glVertex3f(pt[0], pt[1], pt[2]);
		glEnd();
	}
}

bool RGBDMnager::Compute3D()
{
	cl_int ret;
	cl_event evts[3];
	size_t origin[3] = { 0, 0, 0 };
	size_t region[3] = { cDepthWidth, cDepthHeight, 1 };
	
	//ret = clEnqueueWriteImage(_queue[BILATERAL_KER], _depthBuffCL, false, origin, region, cDepthWidth * 4 * sizeof(unsigned short), 0, _depth.front().data, 0, NULL, &evts[0]);
	ret = clEnqueueWriteImage(_queue[VMAP_KER], _depthCL, false, origin, region, cDepthWidth * 4 * sizeof(unsigned short), 0, _depth.front().data, 0, NULL, &evts[0]);
	//ret = clEnqueueWriteImage(_queue[BILATERAL_KER], _depthBuffCL, false, origin, region, cDepthWidth * 4 * sizeof(unsigned short), 0, depth_curr.data, 0, NULL, &evts[0]);
	checkErr(ret, "Unable to write input");
	_idx_curr++;

	// Compute Vertex map
	int gws_x = divUp(cDepthHeight, THREAD_SIZE_X);
	int gws_y = divUp(cDepthWidth, THREAD_SIZE_Y);
	size_t gws[2] = { size_t(gws_x*THREAD_SIZE_X), size_t(gws_y*THREAD_SIZE_Y) };
	size_t lws[2] = { THREAD_SIZE_X, THREAD_SIZE_Y };

	//ret = clEnqueueNDRangeKernel(_queue[BILATERAL_KER], _kernels[BILATERAL_KER], 2, NULL, gws, lws, 1, evts, NULL);
	////ret = clEnqueueNDRangeKernel(_queue[BILATERAL_KER], _kernels[BILATERAL_KER], 2, NULL, gws, lws, 0, NULL, NULL);
	//checkErr(ret, "ComamndQueue::enqueueNDRangeKernel()");

	//ret = clFinish(_queue[BILATERAL_KER]);
	//checkErr(ret, "ComamndQueue::Finish()");

	cl_event evtVMap[3];
	ret = clEnqueueNDRangeKernel(_queue[VMAP_KER], _kernels[VMAP_KER], 2, NULL, gws, lws, 1, evts, &evtVMap[0]);
	//ret = clEnqueueNDRangeKernel(_queue[VMAP_KER], _kernels[VMAP_KER], 2, NULL, gws, lws, 0, NULL,NULL);
	checkErr(ret, "ComamndQueue::enqueueNDRangeKernel()");

	ret = clEnqueueReadImage(_queue[VMAP_KER], _VMapCL, true, origin, region, cDepthWidth * 4 * sizeof(float), 0, _VMap.data, 1, &evtVMap[0], &evtVMap[1]);
	//ret = clEnqueueReadImage(_queue[VMAP_KER], _VMapCL, true, origin, region, cDepthWidth * 4 * sizeof(float), 0, _VMap.data, 0, NULL,NULL);
	checkErr(ret, "Unable to read output");

	ret = clFinish(_queue[VMAP_KER]);
	checkErr(ret, "ComamndQueue::Finish()");

	// Compute Normal map
	ret = clEnqueueNDRangeKernel(_queue[NMAP_KER], _kernels[NMAP_KER], 2, NULL, gws, lws, 2, &evtVMap[0], &evtVMap[2]);
	//ret = clEnqueueNDRangeKernel(_queue[NMAP_KER], _kernels[NMAP_KER], 2, NULL, gws, lws, 0, NULL,NULL);
	checkErr(ret, "ComamndQueue::enqueueNDRangeKernel()");

	ret = clEnqueueReadImage(_queue[NMAP_KER], _NMapCL, true, origin, region, cDepthWidth * 4 * sizeof(float), 0, _NMap.data, 2, &evtVMap[1], NULL);
	//ret = clEnqueueReadImage(_queue[NMAP_KER], _NMapCL, true, origin, region, cDepthWidth * 4 * sizeof(float), 0, _NMap.data, 0, NULL, NULL);
	checkErr(ret, "Unable to read output");

	ret = clFinish(_queue[NMAP_KER]);
	checkErr(ret, "ComamndQueue::Finish()");

	return true;
}

void RGBDMnager::CropBody(cv::Mat &depth)
{
	clock_t start = clock();
	clock_t end = clock();

	vector<float> x;
	vector<float> y;
	//distance head to neck.Let us assume this is enough for all borders

	Vec2 head = skeleton_vec[Head];
	Vec2 neck = skeleton_vec[Neck];
	int distH2N = int(sqrt((head.x-neck.x)*(head.x - neck.x) + (head.y - neck.y)*(head.y-neck.y)) + 10);
	
	for (auto it = skeleton_vec.begin(); it != skeleton_vec.end(); ++it){
		//if (it->x == 0 || it->y == 0)continue;
		x.push_back((float)it->x);
		y.push_back((float)it->y);
	}
	int colStart = int(*min_element(x.begin(),x.end()) - distH2N);
	int lineStart = int(*min_element(y.begin(),y.end()) - distH2N);
	int colEnd = int(*max_element(x.begin(),x.end()) + distH2N);
	int lineEnd = int(*max_element(y.begin(),y.end()) + distH2N);
	colStart = max(0, colStart);
	lineStart = max(0, lineStart);
	colEnd = min(colEnd, cDepthWidth);
	lineEnd = min(lineEnd, cDepthHeight);

	unsigned short tmp = depth.at<unsigned short>(int(skeleton_vec[SpineBase].y), int(skeleton_vec[SpineBase].x));
	double min, max;
	
	Cropped_Box = cv::Mat(depth, cv::Rect(colStart, lineStart, (colEnd - colStart), (lineEnd - lineStart)));	

	start = clock();
	
	cv::minMaxLoc(Cropped_Box, &min, &max);
	
	end = clock();
	cout << "crop1 " << (end - start) << endl;

	float thresh = 500.0 *8.0;//mm * 3bit shift
	if(NII==1.0) thresh = 500.0;

	//body segment
	cv::Mat mask_BG_max = cv::Mat::ones(depth.size(), CV_8UC1);
	cv::Mat mask_BG_min = cv::Mat::ones(depth.size(), CV_8UC1);

	// human area mask
	cv::Mat crop = cv::Mat::zeros(depth.size(), CV_8UC1);
	cv::Mat croprect = crop(cv::Rect(colStart, lineStart, (colEnd - colStart), (lineEnd - lineStart)));
	croprect = croprect + 1;
	Mask_BG = crop.clone();

	if (tmp > max || tmp < min) tmp = unsigned short((min + max) / 2);

	mask_BG_max.setTo(0, depth > thresh + tmp);
	mask_BG_max.setTo(0, depth < tmp - thresh);
	cv::Mat mask = mask_BG_max;
	mask = mask.mul(crop);
	masked_depth = cv::Mat::zeros(masked_depth.size(), masked_depth.type());
	background_depth = cv::Mat::zeros(background_depth.size(), background_depth.type());
	depth_curr.copyTo(masked_depth, mask);

	return;
}







void RGBDMnager::PCA()
{
	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		if (checkBodyPart[i])continue;
		vector<Vec3d> tmp = GetVecPt(i);
		auto a = maskImages[i].clone();
		if (tmp.size() > 0) {
			cout << "bodypart num : " << i << endl;
			//cout << "tmp : " << tmp.size() << endl;
			BodyPartManager[i] =new BodyPart(_context,_device,tmp, masked_depth,_depthCL,colorCL,i);
			checkBodyPart[i] = true;
		}
		else {
			cout << "Nothing point error:: num : " << i << endl;
			checkBodyPart[i] = false;
		}
	}
}

Vec3 RGBDMnager::pt23D(Vec2 pt) {
	float d = _depth.front().at<unsigned short>(int(pt.y), int(pt.x));
	Vec3 pt3D;
	pt3D.x = _VMap.at<cv::Vec4f>(int(pt.y), int(pt.x))[0];
	pt3D.y = _VMap.at<cv::Vec4f>(int(pt.y), int(pt.x))[1];
	pt3D.z = _VMap.at<cv::Vec4f>(int(pt.y), int(pt.x))[2];
	return pt3D;
}

//Return vector<point> mask>0 && z>0
vector<Vec3d> RGBDMnager::GetVecPt(int num)
{
	vector<Vec3d> res;
	auto a = maskImages[num].clone();
	//imshow(to_string(num), a * 100);
	//waitKey(1);
	for (int i = 0; i < _depth.front().cols; i++)
	{
		for (int j = 0; j < _depth.front().rows; j++)
		{
			auto b = a.at<unsigned char>(j, i);
			if (b!= 0 && masked_depth.at<unsigned short>(j,i) != 0) {
				Vec3d pt = GetProjectedPoint(Vec2(i, j), masked_depth.at<unsigned short>(j, i));
				//Vec3 pt(_VMap.at<cv::Vec4f>(j,i)[0], -_VMap.at<cv::Vec4f>(j, i)[1], -_VMap.at<cv::Vec4f>(j, i)[2]);
				if (pt(2) <0) {
					res.push_back(pt);
				}
			}
		}
	}
	return res;
}



void RGBDMnager::Reset()
{
	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		checkBodyPart[i] = false;
		maskImages[i] = Mat::zeros(cDepthHeight, cDepthWidth, CV_8UC1);
	}
	Input->ResetFilecount();
	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		if(BodyPartManager[i]!=NULL)delete BodyPartManager[i];
	}
	prev_skeleton_vec.clear();
	prev_skeletonVec3.clear();
	skeleton_vec.clear();
	skeletonVec3.clear();
	Load();
	Init();
}

void RGBDMnager::getBackImages(cv::Mat & depth, cv::Mat & color)
{
	//background_depth.copyTo(depth);
	depth_curr.copyTo(depth, masked_depth == 0);
	color_curr.copyTo(color);
}

void RGBDMnager::getImages(cv::Mat & depth, cv::Mat & color)
{
	depth_curr.copyTo(depth);
	color_curr.copyTo(color);
}

void RGBDMnager::SavePly()
{
	char dirpath_cstr[50];
	time_t now = time(NULL);
	struct tm *pnow = localtime(&now);
	sprintf(dirpath_cstr, "%smesh/%04d_%02d_%02d_%02d_%02d_%02d/", dirData.c_str(), pnow->tm_year + 1900, pnow->tm_mon + 1, pnow->tm_mday, pnow->tm_hour, pnow->tm_min, pnow->tm_sec);
	_mkdir(dirpath_cstr);
	string dirpath(dirpath_cstr);
	_mkdir((dirpath + "mesh/").c_str());
	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		if (!checkBodyPart[i])continue;
		stringstream filename;
		filename <<"mesh/" << i << ".ply";
		BodyPartManager[i]->SavePLY(dirpath_cstr + filename.str());
		cout << "save : " << filename.str() << endl;
	}
}



double RGBDMnager::getOrientation(const vector<Vec3>& pts, cv::Mat & img)
{    //Construct a buffer used by the pca analysis
	int sz = static_cast<int>(pts.size());
	cv::Mat data_pts = cv:: Mat(sz, 3, CV_64FC1);

	for (int i = 0; i < data_pts.rows; ++i)
	{
		data_pts.at<double>(i, 0) = pts[i].x;
		data_pts.at<double>(i, 1) = pts[i].y;
		data_pts.at<double>(i, 2) = pts[i].z;
	}
	//Perform PCA analysis
	cv::PCA pca_analysis(data_pts,cv::Mat(), cv::PCA::DATA_AS_ROW);
	
	//Store the center of the object
	cv::Point cntr = cv::Point(static_cast<int>(pca_analysis.mean.at<double>(0, 0)),
		static_cast<int>(pca_analysis.mean.at<double>(0, 1)));
	//Store the eigenvalues and eigenvectors
	vector<cv::Point3d> eigen_vecs(3);
	vector<double> eigen_val(3);
	for (int i = 0; i < 3; ++i)
	{
		eigen_vecs[i] =cv:: Point3d(pca_analysis.eigenvectors.at<double>(i, 0),
			pca_analysis.eigenvectors.at<double>(i, 1),
			pca_analysis.eigenvectors.at<double>(i, 2));
		eigen_val[i] = pca_analysis.eigenvalues.at<double>(i);
	}
	cout << "eigen_val :[ " << eigen_val[0] << " , " << eigen_val[1] << " , " << eigen_val[2] << " ]" << endl;
	cout << "eigen_vec :" << eigen_vecs[0] << "," << eigen_vecs[1] << "," << eigen_vecs[2] << endl;

	
	corners[0] = eigen_val[0] * eigen_vecs[0] + eigen_val[1] * eigen_vecs[1] + eigen_val[2] * eigen_vecs[2];
	corners[1] = eigen_val[0] * eigen_vecs[0] + eigen_val[1] * eigen_vecs[1] - eigen_val[2] * eigen_vecs[2];
	corners[2] = eigen_val[0] * eigen_vecs[0] - eigen_val[1] * eigen_vecs[1] - eigen_val[2] * eigen_vecs[2];
	corners[3] = eigen_val[0] * eigen_vecs[0] - eigen_val[1] * eigen_vecs[1] + eigen_val[2] * eigen_vecs[2];

	corners[4] = -eigen_val[0] * eigen_vecs[0] + eigen_val[1] * eigen_vecs[1] + eigen_val[2] * eigen_vecs[2];
	corners[5] = -eigen_val[0] * eigen_vecs[0] + eigen_val[1] * eigen_vecs[1] - eigen_val[2] * eigen_vecs[2];
	corners[6] = -eigen_val[0] * eigen_vecs[0] - eigen_val[1] * eigen_vecs[1] - eigen_val[2] * eigen_vecs[2];
	corners[7] = -eigen_val[0] * eigen_vecs[0] - eigen_val[1] * eigen_vecs[1] + eigen_val[2] * eigen_vecs[2];

	return 0.0;
}
