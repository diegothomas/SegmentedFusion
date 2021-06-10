#include "SceneManager.h"

bool CheckHumanfromSkeleton(vector<Vec2i> skeleton);
SceneManager::SceneManager()
{
	cout << "default constructer @ SceneManager" << endl;
}

SceneManager::SceneManager(cl_context _context, cl_device_id _device)
{
	cout << "sceneManager constructer ..." << endl;

	context = _context;
	device = _device;
	lDeviceIds.push_back(device);
	int res = InitOpenCLMemory();
	Segment_part = make_unique<Segmentation>();

	///////////////////////Kinect/////////////////////////
	scene = KINECT_INPUT;


	cam.initialize();
	cam.showProfile();

	if (isKinect)return;

	int skeletoncase = 5;
	int startframe = 0;
	int num_frame = 200;

	string camname[3] = {
		//correspondanse cameranumber & camera pos
		"camera_0_right",
		"camera_0_left",
		"camera_0_center"
	};
	vector<string> foldername;

	switch (skeletoncase)
	{
	case 0:
		folder = "Data_SegmentedFusion/dataset_3cameras/";
		//string foldername[3] = { folder + "camera_1/",folder + "camera_2/",folder + "camera_0/" };
		foldername.push_back(folder + "camera_1/");
		foldername.push_back(folder + "camera_2/");
		foldername.push_back(folder + "camera_0/");

		camname[0] = "camera_58";
		camname[1] = "camera_48";
		camname[2] = "camera_00";
		break;
	case 1:
		folder = "Data_SegmentedFusion/dataset33/";
		foldername.push_back(folder);
		foldername.push_back(folder);
		foldername.push_back(folder);
		break;
	case 2:
		startframe = 80; num_frame = 300; 600;
		folder = "Data_SegmentedFusion/simple_1226/";
		// right : 1=58 / center : 0=00 / left : 2=38

		foldername.push_back(folder + "camera_1/");
		foldername.push_back(folder + "camera_0/");
		foldername.push_back(folder + "camera_2/");
		
		camname[0] = "camera_58";
		camname[1] = "camera_00";
		camname[2] = "camera_38";

		break;

	case 3:
		startframe = 200; num_frame = 300;
		folder = "Data_SegmentedFusion/stop0107/";
		foldername.push_back(folder);
		foldername.push_back(folder);
		foldername.push_back(folder);
		camname[0] = "camera_48";
		break;

	case 4:
		startframe = 100; num_frame = 300;
		folder = "Data_SegmentedFusion/center00/";
		foldername.push_back(folder+"camera_1/");
		foldername.push_back(folder+"camera_0/");
		foldername.push_back(folder);
		camname[0] = "camera_58";
		camname[1] = "camera_00";
		break;
	case 5:
		startframe = 0; num_frame = 89;
		folder = "Data_SegmentedFusion/dataset35/";
		foldername.push_back(folder);
		camname[0] = "camera_58";
	default:
		break;
	}

	for (int i = 0; i < num_PC; i++)
	{
		InputMng[i] = make_unique<InputManager>(dirData + foldername[i]);
		InputMng[i]->LoadData(num_frame,startframe,camname[i]);
		vecFolder.push_back(foldername[i]);
		// InfiniTAM
		if (runITM) {
			ITMMng.push_back(make_unique<MyITMNamespase::ITMManager>(MyITMNamespase::ITMManager()));
			ITMMng[i]->cameranumber = i;
		}
	}
	scene = ONEFRAME;

	extrinsic_0to1 = cv::Mat::eye(4, 4, CV_64F);
	extrinsic_2to1 = cv::Mat::eye(4, 4, CV_64F);
	Initialize_Extrinsic_D2D();
	isComputed = true;

	for (int i = 0; i < num_PC; i++)
	{
		Eigen::Matrix4d tmp;
		CVMat2EigenMatrix(Mat4d(extrinsicsD2D[i]), tmp);
		extrinsicsD2DEigen.push_back(tmp);
	}
	///////////////InfiniTAM////////////////////////

	///////////////////////OpenCL/////////////////////////


	cout << "sceneManager constructer ... Done" << endl;
}

