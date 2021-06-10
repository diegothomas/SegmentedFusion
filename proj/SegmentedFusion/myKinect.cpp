#include"myKinect.h"

KINECT::KINECT() :
	pKinectSensor(nullptr),
	pCoordinateMapper(nullptr),
	pColorFrameReader(nullptr),
	pDepthFrameReader(NULL)
{
	colorWidth = 1920 / color_resizescale;
	colorHeight = 1080 / color_resizescale;
	colorBufferSize = 1920 * 1080 * sizeof(RGBQUAD); //original datasize

	depthWidth = 512;
	depthHeight = 424;
	depthBufferSize = depthWidth * depthHeight * sizeof(UINT16);

	colorImage = cv::Mat::zeros(colorHeight, colorWidth, CV_8UC4);
	depthImage = cv::Mat::zeros(depthHeight, depthWidth, CV_16UC1);
	depthImage_forshow = cv::Mat::zeros(depthHeight, depthWidth, CV_8UC1);


}


KINECT::~KINECT() {
	end();
}


void KINECT::end() {
	SafeRelease(pCoordinateMapper);
	SafeRelease(pDepthFrameReader);

	if (pKinectSensor)
	{
		pKinectSensor->Close();
		pKinectSensor->Release();
	}
}


void KINECT::initialize() {

	HRESULT hr;

	colorImage = cv::Mat::zeros(1080, 1920, CV_8UC4);
	depthImage = cv::Mat::zeros(depthHeight, depthWidth, CV_16UC1);
	depthImage.convertTo(depthImage_forshow, CV_8UC1, 255.0f / 8000.0f, 0); // Create a depth image for show

	// Default
	hr = GetDefaultKinectSensor(&pKinectSensor);
	if (FAILED(hr)) {
		throw std::runtime_error("error : GetDefaultKinectSensor");
	}

	hr = pKinectSensor->Open();
	if (FAILED(hr)) {
		throw std::runtime_error("Error : Open()");
	}

	// Coordinate Mapper
	hr = pKinectSensor->get_CoordinateMapper(&pCoordinateMapper);
	if (FAILED(hr)) {
		throw std::runtime_error("Error : get_CoordinateMapper");
	}

	// Color
	IColorFrameSource* pColorFrameSource;
	hr = pKinectSensor->get_ColorFrameSource(&pColorFrameSource);
	if (FAILED(hr)) {
		throw std::runtime_error("Error : get_ColorFrameSource");
	}
	hr = pColorFrameSource->OpenReader(&pColorFrameReader);
	if (FAILED(hr)) {
		throw std::runtime_error("Error : OpenReader");
	}

	// Depth
	IDepthFrameSource* pDepthFrameSource;
	hr = pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);
	if (FAILED(hr)) {
		throw std::runtime_error("Error : get_DepthFrameSource");
	}
	hr = pDepthFrameSource->OpenReader(&pDepthFrameReader);
	if (FAILED(hr)) {
		throw std::runtime_error("Error : OpenReader");
	}

	// Skeleton
	IBodyFrameSource* pBodyFrameSource;
	hr = pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
	if (FAILED(hr)) {
		throw std::runtime_error("Error : get_BodyFrameSource");
	}
	hr = pBodyFrameSource->OpenReader(&pBodyFrameReader);
	if (FAILED(hr)) {
		throw std::runtime_error("Error : OpenReader");
	}


	// Get depth intrinsics (each camera returns unique intrinsic values)
	CameraIntrinsics depth_intrinsics;
	hr = pCoordinateMapper->GetDepthCameraIntrinsics(&depth_intrinsics);
	if (SUCCEEDED(hr)) {
		while (depth_intrinsics.FocalLengthX == 0)	pCoordinateMapper->GetDepthCameraIntrinsics(&depth_intrinsics);
		std::cout << "fx: " << depth_intrinsics.FocalLengthX << std::endl;
		std::cout << "fy: " << depth_intrinsics.FocalLengthY << std::endl;
		std::cout << "cx: " << depth_intrinsics.PrincipalPointX << std::endl;
		std::cout << "cy: " << depth_intrinsics.PrincipalPointY << std::endl;
		K_depth = (cv::Mat_<double>(3, 3) << depth_intrinsics.FocalLengthX, 0.0, depth_intrinsics.PrincipalPointX, 0.0, depth_intrinsics.FocalLengthY, depth_intrinsics.PrincipalPointY, 0.0, 0.0, 1.0);
	}


	// Unique id
	WCHAR uniqueID[256] = L"";
	hr = pKinectSensor->get_UniqueKinectId(256, uniqueID);
	std::wstring kinect_wuid(uniqueID);
	std::string kinect_uid(kinect_wuid.begin(), kinect_wuid.end());
	std::cout << kinect_uid << std::endl;
	serialno = "v2_" + kinect_uid;


}


