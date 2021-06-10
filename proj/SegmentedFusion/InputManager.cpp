#include "InputManager.h"
#include <sys\stat.h>



InputManager::InputManager(string datafilename)
{
	filename = datafilename;
	path = filename;
	fileCount = 0;
	depthImageCount = 0;
	skeletonCount = 0;
	skeletonCount_kinect = 0;
}


InputManager::~InputManager()
{
}

void InputManager::LoadData(int length, int firstFrame, string type)
{
	cout << "Now Loading ... " << endl;

	// skeleton save
	struct stat st;
	bool skeleton_exist = false;
	FileStorage fs("D:\\dataset\\Data_SegmentedFusion\\config\\extrinsic_" + type + ".xml", CV_STORAGE_READ);
	if (fs.isOpened()) {
		fs["rmat"] >> exColor2Depth_R;
		fs["tvec"] >> exColor2Depth_t;
	}
	fs.release();
	exColor2Depth(0,0) = exColor2Depth_R.at<double>(0, 0);
	exColor2Depth(0,1) = exColor2Depth_R.at<double>(0, 1);
	exColor2Depth(0,2) = exColor2Depth_R.at<double>(0, 2);
	exColor2Depth(0,3) = exColor2Depth_t.at<double>(0);
				   
	exColor2Depth(1,0) = exColor2Depth_R.at<double>(1, 0);
	exColor2Depth(1,1) = exColor2Depth_R.at<double>(1, 1);
	exColor2Depth(1,2) = exColor2Depth_R.at<double>(1, 2);
	exColor2Depth(1,3) = exColor2Depth_t.at<double>(1);
				   
	exColor2Depth(2,0) = exColor2Depth_R.at<double>(2, 0);
	exColor2Depth(2,1) = exColor2Depth_R.at<double>(2, 1);
	exColor2Depth(2,2) = exColor2Depth_R.at<double>(2, 2);
	exColor2Depth(2,3) = exColor2Depth_t.at<double>(2);
				   
	exColor2Depth(3,0) = 0.0;
	exColor2Depth(3,1) = 0.0;
	exColor2Depth(3,2) = 0.0;
	exColor2Depth(3,3) = 1.0;

	if (_mkdir((path + "skeletons").c_str()) == 0) {
		cout << "make dir : " << (path + "skeletons") << endl;
	}
	else {
		cout << "cannot make dir : " << (path + "skeletons") << endl;
		if (stat((path + "skeletons").c_str(), &st) == 0) {
			cout << "dir is exist" << endl;
			skeleton_exist = true;
		}
	}

	for (int i = firstFrame; i < firstFrame + length; i++)
	{
		char buf[100];

		// Load RGB Image 
		sprintf(buf, "color/color_%04d.png", i);
		Mat tmpIm = cv::imread(path + buf, CV_LOAD_IMAGE_UNCHANGED);
		if (!tmpIm.data) {
			cout << path + buf << " is load error" << "\r";
			tmpIm = Mat::zeros(cDepthHeight, cDepthWidth, CV_8UC3);
		}
		RGBImages.push_back(tmpIm.clone());

		// Load Depth Images 
		sprintf(buf, "depth/%04d.tiff", i);
		sprintf(buf, "depth/depthmap%d.png", i);
		sprintf(buf, "depth/depth_%d.png", i);
		//sprintf(buf, "depth/depth_%d.tiff", i);

		tmpIm = cv::imread(path + buf, CV_LOAD_IMAGE_UNCHANGED);
		DepthImages.push_back(tmpIm.clone());
		if (!tmpIm.data)cout << path + buf << " is load error\r";

		///////////////skeleton load kinect////////////////////////////
		sprintf_s(buf, "pos\\pos_%d.csv", i);
		if (!skeleton_exist)cout << buf << "\r";
		ifstream ifs(path + buf);
		if (!ifs) {
			vector<Vec2> tmp(20);
			skeleton_vec_kinect.push_back(tmp);
		}
		else {
			string line;
			isHumanBody = true;
			int skeletonNumber = 0;
			vector<Vec2> tmpSkeleton;
			while (getline(ifs, line)) {
				vector<string> strvec = Split(line, ',');
				Vec2 tmp(stod(strvec.at(0)), stod(strvec.at(1)));
				if (tmp.x < 0 || tmp.x >= cDepthWidth || tmp.y < 0 || tmp.y >= cDepthHeight) {
					tmp = { 0.0,0.0 };
				}
				tmpSkeleton.push_back(tmp);
				skeletonNumber++;
			}
			skeleton_vec_kinect.push_back(tmpSkeleton);
		}

		//////////////////////skeleton load  openpose ver//////////////////////////////////////

		if (true) {
			vector<Vec2f> skeleton_swap;
			vector<Vec3f> skeleton3d;
			for (auto& p : skeleton_vec_kinect[i]) {
				skeleton_swap.push_back(Vec2f(p.x, p.y));
				Point3f tmp;
				tmp.z = tmpIm.at<unsigned short>(p.y, p.x);
				tmp.x = tmp.z == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * tmp.z * ((float(p.x) - Calib[2]) / Calib[0]);
				tmp.y = tmp.z == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * tmp.z * ((float(p.y) - Calib[3]) / Calib[1]);
				tmp.z = tmp.z == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * tmp.z;
				if (tmp.z == 0.0) { tmp = Point3f(0, 0, 0); }
				skeleton3d.push_back(tmp);
			}
			skeleton_vec.push_back(skeleton_swap);
			skeleton_vec3.push_back(skeleton3d);
		}
		else if (skeleton_exist) {
			vector<Vec2f> tmpSkeleton;
			vector<Vec3f> tmpSkeleton_vec3;
			sprintf_s(buf, "skeletons\\pos%d.xml", i);
			cout << buf << "\r";
			FileStorage skeletonFS(path + buf, CV_STORAGE_READ);
			skeletonFS["tmpskeleton"] >> tmpSkeleton;
			skeletonFS["tmpskeleton_vec3"] >> tmpSkeleton_vec3;

			// set joints which is nothing in OpenPose skeleton
			while (tmpSkeleton_vec3.size() < SkeletonNumber) {
				tmpSkeleton_vec3.push_back(Vec3f(0.0, 0.0, 0.0));
			}
			tmpSkeleton_vec3[SpineShoulder] = (tmpSkeleton_vec3[ShoulderLeft] + tmpSkeleton_vec3[ShoulderRight]) / 2.0;
			tmpSkeleton_vec3[Neck] = (tmpSkeleton_vec3[SpineShoulder] + tmpSkeleton_vec3[Head]) / 2.0;
			tmpSkeleton_vec3[SpineMid] = (tmpSkeleton_vec3[Neck] + tmpSkeleton_vec3[SpineBase]) / 2.0;

			while (tmpSkeleton.size() < SkeletonNumber)
			{
				tmpSkeleton.push_back(Vec2i(0, 0));
			}
			tmpSkeleton[SpineShoulder] = (tmpSkeleton[ShoulderLeft] + tmpSkeleton[ShoulderRight]) / 2.0;
			tmpSkeleton[Neck] = (tmpSkeleton[SpineShoulder] + tmpSkeleton[Head]) / 2.0;
			tmpSkeleton[SpineMid] = (tmpSkeleton[Neck] + tmpSkeleton[SpineBase]) / 2.0;

			skeleton_vec.push_back(tmpSkeleton);
			skeleton_vec3.push_back(tmpSkeleton_vec3);

			skeletonFS.release();
		}
		else if (false) {
			vector<cv::Point2i> Function(cColorHeight*cColorWidth);
			ComputeMappingFunction(DepthImages[i - firstFrame], Function);
			Mapping.push_back(Function);

			///////////////skeleton load openpose/////////////////////////////	
			sprintf_s(buf, "pos_openpose\\pos%d.csv", i);
			ifstream ifs2(path + buf);
			sprintf_s(buf, "skeletons\\pos%d.xml", i);
			FileStorage skeletonFS(path + buf, CV_STORAGE_WRITE);
			if (!ifs2) {
				//cout << "Load error skeleton" << path + buf << endl;
				vector<Vec2f> tmpSkeleton(20);
				vector<Vec3f> tmpSkeleton_vec3(20);
				skeleton_vec.push_back(tmpSkeleton);
				skeleton_vec3.push_back(tmpSkeleton_vec3);

				skeletonFS << "tmpskeleton" << tmpSkeleton;
				skeletonFS << "tmpskeleton_vec3" << tmpSkeleton_vec3;
				skeletonFS << "Function" << Function;
			}
			else {
				string line;
				int skeletonNumber = 0;
				vector<Vec2f> tmpSkeleton;
				vector<Vec3f> tmpSkeleton_vec3;
				while (getline(ifs2, line)) {
					vector<string> strvec = Split(line, ',');
					Vec2f tmp(stod(strvec.at(0)), stod(strvec.at(1)));
					Vec3f tmp_vec3;
					int skeleton_pos_number = int(tmp(1)) * cColorWidth + int(tmp(0));
					tmp(0) = Function[skeleton_pos_number].x;
					tmp(1) = Function[skeleton_pos_number].y;

					if (tmp(0) < 0 || tmp(0) >= tmpIm.size().width || tmp(1) < 0 || tmp(1) >= tmpIm.size().height) {
						tmp = { 0.0,0.0 };
						cout << "depth = 0.0 @ InputMng->Load" << endl;
					}

					tmp_vec3(2) = tmpIm.at<unsigned short>(int(tmp(1)), int(tmp(0)));
					tmp_vec3(0) = tmp_vec3(2) == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * tmp_vec3(2) * ((float(tmp(0)) - Calib[2]) / Calib[0]);
					tmp_vec3(1) = tmp_vec3(2) == 0.0 ? 0.0 : -(Calib[9] / Calib[10]) * tmp_vec3(2) * ((float(tmp(1)) - Calib[3]) / Calib[1]);
					tmp_vec3(2) = tmp_vec3(2) == 0.0 ? 0.0 : -(Calib[9] / Calib[10]) * tmp_vec3(2);

					tmpSkeleton.push_back(tmp);
					tmpSkeleton_vec3.push_back(tmp_vec3);
					skeletonNumber++;
				}
				tmpSkeleton[SpineMid] = (tmpSkeleton[SpineBase] + tmpSkeleton[Neck]) / 2.0;
				skeleton_vec.push_back(tmpSkeleton);
				skeleton_vec3.push_back(tmpSkeleton_vec3);

				skeletonFS << "tmpskeleton" << tmpSkeleton;
				skeletonFS << "tmpskeleton_vec3" << tmpSkeleton_vec3;
				skeletonFS << "Function" << Function;

			}
			ifs2.close();
			skeletonFS.release();
		}
		else {
			vector<cv::Point2i> Function(cColorHeight*cColorWidth);
			ComputeMappingFunction(DepthImages[i - firstFrame], Function);
			Mapping.push_back(Function);
			//maybe this code, right & left is reverse 
			// correspond between kinect and openpose. 
			// joint pos : look at "main.h"
			std::vector<int> jointuse{ 8, 8, 1, 0, 2, 3, 4, 4, 5, 6, 7, 7, 9, 10, 11, 22, 12, 13, 14, 19 };
			std::vector<std::vector<cv::Vec3f>> joints3f;
			std::string camdirpath = path + "pos_openpose/";
			cv::Mat pose_0;

			sprintf_s(buf, "skeletons\\pos%d.xml", i);
			FileStorage skeletonFS(path + buf, CV_STORAGE_WRITE);
			sprintf_s(buf, "%04d", i);
			string fff = camdirpath + "color_" + buf + "_pose.yml";
			cv::FileStorage fs(fff, CV_STORAGE_READ);
			std::cout << fff + "\r";
			if (!fs.isOpened()) { std::cout << fff + " does not exist.\r"; exit(1); return; }
			fs["pose_0"] >> pose_0;
			std::vector<cv::Vec2f> joints3f2;
			if (pose_0.cols == 0) { for (int k = 0; k < jointuse.size(); k++) { joints3f2.push_back(cv::Vec2f(0, 0)); } }
			else {
				for (auto k : jointuse) {
					if (k == 0) {
						int count = 0;
						cv::Vec3f joint(0, 0, 0);
						std::vector<int> tmpidx{ 0,15,16,17,18 };
						for (int l = 0; l < 5; l++) {
							if (pose_0.at<cv::Vec3f>(tmpidx[l])[0] == 0) continue;
							joint += pose_0.at<cv::Vec3f>(tmpidx[l]);
							count++;
						}
						if (count == 0) joints3f2.push_back(cv::Vec2f(0, 0));
						else joints3f2.push_back(Vec2f(joint(0) / double(count), joint(1) / double(count)));
						continue;
					}
					auto a = pose_0.at<cv::Vec3f>(k);
					joints3f2.push_back(Vec2f(a(0),a(1)));
				}
			}
			fs.release();

			vector<Vec2f> tmpSkeleton;
			vector<Vec3f> tmpSkeleton_vec3;
			for(auto tmp: joints3f2){
				Vec3f tmp_vec3;
				int skeleton_pos_number = int(tmp(1)) * cColorWidth + int(tmp(0));
				if (skeleton_pos_number >= Function.size()) {
					tmp = { 0.0,0.0 };
				}
				else {
					tmp(0) = Function[skeleton_pos_number].x;
					tmp(1) = Function[skeleton_pos_number].y;
				}

				if (tmp(0) < 0 || tmp(0) >= tmpIm.size().width || tmp(1) < 0 || tmp(1) >= tmpIm.size().height) {
					tmp = { 0.0,0.0 };
				}

				tmp_vec3(2) = tmpIm.at<unsigned short>(int(tmp(1)), int(tmp(0)));
				if (tmp_vec3(2) == 0.0) {
					// search non zero pt

				}
				tmp_vec3(0) = tmp_vec3(2) == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * tmp_vec3(2) * ((float(tmp(0)) - Calib[2]) / Calib[0]);
				tmp_vec3(1) = tmp_vec3(2) == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * tmp_vec3(2) * ((float(tmp(1)) - Calib[3]) / Calib[1]);
				tmp_vec3(2) = tmp_vec3(2) == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * tmp_vec3(2);

				tmpSkeleton.push_back(tmp);
				tmpSkeleton_vec3.push_back(tmp_vec3);
			}

			// set joints which is nothing in OpenPose skeleton
			while (tmpSkeleton_vec3.size() < SkeletonNumber) {tmpSkeleton_vec3.push_back(Vec3f(0.0, 0.0, 0.0));}
			tmpSkeleton_vec3[SpineMid] = (tmpSkeleton_vec3[Neck] + tmpSkeleton_vec3[SpineBase]) / 2.0;
			tmpSkeleton_vec3[SpineShoulder] = (tmpSkeleton_vec3[ShoulderLeft] + tmpSkeleton_vec3[ShoulderRight]) / 2.0;
			tmpSkeleton_vec3[Neck] = (tmpSkeleton_vec3[SpineShoulder] + tmpSkeleton_vec3[Head]) / 2.0;
			
			while (tmpSkeleton.size() < SkeletonNumber){tmpSkeleton.push_back(Vec2i(0, 0));}
			tmpSkeleton[SpineMid] = (tmpSkeleton[Neck] + tmpSkeleton[SpineBase]) / 2.0;
			tmpSkeleton[SpineShoulder] = (tmpSkeleton[ShoulderLeft] + tmpSkeleton[ShoulderRight]) / 2.0;
			tmpSkeleton[Neck] = (tmpSkeleton[SpineShoulder] + tmpSkeleton[Head]) / 2.0;
			
			skeleton_vec.push_back(tmpSkeleton);
			skeleton_vec3.push_back(tmpSkeleton_vec3);

			skeletonFS << "tmpskeleton" << tmpSkeleton;
			skeletonFS << "tmpskeleton_vec3" << tmpSkeleton_vec3;
		}
	}

	cout << "Done" << endl;
}

