#include "Segmentatnion.h"



void DrawIntersection(Mat& output, Eigen::Vector2i pt1, Eigen::Vector2i pt2) {
	circle(output, Vec2i(pt1.x(), pt1.y()), 5, 65535, -1);
	circle(output, Vec2i(pt2.x(), pt2.y()), 5, 65535, -1);
	line(output, Point(pt1.x(), pt1.y()), Point(pt2.x(), pt2.y()), 65535);
}

Segmentation::Segmentation()
{
}

Segmentation::Segmentation(Mat inputDepth, vector<Vec2i> inputSkeleton)
{
	depthImage = inputDepth.clone();
	depthImage_raw = inputDepth.clone();
	for (auto input : inputSkeleton) {
		skeleton.push_back(Eigen::Vector2i(input(0),input(1)));
	}

	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		maskImage[i] = cv::Mat::zeros(depthImage.size(), CV_8UC1);
	}
	humanMaskImage = Mat::zeros(depthImage.size(), CV_8UC1);
}

Segmentation::Segmentation(Mat inputDepth, vector<Eigen::Vector2i> inputSkeleton)
{
	depthImage = inputDepth.clone();
	depthImage_raw = inputDepth.clone();
	for (auto input : inputSkeleton) {
		skeleton.push_back( input);
	}

	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		maskImage[i] = cv::Mat::zeros(depthImage.size(), CV_8UC1);
	}
	humanMaskImage = Mat::zeros(depthImage.size(), CV_8UC1);

}

Segmentation::Segmentation(Mat inputDepth, vector<Vec2> inputSkeleton)
{
	depthImage = inputDepth.clone();
	depthImage_raw = inputDepth.clone();
	for (auto input : inputSkeleton) {
		skeleton.push_back(Eigen::Vector2i(input.x, input.y));
	}

	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		maskImage[i] = cv::Mat::zeros(depthImage.size(), CV_8UC1);
	}
	humanMaskImage = Mat::zeros(depthImage.size(), CV_8UC1);
}

Segmentation::~Segmentation()
{
}

void Segmentation::Run()
{
	//cout << "Segmentation->Run" << endl;

	for (auto pt : skeleton) {
		circle(depthImage, Vec2i(pt.x(),pt.y()), 5, 65535,-1);
	}

	SegmentArm(0);
	SegmentArm(1);

	SegmentLeg(0);
	SegmentLeg(1);

	SegmentHand();
	SegmentHand(1);
	SegmentHead();

	SegmentFoot(0);
	SegmentFoot(1);

	SegmentBody();

	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		humanMaskImage += maskImage[i];
	}

	//return;
	
	// draw color mask image
	cv::Mat colorMask = cv::Mat::zeros(depthImage.size(), CV_8UC3);

	colorMask.setTo(COLOR_HEAD, maskImage[BODY_PART_HEAD]>0);
	colorMask.setTo(COLOR_BODY, maskImage[BODY_PART_TORSOR]>0);

	colorMask.setTo(COLOR_UPPER_ARM_L, maskImage[BODY_PART_UPPER_ARM_L]>0);
	colorMask.setTo(COLOR_UPPER_ARM_R, maskImage[BODY_PART_UPPER_ARM_R]>0);
	colorMask.setTo(COLOR_FOREARM_L, maskImage[BODY_PART_FOREARM_L]>0);
	colorMask.setTo(COLOR_FOREARM_R, maskImage[BODY_PART_FOREARM_R]>0);

	colorMask.setTo(COLOR_THIGH_L, maskImage[BODY_PART_UP_LEG_L]>0);
	colorMask.setTo(COLOR_THIGH_R, maskImage[BODY_PART_UP_LEG_R]>0);
	colorMask.setTo(COLOR_CALF_L, maskImage[BODY_PART_DOWN_LEG_L]>0);
	colorMask.setTo(COLOR_CALF_R, maskImage[BODY_PART_DOWN_LEG_R]>0);

	colorMask.setTo(COLOR_HAND_L, maskImage[BODY_PART_HAND_L]>0);
	colorMask.setTo(COLOR_HAND_R, maskImage[BODY_PART_HAND_R]>0);
	colorMask.setTo(COLOR_FOOT_L, maskImage[BODY_PART_FOOT_L]>0);
	colorMask.setTo(COLOR_FOOT_R, maskImage[BODY_PART_FOOT_R]>0);

	cv::imshow("Segmenpart:colormask", colorMask);
	//cv::imshow("Segmenpart:inputdepth", depthImage);
	waitKey(1);


}

