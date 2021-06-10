#include "BodyPart.h"



BodyPart::BodyPart()
{
}

BodyPart::BodyPart(cl_context _context, cl_device_id _device,vector<Vec3> _points, cv::Mat im,cv::Mat depth,  cl_mem _depthCL, cl_mem _colorCL, int bdynum,int miss)
{
	myBodyNumber = bdynum;
	device = _device;
	context = _context;
	colorCL = _colorCL;
	depthCL = _depthCL;
	for (auto it = _points.begin(); it != _points.end(); ++it)
	{
		pts.push_back(*it);
	}

	depthimage = depth.clone();
	//maskimage = im.clone();
	eigen_val[0] = 0.0;
	eigen_val[1] = 0.0;
	eigen_val[2] = 0.0;
	this->miss = miss;

}
BodyPart::BodyPart(cl_context _context, cl_device_id _device, vector<Vec3d> _points, cv::Mat depth, cl_mem _depthCL,cl_mem _colorCL, int bdynum, int miss)
{
	myBodyNumber = bdynum;
	device = _device;
	context = _context;
	depthCL = _depthCL;
	colorCL = _colorCL;
	for (auto it = _points.begin(); it != _points.end(); ++it)
	{
		pts.push_back(*it);
	}

	depthimage = depth.clone();
	eigen_val[0] = 0.0;
	eigen_val[1] = 0.0;
	eigen_val[2] = 0.0;
	this->miss = miss;
	

}

BodyPart::~BodyPart()
{
	//if(TSDF!=NULL)delete TSDF;
}

void BodyPart::Initialize(vector<Vec3d>& skeVec)
{
	skeletonVec3.clear();
	for (auto it = skeVec.begin(); it != skeVec.end(); ++it)
	{
		skeletonVec3.push_back(*it);
	}
	if (miss == 0)getOrientation();
	//if (miss == 0)getOrientation_Bone();

	/*change format*/
	float pose[16] = {
		p1.x,p2.x,p3.x,center.x,
		p1.y,p2.y,p3.y,center.y,
		p1.z,p2.z,p3.z,center.z,
		0.0,0.0,0.0,1.0 };

	allPose(0, 0) = p1.x;
	allPose(1, 0) = p1.y;
	allPose(2, 0) = p1.z;
	allPose(3, 0) = 0.0;

	allPose(0, 1) = p2.x;
	allPose(1, 1) = p2.y;
	allPose(2, 1) = p2.z;
	allPose(3, 1) = 0.0;

	allPose(0, 2) = p3.x;
	allPose(1, 2) = p3.y;
	allPose(2, 2) = p3.z;
	allPose(3, 2) = 0.0;

	allPose(0, 3) = center.x;
	allPose(1, 3) = center.y;
	allPose(2, 3) = center.z;
	allPose(3, 3) = 0.0;

	TSDF = make_unique< TSDFManager>(context, device, Vec3(TSDF_VOX_SIZE, TSDF_VOX_SIZE, TSDF_VOX_SIZE), BodySize, intrinsic, pose, depthCL, colorCL, depthimage, myBodyNumber);

}

void BodyPart::Running(cv::Mat depth, Mat color, float boneDQ[8], float jointDQ[8], vector<Vec3d> &skeVec) {
	vector<Vec3>tmpskeleton;
	for (auto it = skeVec.begin(); it != skeVec.end(); ++it)
	{
		tmpskeleton.push_back(*it);
	}
	Running(depth, color,boneDQ, jointDQ, tmpskeleton);
}

void BodyPart::Running(cv::Mat depth, Mat color,float boneDQ[8], float jointDQ[8], vector<Vec3> &skeVec) {
	depthimage = depth.clone();
	skeletonVec3.clear();
	for (auto it = skeVec.begin(); it != skeVec.end(); ++it)
	{
		skeletonVec3.push_back(*it);
	}
	if(miss==0)setPlaneF();
	miss++;
	setPlaneF();
	//TSDF->Fuse_RGBD_GPU(depthimage, color, boneDQ, jointDQ, planeFunction);
	TSDF->TSDF_Update(depthimage,color,boneDQ, jointDQ, planeFunction);
	TSDF->MeshGenerate();
}

void BodyPart::Running(float boneDQ[8], float jointDQ[8], vector<Vec3d> &skeVec) {
	vector<Vec3>tmpskeleton;
	for (auto it = skeVec.begin(); it != skeVec.end(); ++it)
	{
		tmpskeleton.push_back(*it);
	}
	Running(boneDQ, jointDQ, tmpskeleton);
}

