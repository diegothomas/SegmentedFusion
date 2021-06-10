#include "Stitching.h"
typedef Eigen::Quaternionf Quat;

void projection(cv::Mat depth, Vec2 &pt2D, Vec3 &pt) {
	// i is the global index in the depth image
	float d = depth.at<unsigned short>(int(pt2D.x), int(pt2D.y));

	pt.x = d == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * d * ((pt2D.y - Calib[2]) /Calib[0]);
	pt.y = d == 0.0 ? 0.0 : -(Calib[9] / Calib[10]) * d * ((pt2D.x - Calib[3]) / Calib[1]);
	//pt.y = d == 0.0 ? 0.0 : (calib[9] / calib[10]) * d * ((convert_float(n - 1 - i) - calib[3]) / calib[1]);
	pt.z = d == 0.0 ? 0.0 : -(Calib[9] / Calib[10]) * d;

}


void GetRotatefrom2Vectors(Vec3 a, Vec3 b , Eigen::Matrix3f &res) {
	/*'''
		calculate the Rotation matrix form vector a to vector b
		: param a : start vector
		: param b : end vector
		: return : Rotation Matrix
	'''*/
	auto tmpa = a.normalized();
	auto tmpb = b.normalized();
	auto v = tmpa.cross(tmpb);
	auto c = tmpa.dot(tmpb);
	Eigen::Matrix3f Av;
	Av << 0., -v.z, v.y, 
		v.z, 0., -v.x, 
		-v.y, v.x, 0.;
	Eigen::Matrix3f R = Eigen::Matrix3f::Identity();
	if (c == -1) {
		cout << "meet a=-b" << endl;;
		return;
	}
	R = R + Av + 1.0 / (1.0 + c)* Av* Av;
	res = R;
	return;
}

void GetPoint(cv::Mat &_VMap,Vec2 &pt2D, Vec3 &pt) {
	if (pt2D.x < 0 || pt2D.x > _VMap.cols || pt2D.y < 0 || pt2D.y > _VMap.rows) {
		pt.x = 0.0;
		pt.y = 0.0;
		pt.z = 0.0;
	}
	else {
		pt.x = _VMap.at<cv::Vec4f>(int(pt2D.y), int(pt2D.x))[0];
		pt.y = _VMap.at<cv::Vec4f>(int(pt2D.y), int(pt2D.x))[1];
		pt.z = _VMap.at<cv::Vec4f>(int(pt2D.y), int(pt2D.x))[2];
	}
}

void getQuaternionMul(float quat1[4], float quat2[4], float res[4]) {
	Vec3 q1(quat1[1], quat1[2], quat1[3]);
	Vec3 q2(quat2[1], quat2[2], quat2[3]);
	res[0] = quat1[0] * quat2[0] - q1.dot(q2);

	Vec3 cross2quat = q1.cross(q2);

	res[1] = quat1[0] * quat2[1] + quat1[1] * quat2[0] + cross2quat.x;
	res[2] = quat1[0] * quat2[2] + quat1[2] * quat2[0] + cross2quat.y;
	res[3] = quat1[0] * quat2[3] + quat1[3] * quat2[0] + cross2quat.z;
	//res[1:4] = quat1[0] * quat2[1:4] + quat2[0] * quat1[1:4] + np.cross(quat1[1:4], quat2[1:4]);
}

void getQuaternionfromMatrix(Eigen::Matrix3f mat_ori, float quat[4]) {
	Eigen::Matrix3f mat = mat_ori;

	float tr = mat(0, 0) + mat(1, 1) + mat(2, 2);
	float S = 0.0;
	if (tr > 0) {
		S = pow(tr + 1, 0.5) * 2;
		quat[0] = 0.25*S;
		quat[1] = (mat(2, 1) - mat(1, 2)) / S;
		quat[2] = (mat(0, 2) - mat(2, 0)) / S;
		quat[3] = (mat(1, 0) - mat(0, 1)) / S;
	}else if((mat(0, 0) > mat(1, 1)) && (mat(0, 0) > mat(2, 2))) {
		S = pow(1 + mat(0, 0) - mat(1, 1) - mat(2, 2), 0.5) * 2;
		quat[0] = (mat(2, 1) - mat(1, 2)) / S;
		quat[1] = 0.25*S;
		quat[2] = (mat(0, 1) + mat(1, 0)) / S;
		quat[3] = (mat(0, 2) + mat(2, 0)) / S;
	}
	else if (mat(1, 1) > mat(2, 2)) {
		S = pow(1 + mat(1, 1) - mat(0, 0) - mat(2, 2), 0.5) * 2;
		quat[0] = (mat(0, 2) - mat(2, 0)) / S;
		quat[1] = (mat(0, 1) + mat(1, 0)) / S;
		quat[2] = 0.25*S;
		quat[3] = (mat(1, 2) + mat(2, 1)) / S;
	}
	else {
		S = pow(1 + mat(2, 2) - mat(0, 0) - mat(1, 1), 0.5) * 2;
		quat[0] = (mat(1, 0) - mat(0, 1)) / S;
		quat[1] = (mat(0, 2) + mat(2, 0)) / S;
		quat[2] = (mat(1, 2) + mat(2, 1)) / S;
		quat[3] = 0.25*S;
	}
}