void Segmentation::SegmentArm(int side)
{
	//cout << "Segmentation->SegmentArm" << endl;
	string window = "right arm";

	// Right Arm
	int shoulder = ShoulderRight;
	int elbow = ElbowRight;
	int wrist = WristRight;

	if (side == 1) {
		shoulder = ShoulderLeft;
		elbow = ElbowLeft;
		wrist = WristLeft;
		window = "left arm";
	}

	//cout << window << endl;

	// slope Fore and Upper Arm
	Eigen::Vector3d slopesForearm;
	findSlope(skeleton[elbow], skeleton[wrist], slopesForearm);
	Eigen::Vector3d slopesUpperarm;
	findSlope(skeleton[elbow], skeleton[shoulder], slopesUpperarm);

	double a_pen67 = -slopesForearm[1];
	double b_pen67 = slopesForearm[0];
	double a_pen = slopesForearm[0] + slopesUpperarm[0];
	double b_pen = (slopesForearm[1] + slopesUpperarm[1]);

	if (a_pen * b_pen == 0) {
		//cout << "apen*bpen = 0" << endl;
		a_pen = slopesUpperarm[1];
		b_pen = -slopesUpperarm[0];
	}
	// Perpendicular slopes
	double c_pen = -(a_pen*skeleton[elbow].x() + b_pen*skeleton[elbow].y());
	double c_pen67 = -(a_pen67*skeleton[wrist].x() + b_pen67*skeleton[wrist].y());

	// Get intersection points at elbow
	Eigen::Vector2i intersection_elbow[2];
	inferedPoint(a_pen, b_pen, c_pen, skeleton[elbow], 20.0 , intersection_elbow[0], intersection_elbow[1]);
	DrawIntersection(depthImage, intersection_elbow[0], intersection_elbow[1]);
	CheckIntersection(intersection_elbow[0], intersection_elbow[1]);
	
	//get intersection near the wrist
	Eigen::Vector2i intersection_wrist[2];
	inferedPoint(a_pen67, b_pen67, c_pen67, skeleton[wrist], 20.0, intersection_wrist[0], intersection_wrist[1]);
	DrawIntersection(depthImage, intersection_wrist[0], intersection_wrist[1]);
	CheckIntersection(intersection_wrist[0], intersection_wrist[1]);

	Eigen::Vector2i vect_elbow(intersection_elbow[0] - skeleton[elbow]);
	Eigen::Vector2i vect_wrist(intersection_wrist[0] - skeleton[wrist]);
	Eigen::Vector2i vect67(skeleton[wrist] - skeleton[elbow]);
	Eigen::Vector2i vect67_pen = { vect67.y(), -vect67.x() };


	// list of the 4 points defining the corners the forearm
	vector<Eigen::Vector2i> pt4D;
	//list of the 4 points defining the corners the forearm permuted
	vector<Eigen::Vector2i> pt4D_bis;
	pt4D.push_back(intersection_elbow[0]);
	pt4D.push_back(intersection_elbow[1]);
	pt4D.push_back(intersection_wrist[1]);
	pt4D.push_back(intersection_wrist[0]);



	//Get slopes for each line of the polygon
	Eigen::Vector3d eq[4];
	findSlope(intersection_elbow[0], intersection_wrist[0], eq[0]);
	findSlope(intersection_elbow[1], intersection_elbow[0], eq[1]);
	findSlope(intersection_wrist[1], intersection_elbow[1], eq[2]);
	findSlope(intersection_wrist[0], intersection_wrist[1], eq[3]);


	//erase all NaN in the array
	// NANの消去が未実装
	Eigen::Vector3d polygonSlope[4];
	for (int i = 0; i < 4; i++)
	{
		polygonSlope[i] = eq[i];
	}

	// get reference point
	Eigen::Vector2d midpoint((skeleton[elbow].x() + skeleton[wrist].x()) / 2, (skeleton[elbow].y() + skeleton[wrist].y()) / 2);
	double ref[4] = { distEQ2Pt(polygonSlope[0],midpoint),distEQ2Pt(polygonSlope[1],midpoint),
		distEQ2Pt(polygonSlope[2],midpoint),distEQ2Pt(polygonSlope[3],midpoint) };
	cv::Mat tmpmask;
	tmpmask = getmask(pt4D, elbow, wrist, "right arm").clone();
	//fill the polygon
	if (side == 0) {
		tmpmask = getmask(pt4D, elbow, wrist, "right arm");
		//armRight = tmpmask.clone();
		maskImage[BODY_PART_FOREARM_R].setTo(1.0, tmpmask > 0);
	}
	else {
		tmpmask = getmask(pt4D, elbow, wrist, "left arm");
		//armLeft = tmpmask.clone();
		maskImage[BODY_PART_FOREARM_L].setTo(1.0, tmpmask > 0);
	}
	//imshow(window, tmpmask*65535);


	// Upperarm
	// find slopes neck head spline
	//cout << "Upper arm" << endl;
	Eigen::Vector3d slopesH;
	findSlope(skeleton[Neck], skeleton[Head], slopesH);
	a_pen = slopesH[1];
	b_pen = -slopesH[0];
	c_pen = -(a_pen*skeleton[2].x() + b_pen*skeleton[2].y());

	// find the peak of sholder//temporary 
	Eigen::Vector2i points[5];
	points[0] = skeleton[elbow];
	points[1] = skeleton[shoulder];
	points[2] = skeleton[SpineShoulder];
	points[3] = skeleton[3];
	points[4] = Eigen::Vector2i(skeleton[elbow].x(), skeleton[3].y());


	Eigen::Vector3d slopeTorso;
	findSlope(skeleton[SpineShoulder], skeleton[shoulder], slopeTorso);

	a_pen = slopeTorso[0] + slopesUpperarm[0];
	b_pen = slopeTorso[1] + slopesUpperarm[1];
	if (a_pen*b_pen == 0) {
		a_pen = slopeTorso[1];
		b_pen = -slopeTorso[0];
	}
	c_pen = -(a_pen*skeleton[shoulder].x() + b_pen*skeleton[shoulder].y());

	Eigen::Vector2i intersection_shoulder[2];
	inferedPoint(a_pen, b_pen, c_pen, skeleton[shoulder], 20.0, intersection_shoulder[0], intersection_shoulder[1]);

	Eigen::Vector2i vect65 = skeleton[shoulder] - skeleton[elbow];

	Eigen::Vector2i vect_215 = intersection_shoulder[0] - skeleton[shoulder];

	Eigen::Vector3d t = Eigen::Vector3d(vect_elbow.x(), vect_elbow.y(), 0.0).cross(Eigen::Vector3d(vect65.x(), vect65.y(), 0.0));
	Eigen::Vector3d t1 = Eigen::Vector3d(vect_215.x(), vect_215.y(), 0.0).cross(Eigen::Vector3d(-vect65.x(), -vect65.y(), 0.0));

	if (t1.z() > 0) {
		auto tmp = intersection_shoulder[0];
		intersection_shoulder[0] = intersection_shoulder[1];
		intersection_shoulder[1] = tmp;
		//cout << "wrong in line775" << endl;
	}
	if (t.z() < 0) {
		auto tmp = intersection_elbow[0];
		intersection_elbow[0] = intersection_elbow[1];
		intersection_elbow[1] = tmp;
		//cout << "wrong in line781" << endl;
	}

	// check if intersection is onthe head
	Eigen::Vector2i peakshoulder, peakArmpit;

	if (side == 0) {
		peakshoulder = intersection_shoulder[1];
		if (intersection_shoulder[1].y() < skeleton[Head].y()) {
			//cout << ("intersection shoulder is upper the head R") << endl;
			intersection_shoulder[1].y() = skeleton[Neck].y();
			intersection_shoulder[1].x() = round(-(b_pen*intersection_shoulder[1].y() + c_pen) / a_pen);
			peakshoulder = (intersection_shoulder[1]);
		}
	}
	else {
		peakshoulder = intersection_shoulder[0];
		if (intersection_shoulder[0].y() < skeleton[Head].y()) {
			//cout << ("intersection shoulder is upper the head L") << endl;
			intersection_shoulder[0].y() = skeleton[Neck].y();
			intersection_shoulder[0].x() = round(-(b_pen*intersection_shoulder[0].y() + c_pen) / a_pen);
			peakshoulder = (intersection_shoulder[0]);
		}
	}

	DrawIntersection(depthImage, intersection_shoulder[0], intersection_shoulder[1]);
	CheckIntersection(intersection_shoulder[0], intersection_shoulder[1]);
	// the upper arm need a fifth point->Let us find it by finding the closest point to shoulder point
	points[0] = skeleton[elbow];
	points[1] = skeleton[shoulder];
	points[2] = skeleton[SpineShoulder];
	points[3] = skeleton[0];
	points[4] = Eigen::Vector2i(skeleton[elbow].x(), skeleton[SpineMid].y());

	peakArmpit = Eigen::Vector2i(skeleton[shoulder].x(), skeleton[SpineMid].y());


	// constraint on peakArmpit
	if (side == 0 && peakArmpit.x() > intersection_elbow[0].x()) {
		//cout << "meet the constrains on peakArmpitR" << endl;
		peakArmpit = Eigen::Vector2i(intersection_elbow[0].x() / 2 + intersection_shoulder[1].x() / 2 - 2, intersection_elbow[0].y() / 2 + intersection_shoulder[1].y() / 2);
	}
	else if (side == 1 && peakArmpit.x() < intersection_elbow[1].x()) {
		//cout << "meet the constrains on peakArmpitL" << endl;
		peakArmpit = Eigen::Vector2i(intersection_elbow[1].x() / 2 + intersection_shoulder[0].x() / 2 + 2, intersection_elbow[1].y() / 2 + intersection_shoulder[0].y() / 2);
	}

	// check if intersection is on the head
	Eigen::Vector3d slopesPeakShoulder;
	if (peakshoulder.y() <= skeleton[Neck].y()) {
		auto temp = peakshoulder;
		peakshoulder.y() = skeleton[Neck].y();
		findSlope(temp, peakArmpit, slopesPeakShoulder);
		if (side == 0) {
			//cout << ("peakshoulder is upper the neck R") << endl;;
			peakshoulder.x() = round(-(slopesPeakShoulder[1] * peakshoulder.y() + slopesPeakShoulder[2]) / slopesPeakShoulder[0]);
		}
		else {
			//cout << ("peakshoulder is upper the neck L") << endl;
			peakshoulder.x() = round(-(slopesPeakShoulder[1] * peakshoulder.y() + slopesPeakShoulder[2]) / slopesPeakShoulder[0]);
		}
	}

	// create the upperarm polygon out the five point defining it
	vector<Eigen::Vector2i> ptA;
	ptA.push_back(intersection_elbow[0]);
	ptA.push_back(intersection_elbow[1]);
	ptA.push_back(intersection_shoulder[1]);
	ptA.push_back(intersection_shoulder[0]);
	//ptA.push_back(peakArmpit);

	if (side != 0) {
		upperArmPtsL = ptA;
		peakshoulderL = peakshoulder;
	}
	else {
		upperArmPtsR = ptA;
		peakshoulderR = peakshoulder;
	}

	//create the upperarm polygon out the five point defining it
	if (side == 0) {
		window = "right upper arm";
		tmpmask = getmask(ptA, elbow, shoulder, "right upper arm").clone();
		maskImage[BODY_PART_UPPER_ARM_R].setTo(1.0, tmpmask > 0);
	}
	else {
		window = "left upper arm";
		tmpmask = getmask(ptA, elbow, shoulder, "left upper arm").clone();
		maskImage[BODY_PART_UPPER_ARM_L].setTo(1.0, tmpmask > 0);
	}
}

