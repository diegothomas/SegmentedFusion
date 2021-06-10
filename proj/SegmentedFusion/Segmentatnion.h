#pragma once
#include "main.h"
#include "Functions.h"


class Segmentation
{
public:
	Mat getmask(vector<Eigen::Vector2i> intersection, int point1, int point2, string title);
	void CheckIntersection(Eigen::Vector2i & pt1, Eigen::Vector2i & pt2);
	Mat GetHumanbodyMask();
	Mat GetHumanDeoth();
	Mat GetMaskImage(int num);
	Segmentation();
	Segmentation(Mat inputDepth, vector<Vec2i> inputSkeleton);
	Segmentation(Mat inputDepth, vector<Eigen::Vector2i> inputSkeleton);
	Segmentation(Mat inputDepth, vector<Vec2> inputSkeleton);
	~Segmentation();

	void Run();
	void SegmentArm(int side = 0);
	void SegmentLeg(int side = 0);
	void SegmentHead();
	void SegmentHand(int side = 0);
	void SegmentFoot(int side);
	void SegmentBody();
	void Draw();

	void Setparam(Mat inputDepth, vector<Vec2i> inputSkeleton);
	void Setparam(Mat inputDepth, vector<Eigen::Vector2i> inputSkeleton);
	void Setparam(Mat inputDepth, vector<Vec2> inputSkeleton);

	// Get the slope pt1 to pt2
	// Return 3 values @ ax + by + c = 0 :Eigen::Vec3i(a,b,c)
	void findSlope(Eigen::Vector2i pt1, Eigen::Vector2i pt2, Eigen::Vector3d&);

	void inferedPoint(double a, double b, double c, Eigen::Vector2i pos, float T, Eigen::Vector2i & res1, Eigen::Vector2i & res2);
	Mat PolygonOpt(Eigen::Vector3d slopes, float ref);
	inline double distEQ2Pt(Eigen::Vector3d eq, Eigen::Vector2d pt) {
		return double(eq(0) * pt.x() + eq(1) * pt.y() + eq(2));
	}
	Mat depthImage, depthImage_raw;
private:
	vector<Eigen::Vector2i> skeleton;
	//float Calib[11] = { 580.8857f, 583.317f, 319.5f, 239.5f, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 8000.0 };

	Eigen::Vector2i peakshoulderL, peakshoulderR;

	vector<Eigen::Vector2i> upperArmPtsL, upperArmPtsR;
	//leg  slope?
	vector<Eigen::Vector2i> thighPtsR, thighPtsL;
	vector<Eigen::Vector2i> calfPtsR, calfPtsL;
	//arm slope?
	vector<Eigen::Vector2i> foreArmPtsR, foreArmPtsL;
	//head
	vector<Eigen::Vector2i> headPts;
	//body
	vector<Eigen::Vector2i> bodyPts;


	//mask image @ each body part
	Mat armLeft, armRight, legLeft, legRight;
	Mat head, handLeft, handRight, footLeft, footRight;
	Mat MidBody;
	Mat humanbody;

	Mat humanMaskImage;

	Mat maskImage[BODY_PART_NUMBER];
};