void getQuaternionNormalize(float quat[4]) {
	float mag = quat[0] * quat[0] + quat[1] * quat[1] + quat[2] * quat[2] + quat[3] * quat[3];
	quat[0] /= mag;
	quat[1] /= mag;
	quat[2] /= mag;
	quat[3] /= mag;

}

void getDualQuaternionfromMatrix(Eigen::Matrix4f mat_ori, float DQ[8]) {
	Eigen::Matrix3f mat = mat_ori.block(0,0,3,3);
	float quatReal[4], quatDual[4];
	getQuaternionfromMatrix(mat, quatReal);
	getQuaternionNormalize(quatReal);
	quatDual[0] = 0.0;
	
	quatDual[1] = mat_ori(0, 3);
	quatDual[2] = mat_ori(1, 3);
	quatDual[3] = mat_ori(2, 3);

	float dquat[4];
	getQuaternionMul(quatDual, quatReal, dquat);
	for (int i = 0; i < 4; i++)
	{
		dquat[i] *= 0.5;
	}
	for (int i = 0; i < 4; i++)
	{
		DQ[i + 0] = quatReal[i];
		DQ[i + 4] = dquat[i];
	}
	return;
}

Stitching::Stitching()
{
	for (int i = 0; i < num_oflist; i++)
	{
		boneSubTransAll[i] = Eigen::Matrix4f::Identity();
		boneTransAll[i] = Eigen::Matrix4f::Identity();
		init[i] = true;
	}
}

Stitching::Stitching(cv::Mat depth)
{
	this->depth = depth.clone();
	this->VMap = cv::Mat::zeros(depth.size(),CV_8UC1);

	for (int i = 0; i < num_oflist; i++)
	{
		boneSubTransAll[i] = Eigen::Matrix4f::Identity();
		boneTransAll[i] = Eigen::Matrix4f::Identity();
		init[i] = true;
	}
}


Stitching::~Stitching()
{
}

void Stitching::GetVBonesTrans(vector<Vec3> skeVtx_cur, vector<Vec3> skeVtx_prev)
{
	/*"""
	Get transform matrix of bone from previous to current frame
	: param skeVtx_cur : the skeleton Vtx in current frame
	: param skeVtx_prev : the skeleton Vtx in previous frame
	: return : calculated SkeVtx
	"""*/

	//#initial
	for (int i = 0; i < 20; i++) {
		boneTrans[i] = Eigen::Matrix4f::Identity();
		boneSubTrans[i] = Eigen::Matrix4f::Identity();
	}
	//for (int i = 0; i < BODY_PART_NUMBER; i++)
	for (int i = 0; i < num_oflist; i++)
	{
		int bIdx = i;
		int bone[2] = { bonelist[bIdx][0],bonelist[bIdx][1] };
		int boneP[2] = { bonelist[boneParent[bIdx]][0], bonelist[boneParent[bIdx]][1] };

		Vec3 cur_bone0, prev_bone0;
		Vec3 cur_bone1, prev_bone1;
		Vec3 prev_boneP0, prev_boneP1;

		cur_bone0 = skeVtx_cur[bone[0]];
		cur_bone1 = skeVtx_cur[bone[1]];

		prev_bone0 = skeVtx_prev[bone[0]];
		prev_bone1 = skeVtx_prev[bone[1]];

		prev_boneP0 = skeVtx_prev[boneP[0]];
		prev_boneP1 = skeVtx_prev[boneP[1]];

		auto v1 = cur_bone1 - cur_bone0;
		auto v2 = prev_bone1 - prev_bone0;
		auto v3 = prev_boneP1 - prev_boneP0;

		Eigen::Matrix3f R = Eigen::Matrix3f::Identity();
		GetRotatefrom2Vectors(v2, v1, R);
		Eigen::Matrix4f	R_T = Eigen::Matrix4f::Identity();
		R_T.block(0, 0, 3, 3) = R;

		GetRotatefrom2Vectors(v2, v3, R);
		Eigen::Matrix4f R1_T = Eigen::Matrix4f::Identity();
		R1_T.block(0, 0, 3, 3) = R;

		boneTrans[bIdx] = R_T;
		boneSubTrans[bIdx] = R1_T;

		Eigen::Matrix4f	T_T = Eigen::Matrix4f::Identity();
		T_T(0, 3) = prev_bone0.x;
		T_T(1, 3) = prev_bone0.y;
		T_T(2, 3) = prev_bone0.z;

		Eigen::Matrix4f	T1_T = Eigen::Matrix4f::Identity();
		T1_T(0, 3) = cur_bone0.x;
		T1_T(1, 3) = cur_bone0.y;
		T1_T(2, 3) = cur_bone0.z;

		Eigen::Matrix4f T_T_inv = Eigen::Matrix4f::Identity();
		T_T_inv(0, 3) = -prev_bone0.x;
		T_T_inv(1, 3) = -prev_bone0.y;
		T_T_inv(2, 3) = -prev_bone0.z;

		boneTrans[bIdx] = T1_T*(R_T*T_T_inv);
		boneSubTrans[bIdx] = T_T*(R1_T * T_T_inv);
	}
	return;
}