void BodyPart::Running(float boneDQ[8], float jointDQ[8], vector<Vec3> &skeVec) {
	skeletonVec3.clear();
	for (auto it = skeVec.begin(); it != skeVec.end(); ++it)
	{
		skeletonVec3.push_back(*it);
	}
	if (miss == 0)setPlaneF();
	miss++;

	TSDF->Fuse_RGBD_GPU(boneDQ, jointDQ, planeFunction);
	cout << "2nd function" << endl;
}

void BodyPart::DrawMesh()
{
	TSDF->DrawMesh(center);
}

void BodyPart::SavePLY(string filename) {
	TSDF->SavePLY(filename);
}

void BodyPart::getOrientation()
{   //Construct a buffer used by the pca analysis
	int sz = static_cast<int>(pts.size());
	cv::Mat data_pts = cv::Mat(sz, 3, CV_64FC1);

	for (int i = 0; i < data_pts.rows; ++i)
	{
		data_pts.at<double>(i, 0) = pts[i].x;
		data_pts.at<double>(i, 1) = pts[i].y;
		data_pts.at<double>(i, 2) = pts[i].z;
	}
	//Perform PCA analysis
	cv::PCA pca_analysis(data_pts, cv::Mat(), cv::PCA::DATA_AS_ROW);

	//Store the center of the object
	Vec3 cntr = Vec3((pca_analysis.mean.at<double>(0, 0)),
		(pca_analysis.mean.at<double>(0, 1)),
		(pca_analysis.mean.at<double>(0, 2)));
	center = cntr;

	//Store the eigenvalues and eigenvectors
	//Vec3 eigen_vecs[3];
	for (int i = 0; i < 3; ++i)
	{
		eigen_vecs[i] = Vec3(pca_analysis.eigenvectors.at<double>(i, 0),
			pca_analysis.eigenvectors.at<double>(i, 1),
			pca_analysis.eigenvectors.at<double>(i, 2));
		eigen_val[i] = pca_analysis.eigenvalues.at<double>(i);
	}

	// Draw the principal components
	p1 = eigen_vecs[0] * eigen_val[0];
	p2 = eigen_vecs[1] * eigen_val[1];
	p3 = eigen_vecs[2] * eigen_val[2];


	p1 = p1.normalized();
	p2 = p2.normalized();
	p3 = p3.normalized();

	ax_x = p1;
	ax_y = p2;
	ax_z = p3;
	
	MinMaxVal();

	courners[0] = p1*BodySize.x / 2.0 + p2*BodySize.y / 2.0 + p3*BodySize.z / 2.0 + center;
	courners[1] = p1*BodySize.x / 2.0 + p2*BodySize.y / 2.0 - p3*BodySize.z / 2.0 + center;
	courners[2] = p1*BodySize.x / 2.0 - p2*BodySize.y / 2.0 - p3*BodySize.z / 2.0 + center;
	courners[3] = p1*BodySize.x / 2.0 - p2*BodySize.y / 2.0 + p3*BodySize.z / 2.0 + center;

	courners[4] = -p1*BodySize.x / 2.0 + p2*BodySize.y / 2.0 + p3*BodySize.z / 2.0 + center;
	courners[5] = -p1*BodySize.x / 2.0 + p2*BodySize.y / 2.0 - p3*BodySize.z / 2.0 + center;
	courners[6] = -p1*BodySize.x / 2.0 - p2*BodySize.y / 2.0 - p3*BodySize.z / 2.0 + center;
	courners[7] = -p1*BodySize.x / 2.0 - p2*BodySize.y / 2.0 + p3*BodySize.z / 2.0 + center;

	return;
}