SceneManager::~SceneManager()
{
	clReleaseMemObject(_depthCL);
	clReleaseMemObject(_VMapCL);
	clReleaseMemObject(_NMapCL);
	clReleaseMemObject(_RGBMapCL);
	clReleaseMemObject(_SegmentedCL);
	clReleaseMemObject(_intrinsicCL);
	clReleaseMemObject(_PoseCL);
}

void SceneManager::Initialize_Extrinsic()
{
	//mm to OpenGL scale
	double temp_div = 1000.0;
	//camera 0 to 1 

	//Load xml
	cv::FileStorage fs01(dirData + "calib12/extrinsics_cvmat_0w.xml", cv::FileStorage::READ);
	cv::FileStorage fs12(dirData + "calib12/extrinsics_cvmat_1w.xml", cv::FileStorage::READ);
	if (!fs01.isOpened()||!fs12.isOpened()) {
		std::cout << "File can not be opened." << std::endl;
		return;
	}

	cv::Mat tmpEx01_R, tmpEx01_t, tmpEx12_R, tmpEx12_t;
	fs01["rotation"] >> tmpEx01_R;
	fs01["translation"] >> tmpEx01_t;
	fs12["rotation"] >> tmpEx12_R;
	fs12["translation"] >> tmpEx12_t;

	tmpEx01_t = tmpEx01_t / temp_div;
	tmpEx12_t = tmpEx12_t / temp_div;

	//calc Extrinsic
	cv::Mat tmpEx01 = cv::Mat::eye(4, 4, CV_64F);
	cv::Mat tmpEx12 = cv::Mat::eye(4, 4, CV_64F);

	cv::Mat roi;
	roi = tmpEx01(cv::Rect(0, 0, 3, 3));
	tmpEx01_R.copyTo(roi);
	roi = tmpEx01(cv::Rect(3, 0, 1, 3));
	tmpEx01_t.copyTo(roi);

	roi = tmpEx12(cv::Rect(0, 0, 3, 3));
	tmpEx12_R.copyTo(roi);
	roi = tmpEx12(cv::Rect(3, 0, 1, 3));
	tmpEx12_t.copyTo(roi);

	cv::Mat tmp_convert;
	tmp_convert = tmpEx12.inv() * tmpEx01;
	tmp_convert.convertTo(extrinsic_0to1, CV_32F);

	fs01.release(); fs12.release();

	//camera 2 to 1

	//Load xml
	fs01.open(dirData + "calib01/extrinsics_cvmat_0w.xml", cv::FileStorage::READ);
	fs12.open(dirData + "calib01/extrinsics_cvmat_1w.xml", cv::FileStorage::READ);
	if (!fs01.isOpened() || !fs12.isOpened()) {
		std::cout << "File can not be opened." << std::endl;
		return;
	}

	fs01["rotation"] >> tmpEx01_R;
	fs01["translation"] >> tmpEx01_t;
	fs12["rotation"] >> tmpEx12_R;
	fs12["translation"] >> tmpEx12_t;

	tmpEx01_t = tmpEx01_t / temp_div;
	tmpEx12_t = tmpEx12_t / temp_div;

	//calc Extrinsic
	tmpEx01 = cv::Mat::eye(4, 4, CV_64F);
	tmpEx12 = cv::Mat::eye(4, 4, CV_64F);

	roi = tmpEx01(cv::Rect(0, 0, 3, 3));
	tmpEx01_R.copyTo(roi);
	roi = tmpEx01(cv::Rect(3, 0, 1, 3));
	tmpEx01_t.copyTo(roi);

	roi = tmpEx12(cv::Rect(0, 0, 3, 3));
	tmpEx12_R.copyTo(roi);
	roi = tmpEx12(cv::Rect(3, 0, 1, 3));
	tmpEx12_t.copyTo(roi);

	tmp_convert = (tmpEx12.inv() * tmpEx01).inv();
	tmp_convert.convertTo(extrinsic_2to1, CV_32F);
	fs01.release(); fs12.release();


}