void Segmentation::SegmentLeg(int side) {
	
	//cout << "Segmentation->SegmentArm" << endl;
	string window = "right leg";

	//right leg
	int	knee = KneeRight;
	int	hip = HipRight;
	int	ankle = AnkleRight;
	legRight = cv::Mat::zeros(depthImage.size(), CV_8UC1);

	if (side == 1) {
		// left leg
		knee = KneeLeft;
		hip = HipLeft;
		ankle = AnkleLeft;
		legLeft = cv::Mat::zeros(depthImage.size(), CV_8UC1);
		window = "left leg";
	}
	//cout << window << endl;
	Eigen::Vector2i peak1(skeleton[SpineBase].x(), (skeleton[HipLeft].y() + skeleton[HipRight].y()) / 2.0);


	Eigen::Vector3d slopeThigh, slopeCalf;
	double a_pen, b_pen, c_pen;
	findSlope(skeleton[hip], skeleton[knee], slopeThigh);
	findSlope(skeleton[ankle], skeleton[knee], slopeCalf);
	a_pen = slopeThigh[0] + slopeCalf[0];
	b_pen = slopeThigh[1] + slopeCalf[1];
	if (a_pen*b_pen == 0) {
		a_pen = slopeThigh[1];
		b_pen = -slopeThigh[0];
	}
	c_pen = -(a_pen*skeleton[knee].x() + b_pen*skeleton[knee].y());

	// find 2 points corner of the knee
	Eigen::Vector2i intersection_knee[2];
	inferedPoint(a_pen, b_pen, c_pen, skeleton[knee], 20.0, intersection_knee[0], intersection_knee[1]);
	Eigen::Vector2i vect_knee = intersection_knee[0] - intersection_knee[1];

	CheckIntersection(intersection_knee[0], intersection_knee[1]);
	DrawIntersection(depthImage, intersection_knee[0], intersection_knee[1]);

	//slopesthigh & slopecrsh
	Eigen::Vector2i intersection_rsh[2];
	Eigen::Vector3d slopesrsh;
	//findSlope(skeleton[SpineMid], skeleton[hip], slopesrsh);
	findSlope(skeleton[SpineMid], skeleton[knee], slopesrsh);
	a_pen = slopeThigh[1] + slopesrsh[1];
	b_pen = -(slopeThigh[0] + slopesrsh[0]);
	if (a_pen*b_pen == 0) {
		a_pen = slopesrsh[1];
		b_pen = -slopesrsh[0];
	}
	c_pen = -(a_pen*skeleton[hip].x() + b_pen*skeleton[hip].y());
	
	inferedPoint(a_pen, b_pen, c_pen, skeleton[hip],20.0, intersection_rsh[0], intersection_rsh[1]);
	DrawIntersection(depthImage, intersection_rsh[0], intersection_rsh[1]);
	CheckIntersection(intersection_rsh[0], intersection_rsh[1]);

	Eigen::Vector2i pos0;
	vector<Eigen::Vector2i> ptA;

	Mat tmpmask;
	if (side == 0) {
		ptA.push_back(peak1);
		ptA.push_back(intersection_rsh[1]);
		ptA.push_back(intersection_knee[1]);
		ptA.push_back(intersection_knee[0]);
		thighPtsR = ptA;
		thighPtsR[1].y() = max(thighPtsR[1].y(), intersection_rsh[0].y());
		tmpmask = getmask(ptA, knee, hip, "leg_up_right");
		legRight = legRight + tmpmask;
		maskImage[BODY_PART_UP_LEG_R].setTo(1, tmpmask > 0);
	}
	else {
		ptA.push_back(intersection_rsh[0]);
		ptA.push_back(peak1);
		ptA.push_back(intersection_knee[1]);
		ptA.push_back(intersection_knee[0]);
		thighPtsL = ptA;
		thighPtsL[0].y() = max(thighPtsL[0].y(), intersection_rsh[1].y());
		tmpmask = getmask(ptA, knee, hip, "leg_up_left");
		legLeft = legLeft + tmpmask;
		maskImage[BODY_PART_UP_LEG_L].setTo(1, tmpmask > 0);
	}

	///

	a_pen = slopeCalf[1];
	b_pen = -slopeCalf[0];
	c_pen = -(a_pen*skeleton[ankle].x() + b_pen*skeleton[ankle].y());

	// find 2 points corner of the ankle
	Eigen::Vector2i intersection_ankle[2];
	inferedPoint(a_pen, b_pen, c_pen, skeleton[ankle], 20.0, intersection_ankle[0], intersection_ankle[1]);
	if (side != 0) {//: # if two ankles are too close
		if ((intersection_ankle[1].x() > (skeleton[KneeLeft].x() + skeleton[KneeRight].x()) / 2)) {
			//cout << ("two ankles are too close L") << endl;
			intersection_ankle[1].x() = (skeleton[KneeLeft].x() + skeleton[KneeRight].x()) / 2;
		}
	}
	else {
		if ((intersection_ankle[0].x() < (skeleton[KneeLeft].x() + skeleton[KneeRight].x()) / 2)) {
			intersection_ankle[0].x() = (skeleton[KneeLeft].x() + skeleton[KneeRight].x()) / 2;
			//cout << ("two ankles are too close R") << endl;
		}
	}

	CheckIntersection(intersection_ankle[0], intersection_ankle[1]);
	DrawIntersection(depthImage,intersection_ankle[0], intersection_ankle[1]);

	ptA.clear();
	ptA.push_back(intersection_ankle[1]);
	ptA.push_back(intersection_ankle[0]);
	ptA.push_back(intersection_knee[0]);
	ptA.push_back(intersection_knee[1]);
	if (side == 0) {
		window = "leg_down_right";
		tmpmask = getmask(ptA, ankle, knee, "leg_down_right");
		legRight = legRight + 2.0*tmpmask;
		maskImage[BODY_PART_DOWN_LEG_R].setTo(1, tmpmask > 0);
		calfPtsR.clear();
		for(auto p : ptA)calfPtsR.push_back(p);
	}
	else {
		window = "leg_down_left";
		tmpmask = getmask(ptA, ankle, knee, "leg_down_left");
		legLeft = legLeft + tmpmask;
		maskImage[BODY_PART_DOWN_LEG_L].setTo(1, tmpmask > 0);
		calfPtsL.clear();
		for (auto p : ptA)calfPtsL.push_back(p);
	}

	//imshow(window, tmpmask * 65535);
}