void KINECT::showProfile() {
	std::cout << "Color image size: " << colorWidth << "x" << colorHeight << std::endl;
	std::cout << "Depth image size: " << depthWidth << "x" << depthHeight << std::endl;
	std::cout << "K_color:\n" << K_color << std::endl;
	std::cout << "D_color:\n" << D_color << std::endl;
	std::cout << "K_depth:\n" << K_depth << std::endl;
	std::cout << "D_depth:\n" << D_depth << std::endl;
	std::cout << "Rt_color2depth:\n" << Rt_color2depth << std::endl;

}

void KINECT::getCurrentData(cv::Mat& _color, cv::Mat& _depth, std::vector<cv::Vec2i> &skeleton2d, std::vector<cv::Vec3d> &skeleton3d)
{
	_color = colorImage.clone();
	_depth = depthImage.clone();
	//_depth = depthImage_forshow.clone();
	skeleton2d.clear();
	skeleton3d.clear();
	for (auto& p : skeletonJoints) {
		skeleton3d.push_back(cv::Vec3d(p.x,-p.y,p.z));
	}
	for (auto& p : skeletonJoints2d) {
		skeleton2d.push_back(p);
	}

}

void KINECT::update() {

	HRESULT hr;

	// Update color frame
	IColorFrame* pColorFrame;
	hr = pColorFrameReader->AcquireLatestFrame(&pColorFrame);
	if (SUCCEEDED(hr)) {
		cv::Mat color_tmp(1080, 1920, CV_8UC4);
		hr = pColorFrame->CopyConvertedFrameDataToArray(colorBufferSize, reinterpret_cast<BYTE*>(color_tmp.data), ColorImageFormat::ColorImageFormat_Bgra);
		if (FAILED(hr)) { throw std::runtime_error("Error : CopyConvertedFrameDataToArray"); }
		cv::cvtColor(color_tmp, color_tmp, CV_BGRA2BGR); //Convert 8UC4 to 8UC3
		cv::resize(color_tmp, colorImage, cv::Size(), 1.0 / color_resizescale, 1.0 / color_resizescale);
	}

	SafeRelease(pColorFrame);


	// Update depth frame
	IDepthFrame* pDepthFrame;
	hr = pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);
	if (SUCCEEDED(hr)) {
		cv::Mat depth_tmp(depthHeight, depthWidth, CV_16UC1);
		hr = pDepthFrame->AccessUnderlyingBuffer(&depthBufferSize, reinterpret_cast<UINT16**>(&depth_tmp.data));
		if (FAILED(hr)) { throw std::runtime_error("Error : CopyFrameDataToArray"); }
		depthImage = depth_tmp.clone();
		depthImage.convertTo(depthImage_forshow, CV_8UC1, 255.0f / 8000.0f, 0); // Create a depth image for show
	}

	SafeRelease(pDepthFrame);


	//Update body frame
	IBodyFrame* pBodyFrame;
	hr = pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
	if (SUCCEEDED(hr)) {
		IBody* pBody[BODYCOUNT] = { 0 };
		hr = pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, pBody);
		skeletonJoints.clear();
		skeleton_tracked = false;
		for (int i = 0; i < BODYCOUNT; i++) {
			hr = pBody[i]->get_IsTracked(&skeleton_tracked);
			if (SUCCEEDED(hr) && skeleton_tracked) {
				skeleton_tracked = true;
				Joint joints[JointType::JointType_Count];
				hr = pBody[i]->GetJoints(JointType::JointType_Count, joints);
				for (int j = 0; j < JointType::JointType_Count; j++) {
					Vector4 tmpjoint = { joints[j].Position.X, joints[j].Position.Y, joints[j].Position.Z, joints[j].TrackingState };
					skeletonJoints.push_back(tmpjoint);
				}
				break;
			}
		}
		if (!skeleton_tracked) {
			Vector4 zero = { JointType::JointType_Count, 0.0, 0.0, 0.0 };
			for (int i = 0; i < JointType::JointType_Count; i++) { skeletonJoints.push_back(zero); }
		}
	}
	skeletonJoints2d.clear();
	for (int i = 0; i < JointType::JointType_Count; i++) {
		DepthSpacePoint depthSpacePoint = { 0 };
		pCoordinateMapper->MapCameraPointToDepthSpace({ skeletonJoints[i].x, skeletonJoints[i].y, skeletonJoints[i].z }, &depthSpacePoint);
		skeletonJoints2d.push_back(cv::Vec2i(depthSpacePoint.X, depthSpacePoint.Y));
	}
	SafeRelease(pBodyFrame);
}