void SceneManager::Initialize_Extrinsic_D2D()
{
	cv::FileStorage fs(dirData + folder + "extrinsics_between_kinects.xml", cv::FileStorage::READ);
	if (!fs.isOpened()) {
		cout << " File can not be opened" << endl;
		cout << dirData + "dataset1109/extrinsics_betweeen_kinects.xml" << endl;
		extrinsicsD2D[0] = Mat::eye(4, 4, CV_64F);
		extrinsicsD2D[1] = Mat::eye(4, 4, CV_64F);
		extrinsicsD2D[2] = Mat::eye(4, 4, CV_64F);
		return;
	}

	fs["Rt01"] >> extrinsic_0to1;
	fs["Rt02"] >> extrinsic_2to1;
	extrinsicsD2D[0] = extrinsic_0to1;
	extrinsicsD2D[1] = Mat::eye(4, 4, CV_64F);
	extrinsicsD2D[2] = extrinsic_2to1;

	fs.release();
}

int SceneManager::InitOpenCLMemory() {

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
	colorCL = clCreateImage(context, CL_MEM_READ_WRITE, &format2, &desc, NULL, &ret);
	checkErr(ret, "colorCL::Buffer()");

	_depthCL = clCreateImage(context, CL_MEM_READ_WRITE, &format, &desc, NULL, &ret);
	checkErr(ret, "_depthCL::Buffer()");
	_depthBuffCL = clCreateImage(context, CL_MEM_READ_ONLY, &format, &desc, NULL, &ret);
	checkErr(ret, "_depthBuffCL::Buffer()");
	_VMapCL = clCreateImage(context, CL_MEM_READ_WRITE, &format3, &desc, NULL, &ret);
	checkErr(ret, "_VMapCL::Buffer()");
	_NMapCL = clCreateImage(context, CL_MEM_READ_WRITE, &format3, &desc, NULL, &ret);
	checkErr(ret, "_NMapCL::Buffer()");

	_SegmentedCL = clCreateImage(context, CL_MEM_READ_ONLY, &format2, &desc, NULL, &ret);
	checkErr(ret, "_SegmentedCL::Buffer()");

	_intrinsicCL = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 11 * sizeof(float), _intrinsic, &ret);
	checkErr(ret, "_intrinsics::Buffer()");
	_PoseCL = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 16 * sizeof(float), _Pose, &ret);
	checkErr(ret, "_PoseCL::Buffer()");


	//////////////////////////////////////////////////////


	////////// LOAD OPENCL KERNELS ////////////////////////////
	int Kversion = 1;

	//VMap
	_kernels.push_back(LoadKernel(string(srcOpenCL + "VMap.cl"), string("VmapKernel"), context, lDeviceIds[0]));
	ret = clSetKernelArg(_kernels[VMAP_KER], 0, sizeof(_depthCL), &_depthCL);
	ret = clSetKernelArg(_kernels[VMAP_KER], 1, sizeof(_VMapCL), &_VMapCL);
	ret = clSetKernelArg(_kernels[VMAP_KER], 2, sizeof(_intrinsicCL), &_intrinsicCL);
	ret = clSetKernelArg(_kernels[VMAP_KER], 3, sizeof(int), &Kversion);
	ret = clSetKernelArg(_kernels[VMAP_KER], 4, sizeof(cDepthHeight), &cDepthHeight);
	ret = clSetKernelArg(_kernels[VMAP_KER], 5, sizeof(cDepthWidth), &cDepthWidth);
	checkErr(ret, "kernelVMap::setArg()");
	_queue.push_back(clCreateCommandQueue(context, lDeviceIds[0], 0, &ret));
	checkErr(ret, "CommandQueue::CommandQueue()");


	//NMap
	_kernels.push_back(LoadKernel(string(srcOpenCL + "Nmap.cl"), string("NmapKernel"), context, lDeviceIds[0]));
	ret = clSetKernelArg(_kernels[NMAP_KER], 0, sizeof(_VMapCL), &_VMapCL);
	ret = clSetKernelArg(_kernels[NMAP_KER], 1, sizeof(_NMapCL), &_NMapCL);
	ret = clSetKernelArg(_kernels[NMAP_KER], 2, sizeof(cDepthHeight), &cDepthHeight);
	ret = clSetKernelArg(_kernels[NMAP_KER], 3, sizeof(cDepthWidth), &cDepthWidth);
	checkErr(ret, "kernelNMAP_KER::setArg()");
	_queue.push_back(clCreateCommandQueue(context, lDeviceIds[0], 0, &ret));
	checkErr(ret, "CommandQueue::CommandQueue()");


	//Birateral Filter
	float sigma_d = 1.0;
	float sigma_r = 500.0;
	int size = 0;
	_kernels.push_back(LoadKernel(string(srcOpenCL + "Bilateral.cl"), string("BilateralKernel"), context, lDeviceIds[0]));
	ret = clSetKernelArg(_kernels[BILATERAL_KER], 0, sizeof(_depthBuffCL), &_depthBuffCL);
	ret = clSetKernelArg(_kernels[BILATERAL_KER], 1, sizeof(_depthCL), &_depthCL);
	ret = clSetKernelArg(_kernels[BILATERAL_KER], 2, sizeof(sigma_d), &sigma_d);
	ret = clSetKernelArg(_kernels[BILATERAL_KER], 3, sizeof(sigma_r), &sigma_r);
	ret = clSetKernelArg(_kernels[BILATERAL_KER], 4, sizeof(size), &size);
	ret = clSetKernelArg(_kernels[BILATERAL_KER], 5, sizeof(cDepthHeight), &cDepthHeight);
	ret = clSetKernelArg(_kernels[BILATERAL_KER], 6, sizeof(cDepthWidth), &cDepthWidth);
	checkErr(ret, "BilateralKernel::setArg()");
	_queue.push_back(clCreateCommandQueue(context, lDeviceIds[0], 0, &ret));
	checkErr(ret, "CommandQueue::CommandQueue()");

	return 0;
}