void BodyPart::getOrientation_Bone()
{
	int bIdx = myBodyNumber;
	int bone[2] = { bonelist[bIdx][0],bonelist[bIdx][1] };
	int boneP[2] = { bonelist[boneParent[bIdx]][0], bonelist[boneParent[bIdx]][1] };

	Vec3 cur_bone0, prev_bone0;
	Vec3 cur_bone1, prev_bone1;
	Vec3 cur_boneP0, cur_boneP1;

	cur_bone0 = skeletonVec3[bone[0]];
	cur_bone1 = skeletonVec3[bone[1]];

	prev_bone0 = skeletonVec3[bone[0]];
	prev_bone1 = skeletonVec3[bone[1]];

	cur_boneP0 = skeletonVec3[boneP[0]];
	cur_boneP1 = skeletonVec3[boneP[1]];

	center = (cur_bone0 + cur_bone1) / 2.0;

	p3 = (cur_bone1 - cur_bone0).normalized();
	p1 = Vec3(-p3.z,0.0,p3.x).normalized();
	p2 = p3.cross(p1);

	ax_x = p1;
	ax_y = p2;
	ax_z = p3;

	MinMaxVal();
	
	//BodySize.x = 0.3;
	//BodySize.y = 0.3;
	BodySize.z = (cur_bone1 - cur_bone0).length() + 0.01;

	courners[0] = p1*BodySize.x / 2.0 + p2*BodySize.y / 2.0 + p3*BodySize.z / 2.0 + center;
	courners[1] = p1*BodySize.x / 2.0 + p2*BodySize.y / 2.0 - p3*BodySize.z / 2.0 + center;
	courners[2] = p1*BodySize.x / 2.0 - p2*BodySize.y / 2.0 - p3*BodySize.z / 2.0 + center;
	courners[3] = p1*BodySize.x / 2.0 - p2*BodySize.y / 2.0 + p3*BodySize.z / 2.0 + center;

	courners[4] = -p1*BodySize.x / 2.0 + p2*BodySize.y / 2.0 + p3*BodySize.z / 2.0 + center;
	courners[5] = -p1*BodySize.x / 2.0 + p2*BodySize.y / 2.0 - p3*BodySize.z / 2.0 + center;
	courners[6] = -p1*BodySize.x / 2.0 - p2*BodySize.y / 2.0 - p3*BodySize.z / 2.0 + center;
	courners[7] = -p1*BodySize.x / 2.0 - p2*BodySize.y / 2.0 + p3*BodySize.z / 2.0 + center;
}

void BodyPart::setPts(vector<Vec3> _points)
{
	//copy(_points.begin(), _points.end(), pts);
	//pts = vector<Vec3>();
	pts.clear();
	for (auto it = _points.begin(); it != _points.end(); ++it)
	{
		pts.push_back(*it);
	}
}

void BodyPart::DrawBox()
{
	Line3D(courners[0], courners[1]);
	Line3D(courners[1], courners[2]);
	Line3D(courners[2], courners[3]);
	Line3D(courners[3], courners[0]);
	
	Line3D(courners[4], courners[5]);
	Line3D(courners[5], courners[6]);
	Line3D(courners[6], courners[7]);
	Line3D(courners[7], courners[4]);

	Line3D(courners[0], courners[4]);
	Line3D(courners[7], courners[3]);
	Line3D(courners[1], courners[5]);
	Line3D(courners[6], courners[2]);

	GLfloat color[4] = { 1.0,0.0,0.0,1.0 };
	Line3D(center, ax_x * BodySize.x + center, color);
	color[0] = 0.0; color[1] = 1.0;
	Line3D(center, ax_y * BodySize.y + center,color);
	color[1] = 0.0; color[2] = 1.0;
	Line3D(center, ax_z * BodySize.z + center,color);

	//for (auto &p : pts) {
	//	glPointSize(10.0);
	//	glColor4f(0.0, 0.0, 1.0, 1.0);
	//	glBegin(GL_POINTS);
	//	glVertex3f(p.x,p.y,p.z);
	//	glEnd();
	//}
	glPointSize(10.0);
	glColor4f(0.0, 1.0, 0.0, 1.0);
	glBegin(GL_POINTS);
	glVertex3f(center.x,-center.y,-center.z);
	glEnd();

	

}

void BodyPart::setCenter(Vec3 _pos)
{
	center = _pos;
	for (int i = 0; i < 8; i++)
	{
		courners[i] += center;
	}
}

void BodyPart::MinMaxVal()
{
	float tmp_min_x = 10000.0;
	float tmp_min_y = 10000.0;
	float tmp_min_z = 10000.0;
	float tmp_max_x = -10000.0;
	float tmp_max_y = -10000.0;
	float tmp_max_z = -10000.0;
	float alpha = 0.05;

	// point position at bodypart space
	float x, y, z;
	for (auto it = pts.begin(); it != pts.end(); ++it)
	{
		x = ax_x.dot(*it - center);
		y = ax_y.dot(*it - center);
		z = ax_z.dot(*it - center);

		tmp_min_x = min(x, tmp_min_x);
		tmp_min_y = min(y, tmp_min_y);
		tmp_min_z = min(z, tmp_min_z);

		tmp_max_x = max(x, tmp_max_x);
		tmp_max_y = max(y, tmp_max_y);
		tmp_max_z = max(z, tmp_max_z);
	}

	// if tsdf voxel space is small, set alpha
	min_x = tmp_min_x - alpha;
	min_y = tmp_min_y - alpha;
	min_z = tmp_min_z - alpha;

	max_x = tmp_max_x + alpha;
	max_y = tmp_max_y + alpha;
	max_z = tmp_max_z + alpha;

	// tmp is center of min_max @ PCA coordinate
	Vec3 tmp;
	tmp.x = (min_x + max_x) / 2.0;
	tmp.y = (min_y + max_y) / 2.0;
	tmp.z = (min_z + max_z) / 2.0;

	// change coordinate (PCA to world)
	tmp = Vec3(
	Vec3(ax_x.x, ax_y.x, ax_z.x).dot(tmp),
	Vec3(ax_x.y, ax_y.y, ax_z.y).dot(tmp),
	Vec3(ax_x.z, ax_y.z, ax_z.z).dot(tmp)
	);

	center.x += tmp.x;
	center.y += tmp.y;
	center.z += tmp.z;

	// set body parts size
	BodySize.x = max_x - min_x + alpha;
	BodySize.y = max_y - min_y + alpha;
	//BodySize.z = max_z - min_z;
	BodySize.z = BodySize.y;
}