void Segmentation::SegmentHead() {

	//cout << "SegmentHead" << endl;
	double a_pen, b_pen, c_pen;
	Eigen::Vector3d slopesSH;
	findSlope(peakshoulderL, peakshoulderR, slopesSH);

	a_pen = slopesSH[0];
	b_pen = slopesSH[1];
	c_pen = -(a_pen*peakshoulderL.x() + b_pen*peakshoulderL.y());


	// find left
	int x = skeleton[ShoulderLeft].x();
	int y = 0;
	//y = int(round(-(a_pen*x + c_pen) / b_pen));
	y = int(skeleton[Neck].y());
	Eigen::Vector2i headLeft(x, y);

	// find right
	x = skeleton[ShoulderRight].x();
	//y = int(round(-(a_pen*x + c_pen) / b_pen));
	y = int(skeleton[Neck].y());
	Eigen::Vector2i headRight(x, y);

	// distance head - neck
	//float h = 2 * (skeleton_Eigen::Vector2i[2].y - skeleton_Eigen::Vector2i[3].y);
	double h = 2 * (skeleton[Neck].y() - skeleton[Head].y());

	// create point that higher than the head
	Eigen::Vector2i headUp_right(skeleton[ShoulderRight].x(), skeleton[Neck].y() - h);
	Eigen::Vector2i headUp_left(skeleton[ShoulderLeft].x(), skeleton[Neck].y() - h);
	//# stock corner of the polyogne
	vector<Eigen::Vector2i> pt4D;
	pt4D.push_back(headUp_right);
	pt4D.push_back(headUp_left);
	pt4D.push_back(headLeft);
	pt4D.push_back(headRight);
	headPts = pt4D;
	vector<Eigen::Vector2i> pt4D_bis;
	pt4D_bis.push_back(headUp_left);
	pt4D_bis.push_back(headLeft);
	pt4D_bis.push_back(headRight);
	pt4D_bis.push_back(headUp_right);
	//# Compute slope of each line of the polygon
	Eigen::Vector3d HeadSlope[4];
	findSlope(pt4D[0], pt4D_bis[0], HeadSlope[0]);
	findSlope(pt4D[1], pt4D_bis[1], HeadSlope[1]);
	findSlope(pt4D[2], pt4D_bis[2], HeadSlope[2]);
	findSlope(pt4D[3], pt4D_bis[3], HeadSlope[3]);
	//# reference point
	Eigen::Vector2d midpoint = Eigen::Vector2d(skeleton[Head].x(), skeleton[Head].y());
	double ref[4] = { distEQ2Pt(HeadSlope[0], midpoint) ,distEQ2Pt(HeadSlope[1], midpoint),
		distEQ2Pt(HeadSlope[2],midpoint),distEQ2Pt(HeadSlope[3],midpoint) };
	//# fill up the polygon

	cv::Mat mask = PolygonOpt(HeadSlope[0], ref[0]);
	mask = mask.mul(PolygonOpt(HeadSlope[1], ref[1]));
	mask = mask.mul(PolygonOpt(HeadSlope[2], ref[2]));
	mask = mask.mul(PolygonOpt(HeadSlope[3], ref[3]));
	mask.setTo(0, depthImage_raw == 0);
	head = mask.clone();
	maskImage[BODY_PART_HEAD].setTo(1.0, head > 0);

	return;
}