void Stitching::GetVBonesTrans(vector<Vec3d> skeVtx_cur, vector<Vec3d> skeVtx_prev)
{
	/*"""
	Get transform matrix of bone from previous to current frame
	: param skeVtx_cur : the skeleton Vtx in current frame
	: param skeVtx_prev : the skeleton Vtx in previous frame
	: return : calculated SkeVtx
	"""*/

	//#initial
	for (int i = 0; i < 20; i++) {
		boneTrans[i] = Eigen::Matrix4f::Identity();
		boneSubTrans[i] = Eigen::Matrix4f::Identity();
	}
	//for (int i = 0; i < BODY_PART_NUMBER; i++)
	for (int i = 0; i < num_oflist; i++)
	{
		int bIdx = i;
		int bone[2] = { bonelist[bIdx][0],bonelist[bIdx][1] };
		int boneP[2] = { bonelist[boneParent[bIdx]][0], bonelist[boneParent[bIdx]][1] };

		Vec3 cur_bone0, prev_bone0;
		Vec3 cur_bone1, prev_bone1;
		Vec3 prev_boneP0, prev_boneP1;

		cur_bone0 = skeVtx_cur[bone[0]];
		cur_bone1 = skeVtx_cur[bone[1]];

		prev_bone0 = skeVtx_prev[bone[0]];
		prev_bone1 = skeVtx_prev[bone[1]];

		prev_boneP0 = skeVtx_prev[boneP[0]];
		prev_boneP1 = skeVtx_prev[boneP[1]];

		auto v1 = cur_bone1 - cur_bone0;
		auto v2 = prev_bone1 - prev_bone0;
		auto v3 = prev_boneP1 - prev_boneP0;

		Eigen::Matrix3f R = Eigen::Matrix3f::Identity();
		GetRotatefrom2Vectors(v2, v1, R);
		Eigen::Matrix4f	R_T = Eigen::Matrix4f::Identity();
		R_T.block(0, 0, 3, 3) = R;

		GetRotatefrom2Vectors(v2, v3, R);
		Eigen::Matrix4f R1_T = Eigen::Matrix4f::Identity();
		R1_T.block(0, 0, 3, 3) = R;

		boneTrans[bIdx] = R_T;
		boneSubTrans[bIdx] = R1_T;

		Eigen::Matrix4f	T_T = Eigen::Matrix4f::Identity();
		T_T(0, 3) = prev_bone0.x;
		T_T(1, 3) = prev_bone0.y;
		T_T(2, 3) = prev_bone0.z;

		Eigen::Matrix4f	T1_T = Eigen::Matrix4f::Identity();
		T1_T(0, 3) = cur_bone0.x;
		T1_T(1, 3) = cur_bone0.y;
		T1_T(2, 3) = cur_bone0.z;

		Eigen::Matrix4f T_T_inv = Eigen::Matrix4f::Identity();
		T_T_inv(0, 3) = -prev_bone0.x;
		T_T_inv(1, 3) = -prev_bone0.y;
		T_T_inv(2, 3) = -prev_bone0.z;

		boneTrans[bIdx] = T1_T*(R_T*T_T_inv);
		boneSubTrans[bIdx] = T_T*(R1_T * T_T_inv);
	}
	return;
}





void Stitching::SetDepth(cv::Mat depth, cv::Mat VMap, bool init)
{
	this->depth = depth.clone();
	if(init)this->VMap = VMap.clone();
	this->prev_Vmap = this->VMap.clone();
	this->VMap = VMap.clone();
}

void Stitching::SetDepth(cv::Mat depth, bool init)
{
	this->depth = depth.clone();
}

void Stitching::GetJointInfo(int bp, float resBone[8], float resJoint[8])
{
	/*"""
		get joints' position and transform related to the body part
		:param bp
		: param boneTrans : all bones' transform
		: param boneSubTrans : all parent bone's transform
		: return : bone's DQ, joint's DQ
	"""
	*/
	//#plane3
	int sm = 1;
	float boneDQ[8] = { 1,0,0,0,0,0,0,0 };
	float jointDQ[8] = { 1,0,0,0,0,0,0,0 };

	int num1,num2;
	num1 = bp;
	num2 = boneParent[bp];


	boneTransAll[num1] = boneTrans[num1] * boneTransAll[num1];
	getDualQuaternionfromMatrix(boneTransAll[num1], boneDQ);

	if (!init[num1]) {
		boneSubTransAll[num1] = boneTrans[num2] * boneSubTransAll[num1];
		getDualQuaternionfromMatrix(boneSubTransAll[num1], jointDQ);
	}
	else {
		boneSubTransAll[num1] = boneSubTrans[num1]*boneSubTransAll[num1];
		getDualQuaternionfromMatrix(boneSubTransAll[num1], jointDQ);
		init[num1] = false;
	}
	for (int i = 0; i < 8; i++)
	{
		resBone[i] = boneDQ[i];
		resJoint[i] = jointDQ[i];
	}
}