bool InputManager::GetImage_color(cv::Mat & out)
{
	if (fileCount >= RGBImages.size())return false;
	out = RGBImages[fileCount].clone();
	fileCount++;
	return true;
}

bool InputManager::GetImage_depth(cv::Mat & out)
{
	if (depthImageCount >= DepthImages.size())return false;
	out = DepthImages[depthImageCount].clone();
	depthImageCount++;
	return true;
}

bool InputManager::GetImages(cv::Mat & colorout, Mat &depthout)
{
	if (!GetImage_color(colorout) || !GetImage_depth(depthout))return false;
	return true;
}

bool InputManager::GetSkeletonVector(vector<Vec2>& out)
{
	if (skeleton_vec[skeletonCount].size() == 0 || skeleton_vec.size() <= skeletonCount)return false;
	for (auto & pt : skeleton_vec[skeletonCount]) {
		if (pt(0) == 0.0 && pt(1) == 0.0) {
			out.push_back(Vec2(0.0, 0.0));
		}else out.push_back(Vec2(pt(0),pt(1)));
	}
	skeletonCount++;
	return true;
}

bool InputManager::GetSkeletonVector_kinect(vector<Vec2>& out)
{
	if (skeleton_vec_kinect[skeletonCount_kinect].size() == 0 || skeleton_vec.size() <= skeletonCount_kinect)return false;
	for (auto & pt : skeleton_vec_kinect[skeletonCount_kinect]) {
		if (pt.x == 0.0 && pt.y == 0.0) {
			out.push_back(Vec2(0.0, 0.0));
		}
		else out.push_back(pt);
	}
	skeletonCount_kinect++;
	return true;
}

