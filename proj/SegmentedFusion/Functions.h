#pragma once
#include "Header.h"

void Line3D(Vec3 pt1, Vec3 pt2, float size = 2.0);

void Line3D(Vec3 pt1, Vec3 pt2, GLfloat color[4]);

void Line2D(Vec2 pt1, Vec2 pt2, cv::Mat image);

void CVMat2EigenMatrix(Mat4d & input, Eigen::Matrix4d & output);

void GetPoint(cv::Mat &_VMap, Vec2 &pt2D, Vec3 & pt);

void getDualQuaternionfromMatrix(Eigen::Matrix4f mat, float DQ[8]);

cl_kernel LoadKernel(string filename, string Kernelname, cl_context context, cl_device_id device);

void getMatrixfromDualQuaternion(float DQ[], Eigen::Matrix4f outmat);

vector<string> Split(string & input, char delimiter);

Mat LoadOneImage(string path);

Vec3d GetProjectedPoint(Vec2 pt, float d);

Vec3d GetProjectedPoint(Vec2 pt);