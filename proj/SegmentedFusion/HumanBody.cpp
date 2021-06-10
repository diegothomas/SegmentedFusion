#include "HumanBody.h"

HumanBody::HumanBody()
{
	StitchBody = make_unique<Stitching>();
	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		isBodyPart[i] = false;
		BodyPartMask[i] = Mat::zeros(cDepthHeight, cDepthWidth, CV_8UC1);

	}
}

HumanBody::HumanBody(cl_context _context, cl_device_id _device,cl_mem _depthCL,cl_mem CLColor)
{
	context = _context;
	device = _device;
	depthCL = _depthCL;
	colorCL = CLColor;
}


HumanBody::~HumanBody()
{
}

void HumanBody::SetParams(Mat & depth, vector<Vec2i>& _skeleton_vec2, vector<Vec3d>& _skeleton_vec3, Mat colorMat)
{
	human_depth = depth.clone();
	color = colorMat.clone();

	if (skeleton_vec2.size() == 0) {
		for (auto &p : _skeleton_vec2)skeleton_vec2.push_back(p);
	}
	if (skeleton_vec3.size() == 0) {
		for (auto &p : _skeleton_vec3)skeleton_vec3.push_back(p);
	}
	
	skeleton_vec2_prev.clear(); skeleton_vec3_prev.clear();
	for (auto &p : skeleton_vec2)skeleton_vec2_prev.push_back(p);
	for (auto &p : skeleton_vec3)skeleton_vec3_prev.push_back(p);

	skeleton_vec2.clear(); skeleton_vec3.clear();
	for (auto &p : _skeleton_vec2)skeleton_vec2.push_back(p);
	for (auto &p : _skeleton_vec3)skeleton_vec3.push_back(p);



}

void HumanBody::SetParams(Segmentation &seg, vector<Vec2i>& _skeleton_vec2, vector<Vec3d>& _skeleton_vec3, Mat colorMat)
{
	human_depth = seg.GetHumanDeoth().clone();
	color = colorMat.clone();
	raw_depth = seg.depthImage_raw.clone();
	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		BodyPartMask[i] = seg.GetMaskImage(i).clone();
	}
	if (skeleton_vec2.size() == 0) {
		for (auto p : _skeleton_vec2) { skeleton_vec2.push_back(p); skeleton_vec2_prev.push_back(p); }
	}
	if (skeleton_vec3.size() == 0) {
		for (auto &p : _skeleton_vec3) {
			skeleton_vec3.push_back(p); skeleton_vec3_prev.push_back(p);
			auto tmp = getWorldPosition(p);
			skeleton_vec3_world.push_back(tmp);
			skeleton_vec3_world_prev.push_back(tmp);
		}
	}

	for (int i = 0; i < SkeletonNumber; i++)
	{
		auto worldjoint = getWorldPosition(skeleton_vec3[i]);
		if (skeleton_vec2[i](0) == 0 && skeleton_vec2[i](1) == 0 || skeleton_vec3[i](2)==0.0 || 
			(worldjoint - skeleton_vec3_world[i]).dot(worldjoint - skeleton_vec3_world[i]) > 5) {
			skeleton_vec2[i] = skeleton_vec2_prev[i];
			skeleton_vec3[i] = skeleton_vec3_prev[i];
			skeleton_vec3_world[i] = skeleton_vec3_world_prev[i];
			continue;
		}
		skeleton_vec2_prev[i] = skeleton_vec2[i];
		skeleton_vec3_prev[i] = skeleton_vec3[i];
		skeleton_vec3_world_prev[i] = worldjoint;
	}

	for (int i = 0; i < SkeletonNumber; i++)
	{
		if (_skeleton_vec2[i](0) == 0 && _skeleton_vec2[i](1) == 0) {
			continue;
		}
		skeleton_vec2[i] = _skeleton_vec2[i];
		skeleton_vec3[i] = _skeleton_vec3[i];

		skeleton_vec3_world[i] = getWorldPosition(_skeleton_vec3[i]);
	}

}

void HumanBody::SetPosParams(Eigen::Matrix4d position, Eigen::Matrix4d D2C,int c) {
	cameranumber = c;
	myBodyPosition = position;
	PoseD2C = D2C;
}