vector<Vec3>* BodyPart::GetPts(int num) {
	return &this->pts;
}

void BodyPart::setPlaneF() {

	switch (myBodyNumber)
	{
	case BODY_PART_HEAD:
		boneV = skeletonVec3[Head] - skeletonVec3[SpineMid];
		point = skeletonVec3[SpineShoulder];
		break;
	case BODY_PART_DOWN_LEG_L:
		boneV = skeletonVec3[AnkleLeft] - skeletonVec3[HipLeft];
		point = skeletonVec3[KneeLeft];
		break;
	case BODY_PART_DOWN_LEG_R:
		boneV = skeletonVec3[AnkleRight] - skeletonVec3[HipRight];
		point = skeletonVec3[KneeRight];
		break;
	case BODY_PART_FOOT_L:
		boneV = skeletonVec3[FootLeft] - skeletonVec3[KneeLeft];
		point = skeletonVec3[AnkleLeft];
		break;
	case BODY_PART_FOOT_R:
		boneV = skeletonVec3[FootRight] - skeletonVec3[KneeRight];
		point = skeletonVec3[AnkleRight];
		break;
	case BODY_PART_FOREARM_L:
		boneV = skeletonVec3[WristLeft] - skeletonVec3[ShoulderLeft];
		point = skeletonVec3[ElbowLeft];
		break;
	case BODY_PART_FOREARM_R:
		boneV = skeletonVec3[WristRight] - skeletonVec3[ShoulderRight];
		point = skeletonVec3[ElbowRight];
		break;
	case BODY_PART_HAND_L:
		//boneV = skeletonVec3[HandLeft] - skeletonVec3[WristLeft];
		boneV = skeletonVec3[HandLeft] - skeletonVec3[ElbowLeft];
		point = skeletonVec3[WristLeft];
		break;
	case BODY_PART_HAND_R:
		//boneV = skeletonVec3[HandRight] - skeletonVec3[WristRight];
		boneV = skeletonVec3[HandRight] - skeletonVec3[ElbowRight];
		point = skeletonVec3[WristRight];
		break;
	case BODY_PART_TORSOR:
		boneV = skeletonVec3[SpineMid] - skeletonVec3[SpineBase];
		point = skeletonVec3[SpineBase];
		break;
	case BODY_PART_UPPER_ARM_L:
		boneV = skeletonVec3[ElbowLeft] - skeletonVec3[SpineShoulder];
		point = skeletonVec3[ShoulderLeft];
		break;
	case BODY_PART_UPPER_ARM_R:
		boneV = skeletonVec3[ElbowRight] - skeletonVec3[SpineShoulder];
		point = skeletonVec3[ShoulderRight];
		break;
	case BODY_PART_UP_LEG_L:
		boneV = skeletonVec3[KneeLeft] - skeletonVec3[SpineMid];
		point = skeletonVec3[HipLeft];

		break;
	case BODY_PART_UP_LEG_R:
		boneV = skeletonVec3[KneeRight] - skeletonVec3[SpineMid];
		point = skeletonVec3[HipRight];

		break;
	default:
		break;
	}

	boneV = boneV.normalized();
	planeFunction[0] = boneV.x;
	planeFunction[1] = boneV.y;
	planeFunction[2] = boneV.z;
	planeFunction[3] = -(boneV.dot(point));


}

void BodyPart::TransfomMesh(float pose[16]) {
	TSDF->VtxTransform(pose);
}

void BodyPart::TransfomMesh(Eigen::Matrix4d pose)
{
	float _pose[16] = {
		pose(0,0),pose(0,1),pose(0,2),pose(0,3),
		pose(1,0),pose(1,1),pose(1,2),pose(1,3),
		pose(2,0),pose(2,1),pose(2,2),pose(2,3),
		pose(3,0),pose(3,1),pose(3,2),pose(3,3),
	};
	TransfomMesh(_pose);
}

void BodyPart::SetD2C(float pose[16]) {
	TSDF->SetAllPose(pose);
}