void SceneManager::ProcessFrame()
{
	if (isKinect) {
		cam.update();
		//FrameKinectProjection();
		//return;
	}
	switch (scene)
	{
	case SceneManager::ONEFRAME:
		FrameProjection();
		scene = WAITFRMAE;
		break;
	case SceneManager::CONTFRAME:
		if (!FrameProjection()) {
			scene = WAITFRMAE;
		}
		//cout << "cont frame" << endl;
		break;
	case SceneManager::CAPTURE:
		Capture();
		scene = WAITFRMAE;
		break;
	case SceneManager::RESET:
		Reset();
		scene = WAITFRMAE;
		break;
	case SceneManager::KINECT_INPUT:
		if(isKinect)FrameKinectProjection();
		else FrameProjection();
		//scene = WAITFRMAE;
		cout << "Kinect capture \r";
		break;
	case SceneManager::WAITFRMAE:
	default:
		break;
	}
}

void SceneManager::Draw()
{
	if (runBackground)Draw3D();
	if (isKinect) {

		// kinect inputs
		Mat color, depth;
		vector<Vec2i> vec2;
		vector<Vec3d> vec3;
		cam.getCurrentData(color, depth, vec2, vec3);
		skeletonVector3DCurrent.clear();
		Mat depthImage_forshow = depth.clone();
		for (auto p : vec3) {
			skeletonVector3DCurrent.push_back(Vec3d(p(0), p(1), p(2)));
			double x, y;
			x = 0.0;
			y = 0.0;
			x = p(0) * Calib[0] / p(2) + cDepthWidth / 2.0;
			y = p(1) * Calib[1] / p(2) + cDepthHeight / 2.0;
			if (x >= cDepthWidth || x <= 0 || y >= cDepthHeight || y <= 0) {}
			else {
				cv::circle(depthImage_forshow, cv::Point(x, y), 5, 65535, -1);
			}

		}
		imshow("projection 3D", depthImage_forshow*10);
		cam.show();

		for (int i = 0; i < HumanBodies.size(); i++)
		{
			HumanBodies[i]->Draw();
		}

		for (int i = 0; i < skeletonVector3DCurrent.size(); i++)
		{
			Vec3d vec = skeletonVector3DCurrent[i];
			glPointSize(8.0);
			glBegin(GL_POINTS);
			glVertex3f(vec(0), -vec(1), -vec(2));
			glEnd();
		}

		return;
	}
	for (int i = 0; i < HumanBodies.size(); i++)
	{
		HumanBodies[i]->Draw();
	}
	// TODO : InfiniTAM manager?
	for (int i = 0; i < num_PC; i++)
	{
		if (runITM)ITMMng[i]->Draw();
		//glColor3b(int(i==0), int(i==1), (int)1==2);
		//glPointSize(10.0);
		//glBegin(GL_POINTS);
		//Eigen::Vector4d pt(0, 0, 0, 1);
		//pt = extrinsicsD2DEigen[i] * pt;
		////cout << pt << endl;
		//glVertex3d(pt.x(),-pt.y(),-pt.z());
		//glEnd();

		//imshow(to_string(i), backGroundBuffer[i]);
		//waitKey(1);
	}

}