void InputManager::DrawColorSkeleton() {
	if (skeletonCount >= skeleton_vec.size()-1) { cout << "ret" << endl; return; }
	cout<<"skeleton count :" << skeletonCount << endl;	

	cv::Mat matzero = RGBImages[skeletonCount].clone();
	cv::Mat matkinect = RGBImages[skeletonCount].clone();
	matkinect = DepthImages[skeletonCount].clone();
	matzero = DepthImages[skeletonCount].clone();

	skeletonCount_kinect = max(skeletonCount, skeletonCount_kinect);
	skeletonCount = max(skeletonCount, skeletonCount_kinect);


	for (int i = 0; i < SkeletonNumber; i++)
	{
		cv::circle(matzero, cv::Point(skeleton_vec[skeletonCount_kinect][i](0), skeleton_vec[skeletonCount_kinect][i](1)), 5, 255, -1);
		cv::circle(matkinect, cv::Point(skeleton_vec_kinect[skeletonCount_kinect][i].x, skeleton_vec_kinect[skeletonCount_kinect][i].y), 5, 255, -1);
	}

	cv::imshow("InputMng->Draw : openpose", matzero);
	cv::imshow("InputMng->Draw : kinect", matkinect);
	cv::waitKey(1);
}

void InputManager::StreamSkeletonVideo() {

	for (int j = 0; j < skeleton_vec.size(); j++)
	{
		cv::Mat MatOP_RGB = RGBImages[j].clone();
		cv::Mat MatOP_Depth = DepthImages[j].clone();
		cv::Mat MatKinect_RGB = RGBImages[j].clone();
		cv::Mat MatKinect_Depth = DepthImages[j].clone();
		for (int i = 0; i < SkeletonNumber; i++)
		{
			cv::circle(MatOP_RGB, cv::Point(skeleton_vec[j][i](0), skeleton_vec[j][i](1)), 5, cv::Scalar(0, 10*i, 0), -1);
			cv::circle(MatOP_Depth, cv::Point(skeleton_vec[j][i](0), skeleton_vec[j][i](1)), 5, 255, -1);
			cv::circle(MatKinect_RGB, cv::Point(skeleton_vec_kinect[j][i].x, skeleton_vec_kinect[j][i].y), 5, cv::Scalar(0, 10 * i, 0), -1);
			cv::circle(MatKinect_Depth, cv::Point(skeleton_vec_kinect[j][i].x, skeleton_vec_kinect[j][i].y), 5, 255, -1);
		}
		cv::imshow("InputMng->Stream : openposeRGB", MatOP_RGB);
		cv::imshow("InputMng->Stream : openposeDepth", MatOP_Depth);
		cv::imshow("InputMng->Stream : KinectRGB", MatKinect_RGB);
		cv::imshow("InputMng->Stream : KinectDepth", MatKinect_Depth);
		cv::waitKey(10);
	}
}