void KINECT::show() {
	colorImage_forshow = colorImage.clone();
	if (show_skeleton && skeleton_tracked) { // Just for show (do not save)
		for (int i = 0; i < JointType::JointType_Count; i++) {
			ColorSpacePoint colorSpacePoint = { 0 }; DepthSpacePoint depthSpacePoint;
			pCoordinateMapper->MapCameraPointToColorSpace({ skeletonJoints[i].x, skeletonJoints[i].y, skeletonJoints[i].z }, &colorSpacePoint);
			pCoordinateMapper->MapCameraPointToDepthSpace({ skeletonJoints[i].x, skeletonJoints[i].y, skeletonJoints[i].z }, &depthSpacePoint);
			if (skeletonJoints[i].w == 2) {
				cv::circle(colorImage_forshow, cv::Point(colorSpacePoint.X / color_resizescale, colorSpacePoint.Y / color_resizescale), 5, cv::Scalar(0, 255, 0), -1);
				cv::circle(depthImage_forshow, cv::Point(depthSpacePoint.X, depthSpacePoint.Y), 5, 0, -1);
			}
			if (skeletonJoints[i].w == 1) {
				cv::circle(colorImage_forshow, cv::Point(colorSpacePoint.X / color_resizescale, colorSpacePoint.Y / color_resizescale), 5, cv::Scalar(255, 0, 255), -1);
				cv::circle(depthImage_forshow, cv::Point(depthSpacePoint.X, depthSpacePoint.Y), 5, 0, -1);
			}
		}
	}
	cv::imshow("color", colorImage_forshow);
	cv::imshow("depth", depthImage_forshow);
}


void KINECT::bufferImg() {

	colorBuffer.push_back(colorImage.clone());
	depthBuffer.push_back(depthImage.clone());
	skeletonBuffer.push_back(skeletonJoints);

}


void KINECT::clearBuffer() {

	// clear all buffer
	colorBuffer.clear();
	depthBuffer.clear();
	colorBuffer.shrink_to_fit();
	depthBuffer.shrink_to_fit();
	//colorTSBuffer.clear();
	//depthTSBuffer.clear();
	//colorTSBuffer.shrink_to_fit();
	//depthTSBuffer.shrink_to_fit();
}


void KINECT::saveImgs(std::string folderpath) {

	// save all imgs in the buffer
	for (int i = 0; i < colorBuffer.size(); i++) {
		cv::imwrite(folderpath + "color_" + std::to_string(i) + ".png", colorBuffer[i]);
		cv::imwrite(folderpath + "depth_" + std::to_string(i) + ".tiff", depthBuffer[i]);
	}

	// save skeletons
	for (int i = 0; i < skeletonBuffer.size(); i++) {
		std::ofstream ofs(folderpath + "skeleton_" + std::to_string(i) + ".csv");
		for (int j = 0; j < JointType::JointType_Count; j++) {
			ofs << skeletonBuffer[i][j].x << "," << skeletonBuffer[i][j].y << "," << skeletonBuffer[i][j].z << "," << skeletonBuffer[i][j].w << "\n";
		}
		ofs.close();
	}

	// save profiles
	std::ofstream ofs(folderpath + "profiles.txt");
	ofs << serialno << "\n";
	ofs << int(colorBuffer.size()) << "\n";
	ofs << "K_color " << K_color.at<double>(0, 0) << " " << K_color.at<double>(1, 1) << " " << K_color.at<double>(0, 2) << " " << K_color.at<double>(1, 2) << "\n";
	ofs << "D_color " << D_color.at<double>(0, 0) << " " << D_color.at<double>(0, 1) << " " << D_color.at<double>(0, 2) << " " << D_color.at<double>(0, 3) << " " << D_color.at<double>(0, 4) << "\n";
	ofs << "K_depth " << K_depth.at<double>(0, 0) << " " << K_depth.at<double>(1, 1) << " " << K_depth.at<double>(0, 2) << " " << K_depth.at<double>(1, 2) << "\n";
	ofs << "D_depth " << D_depth.at<double>(0, 0) << " " << D_depth.at<double>(0, 1) << " " << D_depth.at<double>(0, 2) << " " << D_depth.at<double>(0, 3) << " " << D_depth.at<double>(0, 4) << "\n";
	ofs << "Rt_C2D " << Rt_color2depth.at<double>(0, 0) << " " << Rt_color2depth.at<double>(0, 1) << " " << Rt_color2depth.at<double>(0, 2) << " " << Rt_color2depth.at<double>(0, 3) << " "
		<< Rt_color2depth.at<double>(1, 0) << " " << Rt_color2depth.at<double>(1, 1) << " " << Rt_color2depth.at<double>(1, 2) << " " << Rt_color2depth.at<double>(1, 3) << " "
		<< Rt_color2depth.at<double>(2, 0) << " " << Rt_color2depth.at<double>(2, 1) << " " << Rt_color2depth.at<double>(2, 2) << " " << Rt_color2depth.at<double>(2, 3) << " "
		<< Rt_color2depth.at<double>(3, 0) << " " << Rt_color2depth.at<double>(3, 1) << " " << Rt_color2depth.at<double>(3, 2) << " " << Rt_color2depth.at<double>(3, 3) << "\n";
	ofs.close();

	// clear all buffer
	clearBuffer();


}