bool SceneManager::FrameProjection()
{
	// in one frame, get depth image & all skeletons
	// then processing (depth + skeletons -> humanmasks + background)
	// next, for each humanbodys, make 3D mesh
	// -> Draw()

	int j = 0;
	for (int i = 0; i < num_PC; i++)
	{
		Mat color, depth;
		vector<Vec2i> vec2;
		vector<Vec3d> vec3;
		// load current data
		if (!InputMng[i]->GetData(color, depth, vec2, vec3)) { cout << "Last frame(maybe)" << endl; return false; }
		cout << "Image number : " << InputMng[i]->fileCount << "\r";

		background_depth = depth.clone();
		backGroundBuffer[i] = background_depth.clone();
		colorBuffer[i] = color.clone();
		// check humanbody exist
		if (!CheckHumanfromSkeleton(vec2)) { continue; }

		Processing(color, depth, vec2, vec3);
		background_depth = Mat::zeros(cDepthHeight, cDepthWidth, CV_8UC1);
		depth.copyTo(background_depth, Segment_part->GetHumanbodyMask() == 0);
		backGroundBuffer[i] = background_depth.clone();
		
		while(j >= HumanBodies.size()){ HumanBodies.push_back(make_unique<HumanBody>(context, device, _depthCL, colorCL)); }
		if (!HumanBodies[j]->isInitialized) { HumanBodies[j]->InitilizeHumanBody(); }
		HumanBodies[j]->isHumanBody = true;
		HumanBodies[j]->SetPosParams(extrinsicsD2DEigen[i], InputMng[i]->exColor2Depth.inverse(),i);
		HumanBodies[j]->SetParams(*Segment_part, vec2, vec3,color);
		HumanBodies[j]->Running();
		//j++;
	}
	if (runITM) {
		for (int i = 0; i < num_PC; i++)
		{
			ITMMng[i]->SetImages(colorBuffer[i], backGroundBuffer[i]);
			ITMMng[i]->Running();
		}
	}

	if (runBackground) {
		PointClouds.clear(); PointClouds_Color.clear();
		for (int i = 0; i < num_PC; i++)
		{
			Comupre3D(i, backGroundBuffer[i], colorBuffer[i]);
		}
	}
	
	return true;
}