void Segmentation::SegmentHand(int side)
{

	string window = "HandR";
	//# Right side
	int	idx = HandRight;//#hand
	int	wrist = WristRight;//#wrist
	int	handtip = 23; //#hand tip
	int	handtip1 = 21;
	int	thumb = 24;
	int	idx1 = HandLeft; //# the other hand
	
	// Left side
	if(side == 1) {
		idx = HandLeft;
		wrist = WristLeft;
		handtip = 21;
		handtip1 = 23;
		thumb = 22;
		idx1 = HandRight;//# the other hand
		window = "HandL";
	}
	//cout << "Segment" << window << endl;

	//#create a sphere of radius 12 so that anything superior does not come in the feet label
	int handDist = 25;
	//#since feet are on the same detph as the floor some processing are required before using cc
	int	line = depthImage.rows;
	int col = depthImage.cols;
	cv::Mat	mask(line, col, 2);

	
	int centerpos = (skeleton[idx].x() + skeleton[idx1].x())*1.0 / 2;

	if (abs(skeleton[idx].x() - skeleton[idx1].x()) < 0.001) {
		//cout << "junctions of two hand are in the same x axis" << endl;
		if (side == 0) {
			skeleton[idx].x() = skeleton[idx].x() + 0.1;
			skeleton[idx1].x() = skeleton[idx1].x() - 0.1;
		}
		else {
			skeleton[idx].x() -= 0.1;
			skeleton[idx1].x() -= 0.1;
		}
	}
	//mask2 = np.ones([line, col])*centerpos;
	if (skeleton[idx].x() - centerpos >= 0) {
		//mask2 = (ind[:, : , 0] - mask2) >= 0;
	}
	else {
		//mask2 = (ind[:, : , 0] - mask2) <= 0;
	}
	//mask = mask * binaryImage * mask2;

	// compute the body part as it is done for the head
	//labeled, n = spm.label(mask)
	//threshold = labeled[pos2D[idx, 1], pos2D[idx, 0]]

	Eigen::Vector2i handpos = skeleton[idx];
	Eigen::Vector2i wrist2hand = skeleton[idx] - skeleton[wrist];
	double handz = depthImage_raw.at<unsigned short>((int)skeleton[idx].y(), (int)skeleton[idx].x());
	cv::Mat tmp = cv::Mat::zeros(depthImage.size(), CV_8UC1);
	cv::circle(tmp, cv::Point(handpos.x(), handpos.y()), 25, 1, -1);

	tmp.setTo(0, maskImage[BODY_PART_TORSOR]);
	tmp.setTo(0, depthImage_raw > handz + 80.0*255);
	tmp.setTo(0, depthImage_raw < handz - 80.0*255);
	if (side == 0) {
		tmp.setTo(0, maskImage[BODY_PART_FOREARM_R]);
		tmp.setTo(0, depthImage_raw == 0);
		tmp.setTo(0, maskImage[BODY_PART_UPPER_ARM_R]);
		tmp.setTo(0, maskImage[BODY_PART_UP_LEG_R]);

		handRight = tmp.clone();
		maskImage[BODY_PART_HAND_R] = handRight.clone();
	}
	else {
		tmp.setTo(0, depthImage_raw == 0);
		tmp.setTo(0, maskImage[BODY_PART_FOREARM_L]);
		tmp.setTo(0, maskImage[BODY_PART_UPPER_ARM_L]);
		tmp.setTo(0, maskImage[BODY_PART_UP_LEG_L]);

		handLeft = tmp.clone();
		maskImage[BODY_PART_HAND_L] = handLeft.clone();
	}

}