void InputManager::ComputeMappingFunction(Mat &depth, vector<Point> &output) {

	float K_forcal = 531.15; //foral length of color camera
	//float K_forcal = 1081.37;
	for (int i = 0; i < cDepthWidth; i++)
	{
		for (int j = 0; j < cDepthHeight; j++)
		{
			// tmp = 3D point at depth
			Point3f tmp;
			tmp.z = depth.at<unsigned short>(j, i);
			tmp.x = tmp.z == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * tmp.z * ((float(i) - Calib[2]) / Calib[0]);
			tmp.y = tmp.z == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * tmp.z * ((float(j) - Calib[3]) / Calib[1]);
			tmp.z = tmp.z == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * tmp.z;
			if (tmp.z == 0.0) { continue; }
			// Transform
			Eigen::Matrix<double, 3, 3> rmat_eigen;
			Mat rmat = exColor2Depth_R;
			rmat_eigen << rmat.at<double>(0, 0), rmat.at<double>(0, 1), rmat.at<double>(0, 2),
				rmat.at<double>(1, 0), rmat.at<double>(1, 1), rmat.at<double>(1, 2),
				rmat.at<double>(2, 0), rmat.at<double>(2, 1), rmat.at<double>(2, 2);
			Mat tvec = exColor2Depth_t;
			Eigen::Vector3d newpoint = rmat_eigen * Eigen::Vector3d(tmp.x, tmp.y, tmp.z) + Eigen::Vector3d(tvec.at<double>(0), tvec.at<double>(1), tvec.at<double>(2));
			tmp = Point3f(newpoint.x(), newpoint.y(), newpoint.z());

			// Reprojected point of tmp
			Point2i tmp_color;
			tmp_color.x = tmp.x * K_forcal / tmp.z + cColorWidth/2;
			tmp_color.y = tmp.y * K_forcal / tmp.z + cColorHeight/2;
			if (tmp_color.y >= cColorHeight || tmp_color.x >= cColorWidth || tmp_color.x < 0 || tmp_color.y < 0) {
				//output[int(tmp_color.y) * cColorWidth + int(tmp_color.x)] = Point2i(0,0);
			}
			else {
				output[int(tmp_color.y) * cColorWidth + int(tmp_color.x)] = Point2i(i, j);
			}
		}
	}

}