void HumanBody::Running()
{
	Mat VMap;
	StitchBody->SetDepth(human_depth, VMap);
	StitchBody->GetVBonesTrans(skeleton_vec3, skeleton_vec3_prev);
	float poseD2C[16] = {
		PoseD2C(0,0),PoseD2C(0,1),PoseD2C(0,2),PoseD2C(0,3),
		PoseD2C(1,0),PoseD2C(1,1),PoseD2C(1,2),PoseD2C(1,3),
		PoseD2C(2,0),PoseD2C(2,1),PoseD2C(2,2),PoseD2C(2,3),
		PoseD2C(3,0),PoseD2C(3,1),PoseD2C(3,2),PoseD2C(3,3),
	};
	// for each body part
	for (int j = 0; j < BODY_PART_NUMBER; j++) {
		int i = j;
		if (!isBodyPart[i]) {
			Init(i);
			if (!isBodyPart[i])continue;
			BodyParts[i]->Initialize(skeleton_vec3);
		}
		float bone[8], joint[8];
		StitchBody->GetJointInfo(i, bone, joint);

		if (bone[1]==0.0 && bone[2]==0.0&&bone[3] == 0.0)continue;

		// Body part mask
		Mat maskedDepthImage = Mat::zeros(raw_depth.size(),CV_8UC1);
		raw_depth.copyTo(maskedDepthImage, BodyPartMask[i] > 0);

		BodyParts[i]->SetD2C(poseD2C);
		BodyParts[i]->Running(maskedDepthImage, color, bone, joint, skeleton_vec3);
		//BodyParts[i]->Running(raw_depth,color, bone, joint, skeleton_vec3);
		BodyParts[i]->TransfomMesh(myBodyPosition);
	}

}


void HumanBody::Draw() {
	if (!isHumanBody)return;
	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		if (!existBodyPart[i])continue;
		BodyParts[i]->DrawMesh();
		BodyParts[i]->DrawBox();
	}
	DrawSkeleton();
	//Mat image_skeleton(cDepthHeight, cDepthWidth, CV_8UC4);
	//for (int i = 0; i < SkeletonNumber; i++)
	//{
	//	circle(image_skeleton, skeleton_vec2[i], 3, Scalar(0,1,0), -1);
	//}
	//imshow("skeleotns", image_skeleton>0);
	//waitKey(1);
}


bool HumanBody::Init(int i) {
	// PCA 
	vector<Vec3d> res;
	Mat a = BodyPartMask[i].clone();
	for (int i = 0; i < human_depth.size().width; i++)
	{
		for (int j = 0; j < human_depth.size().height; j++)
		{
			auto b = a.at<unsigned char>(j, i);
			if (b != 0 && human_depth.at<unsigned short>(j, i) != 0) {
				Vec3d pt(0, 0, 0);
				pt = GetProjectedPoint(Vec2(i, j), human_depth.at<unsigned short>(j, i));
				if (pt(2) > 0) {res.push_back(pt);}
			}
		}
	}
	if (res.size() > 0) {
		isBodyPart[i] = true;
		existBodyPart[i] = true;
		// make_unique
		BodyParts[i] = make_unique<BodyPart>(context, device, res, human_depth, depthCL,colorCL	, i);
		cout << "make body parts " << i << endl;
	}
	return true;
}


void HumanBody::DrawSkeleton() {
	float pt[3];
	Eigen::Vector4d vec;
	if (cameranumber == 0)glColor4f(1.0, 0.0, 0.0, 1.0);
	else if (cameranumber == 1)glColor4f(0.0, 1.0, 0.0, 1.0);
	else if (cameranumber == 2)glColor4f(0.0, 0.0, 1.0, 1.0);
	for (auto& i : skeleton_vec3)
	{
		vec(0) = i(0);
		vec(1) = i(1);
		vec(2) = i(2);
		vec(3) = 1.0;
		//vec = myBodyPosition * vec;
		glPointSize(8.0);
		glBegin(GL_POINTS);
		glVertex3f(vec(0), vec(1), vec(2));
		glEnd();
	}
}

void HumanBody::DrawSkeleton(vector<Vec3d> skeleton) {
	float pt[3];
	Eigen::Vector4d vec;
	if (cameranumber == 0)glColor4f(1.0, 0.0, 0.0, 1.0);
	else if (cameranumber == 1)glColor4f(0.0, 1.0, 0.0, 1.0);
	else if (cameranumber == 2)glColor4f(0.0, 0.0, 1.0, 1.0);
	for (auto& i : skeleton)
	{
		vec(0) = i(0);
		vec(1) = i(1);
		vec(2) = i(2);
		vec(3) = 1.0;
		vec = myBodyPosition * vec;

		glPointSize(8.0);
		glBegin(GL_POINTS);
		glVertex3f(vec(0), vec(1), vec(2));
		glEnd();
	}
}

void HumanBody::InitilizeHumanBody()
{
	StitchBody = make_unique<Stitching>();
	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		BodyPartMask[i] = Mat::zeros(cDepthHeight, cDepthWidth, CV_8UC1);
		isBodyPart[i] = false;
		existBodyPart[i] = false;
	}
	myBodyPosition = Eigen::MatrixXd::Identity(4,4);
	isInitialized = true;
}