bool SceneManager::FrameKinectProjection()
{
	// in one frame, get depth image & all skeletons
	// then processing (depth + skeletons -> humanmasks + background)
	// next, for each humanbodys, make 3D mesh
	// -> Draw()
	int j = 0;

	// kinect inputs
	Mat color, depth;
	vector<Vec2i> vec2;
	vector<Vec3d> vec3;
	cam.getCurrentData(color, depth, vec2, vec3);

	if (runBackground) {
		PointClouds.clear(); PointClouds_Color.clear();
		vector<Eigen::Vector4d> PointCloud;
		vector<Vec3b> PointCloudColor;
		Eigen::Vector4d pt(0.0, 0.0, 0.0, 1.0);
		//double div = 690.0;
		for (int i = 0; i < depth.size().height; i++)
		{
			for (int j = 0; j < depth.size().width; j++) {
				pt.z() = depth.at<unsigned short>(i, j) / Calib[10];
				pt.x() = pt.z() == 0.0 ? 0.0 : pt.z() * ((float(j) - Calib[2]) / Calib[0]);
				pt.y() = pt.z() == 0.0 ? 0.0 : pt.z() * ((float(i) - Calib[3]) / Calib[1]);
				pt.z() = pt.z() == 0.0 ? 0.0 : pt.z();

				if (pt.x() == 0.0&&pt.y() == 0.0 || pt.z() == 0.0)continue;
				Vec3b colorpt(1.0, 0.0, 0.0);
				PointCloudColor.push_back(colorpt);
				PointCloud.push_back(pt);
			}
		}
		PointClouds.push_back(PointCloud);
		PointClouds_Color.push_back(PointCloudColor);
		
	}


	background_depth = depth.clone();
	backGroundBuffer[0] = background_depth.clone();
	colorBuffer[0] = color.clone();
	// check humanbody exist
	if (!CheckHumanfromSkeleton(vec2) || !cam.skeleton_tracked) { return false; }
	Processing(color, depth, vec2, vec3);
	background_depth = Mat::zeros(cDepthHeight, cDepthWidth, CV_8UC1);
	depth.copyTo(background_depth, Segment_part->GetHumanbodyMask() == 0);
	backGroundBuffer[0] = background_depth.clone();

	while (j >= HumanBodies.size()) { HumanBodies.push_back(make_unique<HumanBody>(context, device, _depthCL, colorCL)); }
	if (!HumanBodies[j]->isInitialized) { HumanBodies[j]->InitilizeHumanBody(); }
	HumanBodies[j]->isHumanBody = true;
	HumanBodies[j]->SetPosParams(Eigen::Matrix4d::Identity(), Eigen::Matrix4d::Identity(), 0);
	HumanBodies[j]->SetParams(*Segment_part, vec2, vec3, color);
	HumanBodies[j]->Running();


	return true;
}

void SceneManager::Capture()
{
	cout << "capturing...";
	scene = WAITFRMAE;
	cout << ".. Done" << endl;
}

void SceneManager::Reset()
{
	cout << "Reset" << endl;
	HumanBodies.clear();
	if (isKinect) { return; }
	for (int i = 0; i < num_PC; i++)
	{
		InputMng[i]->ResetFilecount();
	}
}

void SceneManager::SaveSkeletonTr() {
	//// save skeleton @ full view

}