bool InputManager::GetCurrentSkeleton(vector<Vec2i>& output)
{
	if (skeletonCount >= skeleton_vec.size()) { return false; }
	output.clear();
	for (auto ske : skeleton_vec[skeletonCount]) {
		output.push_back(ske);
	}
	skeletonCount++;
	return true;
}

bool InputManager::GetCurrentSkeleton(vector<Eigen::Vector2i>& output)
{
	if (skeletonCount >= skeleton_vec.size()) { return false; }
	output.clear();
	for (auto ske : skeleton_vec[skeletonCount]) {
		output.push_back(Eigen::Vector2i(int(ske(0)), int(ske(1))));
	}
	skeletonCount++;
	return true;
}

bool InputManager::GetCurrentSkeleton(vector<Vec2i>& output, vector<Vec3d>& output3d)
{
	if (skeletonCount >= skeleton_vec.size()) { return false; }
	output.clear();
	output3d.clear();
	for (auto ske : skeleton_vec[skeletonCount]) {
		output.push_back(ske);
	}
	for (auto ske : skeleton_vec3[skeletonCount]) {
		output3d.push_back(ske);
	}
	skeletonCount++;
	return true;
}

bool InputManager::GetCurrentSkeleton(vector<Vec2>& output, vector<Vec3>& output3d)
{
	if (skeletonCount >= skeleton_vec.size()) { return false; }
	output.clear();
	output3d.clear();
	for (auto ske : skeleton_vec[skeletonCount]) {
		output.push_back(Vec2i(ske(0),ske(1)));
	}
	for (auto ske : skeleton_vec3[skeletonCount]) {
		output3d.push_back(Vec3d(ske(0), ske(1),ske(2)));
	}
	skeletonCount++;
	return true;
}