void Segmentation::SegmentFoot(int side) {

	int idx, disidx, idx1;

	//Right Side
	if (side == 0) {
		idx = FootRight;
		disidx = 18; //ankle
		idx1 = 15; //foot of another side
	}
	else {// Left Side
		idx = FootLeft;
		disidx = 14;
		idx1 = 19;
	}
	int footDist = 10;

	//基本は中心からfootdistの距離の範囲が足だと判断
	//radius search
	cv::Mat tmp = cv::Mat::zeros(depthImage.size(), CV_8UC1);
	cv::circle(tmp, cv::Point(skeleton[idx].x(), skeleton[idx].y()), footDist, 1, -1);
	if (side == 0) {
		footRight = tmp.clone();
		maskImage[BODY_PART_FOOT_R] = footRight.clone();
	}
	else {
		footLeft = tmp.clone();
		maskImage[BODY_PART_FOOT_L] = footLeft.clone();

	}
}

void Segmentation::SegmentBody()
{
	vector<Eigen::Vector2i> pt;
	//pt.push_back(peakshoulderL);
	//pt.push_back(peakshoulderR);
	pt.push_back(Eigen::Vector2i(peakshoulderL.x(), skeleton[Neck].y()));
	pt.push_back(Eigen::Vector2i(peakshoulderR.x(), skeleton[Neck].y()));
	pt.push_back(thighPtsR[1]);
	pt.push_back(thighPtsL[0]);

	//Get slopes for each line of the polygon
	Eigen::Vector3d eq[4];
	findSlope(pt[0], pt[1], eq[0]);
	findSlope(pt[1], pt[2], eq[1]);
	findSlope(pt[2], pt[3], eq[2]);
	findSlope(pt[3], pt[0], eq[3]);

	//maskImage[BODY_PART_TORSOR] = getmask(pt, ShoulderLeft, HipRight);
	maskImage[BODY_PART_TORSOR] = getmask(pt, Neck, HipRight, "").clone();
	maskImage[BODY_PART_TORSOR] += getmask(pt, Neck, HipLeft, "").clone();
}