void SceneManager::Processing(Mat &color, Mat &depth, vector<Vec2i> &skeletonVec2, vector<Vec3d> &skeletonVec3) {
	vector<float> x;
	vector<float> y;
	vector<Vec2> skeleton_vec;
	for (auto p : skeletonVec2)skeleton_vec.push_back(Vec2(p(0), p(1)));

	Vec2 head = skeleton_vec[Head];
	Vec2 neck = skeleton_vec[Neck];
	int distH2N = int(sqrt((head.x - neck.x)*(head.x - neck.x) + (head.y - neck.y)*(head.y - neck.y)) + 10);

	for (auto it = skeleton_vec.begin(); it != skeleton_vec.end(); ++it) {
		//if (it->x == 0 || it->y == 0)continue;
		x.push_back((float)it->x);
		y.push_back((float)it->y);
	}
	int colStart = int(*min_element(x.begin(), x.end()) - distH2N);
	int lineStart = int(*min_element(y.begin(), y.end()) - distH2N);
	int colEnd = int(*max_element(x.begin(), x.end()) + distH2N);
	int lineEnd = int(*max_element(y.begin(), y.end()) + distH2N);
	colStart = max(0, colStart);
	lineStart = max(0, lineStart);
	colEnd = min(colEnd, cDepthWidth);
	lineEnd = min(lineEnd, cDepthHeight);

	unsigned short tmp = depth.at<unsigned short>(int(skeleton_vec[SpineBase].y), int(skeleton_vec[SpineBase].x));
	double min, max;
	Mat Cropped_Box, Mask_BG;

	Cropped_Box = cv::Mat(depth, cv::Rect(colStart, lineStart, (colEnd - colStart), (lineEnd - lineStart)));
	cv::minMaxLoc(Cropped_Box, &min, &max);
	float thresh = 500 * 8.0;
	thresh = 500.0;

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
	depth.copyTo(masked_depth, mask);
	Segment_part->Setparam(masked_depth, skeleton_vec);
	Segment_part->Run();
	Segment_part->Draw();

}

void SceneManager::Comupre3D(int cameranumber,Mat depthImage, Mat colorImage) {
	float K_forcal = 531.15;

	vector<Eigen::Vector4d> PointCloud;
	vector<Vec3b> PointCloudColor;
	Eigen::Vector4d pt(0.0, 0.0, 0.0, 1.0);
	double div = 8.0 * 1000;
	for (int i = 0; i < depthImage.size().height; i++)
	{
		for (int j = 0; j < depthImage.size().width; j++) {
			pt.z() = depthImage.at<unsigned short>(i, j)/div;
			pt.x() = pt.z() == 0.0 ? 0.0 : pt.z() * ((float(j) - Calib[2]) / Calib[0]);
			pt.y() = pt.z() == 0.0 ? 0.0 : pt.z() * ((float(i) - Calib[3]) / Calib[1]);
			pt.z() = pt.z() == 0.0 ? 0.0 : pt.z();

			if (pt.x() == 0.0&&pt.y() == 0.0 || pt.z() == 0.0)continue;
			auto pt_color = InputMng[cameranumber]->exColor2Depth * pt;
			Point2i pt_2d;
			pt_2d.x = int(pt_color.x() * K_forcal / pt_color.z() + Calib[2]);
			pt_2d.y = int(pt_color.y() * K_forcal / pt_color.z() + Calib[3]);

			if (pt_2d.y >= depthImage.size().height || pt_2d.x >= depthImage.size().width || pt_2d.x < 0 || pt_2d.y < 0) {
				continue;
			}
			else {
				Vec3b color = colorImage.at<Vec3b>(int(pt_2d.y), int(pt_2d.x));
				PointCloudColor.push_back(color);
			}
			pt = extrinsicsD2DEigen[cameranumber] * pt;
			PointCloud.push_back(pt);
		}
	}
	PointClouds.push_back(PointCloud);
	PointClouds_Color.push_back(PointCloudColor);
}

void SceneManager::Draw3D() {
	glPointSize(1.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < num_PC; i++)
	{
		for (int j = 0; j < PointClouds[i].size();j++)
		{
			glColor4ub(PointClouds_Color[i][j](2), PointClouds_Color[i][j](1), PointClouds_Color[i][j](0), 1.0);
			auto pt = PointClouds[i][j];
			//if (pt.y() < 0.5)continue;
			glVertex4d(pt.x(),-pt.y(),-pt.z(),1.0);
		}
	}
	glEnd();
}

bool CheckHumanfromSkeleton(vector<Vec2i> skeleton) {
	int fails = 0;
	for (auto pt : skeleton) {
		if (pt(0) <= 0 && pt(1) <= 0) {
			fails++;
			//return false;
		}
	}
	//cout << "fails : " << fails << endl;
	return fails < 2;
	return true;
}