bool InputManager::GetData(Mat &color, Mat &depth, vector<Eigen::Vector2i> &skeleton) {
	bool res = GetImages(color, depth);
	res = res * GetCurrentSkeleton(skeleton);
	return res;
}

bool InputManager::GetData(Mat &color, Mat &depth, vector<Vec2i> &skeleton) {
	bool res = GetImages(color, depth);
	res = res * GetCurrentSkeleton(skeleton);
	return res;
}

bool InputManager::GetData(Mat &color, Mat &depth, vector<Vec2i> &skeleton, vector<Vec3d> &skeleton_vec3) {
	bool res = GetImages(color, depth);
	res = res * GetCurrentSkeleton(skeleton, skeleton_vec3);
	//skeletonCount--; depthImageCount--; fileCount--;
	return res;
}


bool InputManager::GetData(Mat &color, Mat &depth, vector<Vec2> &skeleton, vector<Vec3> &skeleton_vec3) {
	bool res = GetImages(color, depth);
	res = res * GetCurrentSkeleton(skeleton, skeleton_vec3);
	return res;
}



//マウス入力用のパラメータ
struct mouseParam {
	int x;
	int y;
	int event;
	int flags;
};
//コールバック関数
void CallBackFunc(int eventType, int x, int y, int flags, void* userdata)
{
	mouseParam *ptr = static_cast<mouseParam*> (userdata);

	ptr->x = x;
	ptr->y = y;
	ptr->event = eventType;
	ptr->flags = flags;
}