void Segmentation::Draw()
{
	//for (int i = 0; i < BODY_PART_NUMBER; i++)
	//{
	//	imshow(to_string(i), maskImage[i]*255);
	//}
	//imshow("Segment->Draw:raw", depthImage_raw*255);
	//imshow("Segment->Draw", depthImage*255);
	imshow("humanMaskImage", (humanMaskImage>0) * 255);
	waitKey(1);
}

void Segmentation::Setparam(Mat inputDepth, vector<Vec2i> inputSkeleton)
{
	skeleton.clear();
	depthImage = inputDepth.clone();
	depthImage_raw = inputDepth.clone();
	for (auto input : inputSkeleton) {
		skeleton.push_back(Eigen::Vector2i(input(0), input(1)));
	}

	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		maskImage[i] = cv::Mat::zeros(depthImage.size(), CV_8UC1);
	}
	humanMaskImage = Mat::zeros(depthImage.size(), CV_8UC1);

}

void Segmentation::Setparam(Mat inputDepth, vector<Eigen::Vector2i> inputSkeleton) {
	skeleton.clear();
	skeleton.shrink_to_fit();
	depthImage = inputDepth.clone();
	depthImage_raw = inputDepth.clone();
	for (auto input : inputSkeleton) {
		skeleton.push_back(input);
	}

	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		maskImage[i] = cv::Mat::zeros(depthImage.size(), CV_8UC1);
	}
	humanMaskImage = Mat::zeros(depthImage.size(), CV_8UC1);
}

void Segmentation::Setparam(Mat inputDepth, vector<Vec2> inputSkeleton) {
	depthImage_raw = Mat::zeros(inputDepth.size(), CV_8UC1);
	depthImage = Mat::zeros(inputDepth.size(), CV_8UC1);
	depthImage_raw = inputDepth.clone();
	depthImage = inputDepth.clone();
	skeleton.clear();
	skeleton.shrink_to_fit();
	for (auto input : inputSkeleton) {
		skeleton.push_back(Eigen::Vector2i(input.x, input.y));
	}

	for (int i = 0; i < BODY_PART_NUMBER; i++)
	{
		maskImage[i] = cv::Mat::zeros(depthImage.size(), CV_8UC1);
	}
	humanMaskImage = Mat::zeros(depthImage.size(), CV_8UC1);
}

void Segmentation::findSlope(Eigen::Vector2i pt1, Eigen::Vector2i pt2, Eigen::Vector3d& output)
{
	if (pt1 == pt2) {
		//cout << "pt1 & pt2 : " << pt1 << " / same point" << endl;
		output(0) = 0.0;
		output(1) = 0.0;
		output(2) = 0.0;
		return;
	}

	double diffY = pt2(1) - pt1(1);
	double diffX = pt1(0) - pt2(0);
	double dist = sqrt(diffX*diffX + diffY*diffY);
	double a = diffY / dist;
	double b = diffX / dist;
	double c = -a*pt1(0) - b*pt1(1);
	output(0) = a;
	output(1) = b;
	output(2) = c;
}