void InputManager::sampleSolvePnP()
{
	mouseParam mouseEvent,mouseEvent_Depth;
	vector<Point2f> colorPointVector;
	vector<Point3f> depthPointVEctor;

	//入力画像
	cv::Mat input_img = RGBImages[0].clone();
	cv::Mat input_depth = DepthImages[0].clone();

	//表示するウィンドウ名
	cv::String showing_name = "input";
	cv::String showing_name_depth = "depth";

	//画像の表示
	cv::imshow(showing_name, input_img);
	cv::imshow(showing_name_depth, input_depth);

	//コールバックの設定
	cv::setMouseCallback(showing_name, CallBackFunc, &mouseEvent);
	cv::setMouseCallback(showing_name_depth, CallBackFunc, &mouseEvent_Depth);

	bool flgBtnDown = false;
	bool flgBtnDown_depth = false;
	while (1) {
		cv::imshow(showing_name, input_img);
		cv::imshow(showing_name_depth, input_depth);
		cv::waitKey(1);
		//左クリックがあったら表示
		if (mouseEvent.event == cv::EVENT_LBUTTONDOWN) {
			//クリック後のマウスの座標を出力
			//cout << "down";
			flgBtnDown = true;
		}
		if (mouseEvent.event == cv::EVENT_LBUTTONUP ) {
			//cout << "up";
			if (flgBtnDown) {
				cout << "color point : " << colorPointVector.size() << " / ";
				std::cout << mouseEvent.x << " , " << mouseEvent.y << std::endl;
				flgBtnDown = false;	
				colorPointVector.push_back(Point2f(mouseEvent.x, mouseEvent.y));
				circle(input_img, Point2i(mouseEvent.x, mouseEvent.y), 2, 0);
			}
		}

		//左クリックがあったら表示
		if (mouseEvent_Depth.event == cv::EVENT_LBUTTONDOWN) {
			//クリック後のマウスの座標を出力
			//cout << "down";
			flgBtnDown_depth = true;
		}
		if (mouseEvent_Depth.event == cv::EVENT_LBUTTONUP) {
			//cout << "up";
			if (flgBtnDown_depth) {
				flgBtnDown_depth = false;

				Point3f tmp;
				int i = mouseEvent_Depth.y;
				int j = mouseEvent_Depth.x;
				tmp.z = input_depth.at<unsigned short>(i,j);
				tmp.x = tmp.z == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * tmp.z * ((float(j) - Calib[2]) / Calib[0]);
				tmp.y = tmp.z == 0.0 ? 0.0 : -(Calib[9] / Calib[10]) * tmp.z * ((float(i) - Calib[3]) / Calib[1]);
				tmp.z = tmp.z == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * tmp.z;
				//if (tmp.x== 0.0&&tmp.y == 0.0 || tmp.z == 0.0)continue;


				cout << "depth point : " << depthPointVEctor.size() << " / ";
				std::cout << mouseEvent_Depth.x << " , " << mouseEvent_Depth.y << std::endl;
				depthPointVEctor.push_back(tmp);

				circle(input_depth, Point2i(j,i), 2, 0);

			}
		}


		//右クリックがあったら終了
		if (mouseEvent.event == cv::EVENT_RBUTTONDOWN) {
			if (colorPointVector.size() != depthPointVEctor.size()) {
				cout << "num of point is different (color , depth) : ( " << colorPointVector.size() << "," << depthPointVEctor.size() << ")" << endl;
				continue;
			}
			break;
		}
	}


	cv::Mat K = (cv::Mat_<double>(3, 3) << 531.15, 0, Calib[2], 0, 531.15, Calib[3], 0, 0, 1);
	cv::Mat distortion = (cv::Mat_<double>(4, 1) << 0, 0, 0, 0);
	cv::Mat rvec_est;
	cv::Mat tvec_est;
	Mat rmat = cv::Mat_<float>(3,3);
	cv::Mat tmat = Mat::zeros(3, 3, CV_32F);

	cv::solvePnP(depthPointVEctor, colorPointVector, K, distortion, rvec_est, tvec_est);
	Rodrigues(rvec_est, rmat);
	Rodrigues(tvec_est, tmat);
	cout << rmat << endl;
	cout << tvec_est << endl;

	Eigen::Matrix<float, 3, 3> rmat_eigen;
	rmat_eigen << rmat.at<double>(0, 0), rmat.at<double>(1, 0), rmat.at<double>(2, 0),
		rmat.at<double>(0, 1), rmat.at<double>(1, 1), rmat.at<double>(1, 2),
		rmat.at<double>(0, 2), rmat.at<double>(1, 2), rmat.at<double>(2, 2);

	//vector<Point2f> vector_color_project;
	//vector<Point3f> vector_depth_project;
	Mat reproject_Mat = Mat::zeros(input_depth.size(), CV_8UC3);

	int out_count = 0;
	for (int i = 0; i < input_depth.size().height; i++)
	{
		for (int j = 0; j < input_depth.size().width; j++)
		{
			Point3f tmp;
			tmp.z = input_depth.at<unsigned short>(i, j);
			//tmp.x = tmp.z == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * tmp.z * ((float(j) - Calib[2]) / Calib[0]);
			//tmp.y = tmp.z == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * tmp.z * ((float(i) - Calib[3]) / Calib[1]);
			//tmp.z = tmp.z == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * tmp.z;
			tmp.x = tmp.z == 0.0 ? 0.0 : tmp.z * ((float(j) - Calib[2]) / Calib[0]);
			tmp.y = tmp.z == 0.0 ? 0.0 : -tmp.z * ((float(i) - Calib[3]) / Calib[1]);
			tmp.z = tmp.z == 0.0 ? 0.0 : tmp.z;
			//cv2eigen(rmat,rmat_eigen);

			
			Eigen::Vector3f newpoint = rmat_eigen * Eigen::Vector3f(tmp.x,tmp.y,tmp.z) - Eigen::Vector3f(tvec_est.at<double>(0), tvec_est.at<double>(1), tvec_est.at<double>(2));

			tmp = Point3f(newpoint.x(), newpoint.y(),newpoint.z());

			if (tmp.z == 0.0) {  continue; }
			//reprojdection
			tmp.x = tmp.x * Calib[0] / tmp.z + Calib[2];
			tmp.y = tmp.y * Calib[1] / tmp.z + Calib[3];

			if (tmp.y > input_depth.size().height || tmp.x > input_depth.size().width || tmp.x < 0 || tmp.y < 0) {
				//cout << "out of range" << tmp << endl;
				out_count++;
				continue; 
			}

			reproject_Mat.at<Vec3b>(int(tmp.y), int(tmp.x))[0] = tmp.z;
			reproject_Mat.at<Vec3b>(int(tmp.y), int(tmp.x))[1] = tmp.z;
			reproject_Mat.at<Vec3b>(int(tmp.y), int(tmp.x))[2] = tmp.z;

		}
	}
	cout << "num of out : " << out_count << endl;
	imshow("proj", reproject_Mat);

	cv::waitKey(0);


}