void Segmentation::inferedPoint(double a, double b, double c, Eigen::Vector2i pos, float T, Eigen::Vector2i  &res1, Eigen::Vector2i &res2)
{/*
 Find two points that are the corners of the segmented part
 : param A : Depth Image
 : param a : dist x axe between two points
 : param b : dist y axe between two points
 : param c : constant of this line
 : param point : a junction
 : param T : max distance to find intersection
 : return : two intersection points between a slope and the edges of the body part
 /**/

	int line = depthImage.size[0];
	int	col = depthImage.size[1];
	bool process_y = abs(a) > abs(b);
	int x = 0;
	int y = 0;
	Eigen::Vector2d point = Eigen::Vector2d(pos.x(),pos.y());
	float thres = T;
	while (true)
	{
		Eigen::Vector2d vec = Eigen::Vector2d(b, -a).normalized();
		point += vec;
		bool inImage = (point.x() >= 0) && (point.x() < col) && (point.y() >= 0) && (point.y() < line);
		if (inImage) {
			if (depthImage_raw.at<unsigned short>(int(point.y()), int(point.x())) == 0) {
				point -= vec;
				break;
			}
			else {
				double distCdt = (point - Eigen::Vector2d(pos.x(),pos.y())).norm();
				if (distCdt > thres) {
					point -= vec;
					break;
				}
			}
		}
		else {
			point -= vec;
			break;
		}
	}
	res1 = Eigen::Vector2i(point.x(),point.y());

	point = Eigen::Vector2d(pos.x(), pos.y());
	while (true)
	{
		Eigen::Vector2d vec = Eigen::Vector2d(-b, a).normalized();
		point += vec;
		bool inImage = (point.x() >= 0) && (point.x() < col) && (point.y() >= 0) && (point.y() < line);
		if (inImage) {
			if (depthImage_raw.at<unsigned short>(int(point.y()), int(point.x())) == 0) {
				point -= vec;
				break;
			}
			else {
				double distCdt = (point - Eigen::Vector2d(pos.x(), pos.y())).norm();
				if (distCdt > thres) {
					point -= vec;
					break;
				}
			}
		}
		else {
			point -= vec;
			break;
		}
	}
	res2 = Eigen::Vector2i(point.x(), point.y());
	if (res1 == res2) {
		res1 += Eigen::Vector2i(1, 1);
		res2 -= Eigen::Vector2i(1, 1);
	}

}

Mat Segmentation::PolygonOpt(Eigen::Vector3d slopes, float ref)
{
	/*
	Test the sign of alpha = (a[k] * j + b[k] * i + c[k])*ref[k]
	to know whether a point is within a polygon or not
	: param slopes : list of slopes defining a the border lines of the polygone
	: param ref : a point inside the polygon
	: param limit : number of slopes
	: return : the body part filled with true.
	*/
	
	int line = depthImage.rows;
	int	col = depthImage.cols;
	//cv::Mat res = cv::Mat::zeros(480, 640, CV_16UC1);
	cv::Mat res = cv::Mat::zeros(cDepthHeight,cDepthWidth, CV_8UC1);

	//create a matrix containing in each pixel its indices
	//mask the res image by body part category
	for (int i = 0; i < cDepthWidth; i++)
	{
		for (int j = 0; j < cDepthHeight; j++)
		{
			float other = slopes[0] * i + slopes[1] * j + slopes[2];
			if (ref*other >= 0) {
				//res.at<unsigned short>(j, i) = 1.0;
				res.at<unsigned char>(j, i) = 1.0;
			}
			else {
				//res.at<unsigned short>(j, i) = 0.0;
				res.at<unsigned char>(j, i) = 0.0;
			}
		}
	}
	return res;

}

Mat Segmentation::getmask(vector<Eigen::Vector2i> intersection, int point1, int point2, string title) {
	/*
	パーツのボックスの4点とスケルトンをとってきて　その中を塗りつぶしてマスクとする
	param intersection : corner of the body part
	param point1,point2 : skeleton number of the body part

	param title : window title if it need
	*/

	//fill the polygon
	cv::Mat res, res0;
	cv::Mat mask0, mask1, mask2, mask3;

	//Get slopes for each line of the polygon
	Eigen::Vector3d eq[4];
	findSlope(intersection[0], intersection[1], eq[0]);
	findSlope(intersection[1], intersection[2], eq[1]);
	findSlope(intersection[2], intersection[3], eq[2]);
	findSlope(intersection[3], intersection[0], eq[3]);


	//erase all NaN in the array
	// NANの消去が未実装
	Eigen::Vector3d polygonSlope[4];
	for (int i = 0; i < 4; i++)
	{
		polygonSlope[i] = eq[i];
	}

	// get reference point
	Eigen::Vector2d midpoint((skeleton[point1].x() + skeleton[point2].x()) / 2, (skeleton[point1].y() + skeleton[point2].y()) / 2);
	float ref[4] = { distEQ2Pt(polygonSlope[0],midpoint),distEQ2Pt(polygonSlope[1],midpoint),
		distEQ2Pt(polygonSlope[2],midpoint),distEQ2Pt(polygonSlope[3],midpoint) };

	//(depthimage*PolygonOpt(*polygonSlope, ref, 4));
	mask0 = PolygonOpt(polygonSlope[0], ref[0]).clone();
	mask1 = PolygonOpt(polygonSlope[1], ref[1]).clone();
	mask2 = PolygonOpt(polygonSlope[2], ref[2]).clone();
	mask3 = PolygonOpt(polygonSlope[3], ref[3]).clone();

	res0 = mask0.mul(mask1);
	res0 = res0.mul(mask2);
	res0 = res0.mul(mask3);
	//res.setTo(1, depthImage_raw > 0);
	res0 = res0.mul(depthImage_raw > 0);
	return res0.clone();
}

void Segmentation::CheckIntersection(Eigen::Vector2i &pt1, Eigen::Vector2i &pt2) {
	if (pt1.x() > pt2.x()) { swap(pt1, pt2); }
}

Mat Segmentation::GetHumanbodyMask() { return humanMaskImage; }
Mat Segmentation::GetHumanDeoth()
{
	Mat res;
	depthImage_raw.copyTo(res, humanMaskImage > 0);
	return res;
}
Mat Segmentation::GetMaskImage(int num) { return maskImage[num]; }