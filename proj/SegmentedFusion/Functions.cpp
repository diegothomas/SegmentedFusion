#include "Functions.h"

/////////////////Not in Class But use///////////////////////
cl_kernel LoadKernel(string filename, string Kernelname, cl_context context, cl_device_id device) {
	cl_int ret;
	std::ifstream file(filename);
	checkErr(file.is_open() ? CL_SUCCESS : -1, filename.c_str());
	std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
	const char * code = prog.c_str();
	cl_program lProgram = clCreateProgramWithSource(context, 1, &code, 0, &ret);
	ret = clBuildProgram(lProgram, 1, &device, "", 0, 0);
	checkErr(ret, "Program::build()");

	cl_kernel kernel = clCreateKernel(lProgram, Kernelname.c_str(), &ret);
	checkErr(ret, (Kernelname + string("::Kernel()")).c_str());
	return kernel;
}

void getMatrixfromDualQuaternion(float DQ[], Eigen::Matrix4f outmat)
{
	Eigen::Quaternionf real(DQ[0], DQ[1], DQ[2], DQ[3]);
	real.normalize();
	Eigen::Quaternionf dual(0, DQ[5], DQ[6], DQ[7]);

	dual = dual * real;
}

vector<string> Split(string& input, char delimiter)
{
	istringstream stream(input);
	string field;
	vector<string> result;
	while (getline(stream, field, delimiter)) {
		result.push_back(field);
	}
	return result;
}

Mat LoadOneImage(string path)
{
	Mat tmpIm = imread(path, CV_LOAD_IMAGE_UNCHANGED);
	if (!tmpIm.data) {
		cout << path << " is load error" << endl;
		tmpIm = Mat::zeros(cDepthHeight, cDepthWidth, CV_8UC3);
	}
	return tmpIm;
}

Vec3d GetProjectedPoint(Vec2 pt, float d)
{
	Vec3d res;
	int i = pt.y; int j = pt.x;
	res(0) = d == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * d * ((float(j) - Calib[2]) / Calib[0]);
	res(1) = d == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * d * ((float(i) - Calib[3]) / Calib[1]);
	res(2) = d == 0.0 ? 0.0 : (Calib[9] / Calib[10]) * d;
	return res;
}

Vec3d GetProjectedPoint(Vec2 pt)
{
	return Vec3d();
}

void Line3D(Vec3 pt1, Vec3 pt2, float size) {
	glLineWidth(size);
	//glColor4f(1.0, 0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(GLfloat(pt1.x), -GLfloat(pt1.y), -GLfloat(pt1.z));
	glVertex3f(GLfloat(pt2.x), -GLfloat(pt2.y), -GLfloat(pt2.z));
	glEnd();
}

void Line3D(Vec3 pt1, Vec3 pt2, GLfloat color[4]) {
	glLineWidth(2.0);
	glColor4f(1.0, 0.0, 0.0, 1.0);
	glColor4f(color[0], color[1], color[2], color[3]);
	glBegin(GL_LINES);
	glVertex3f(GLfloat(pt1.x), -GLfloat(pt1.y), -GLfloat(pt1.z));
	glVertex3f(GLfloat(pt2.x), -GLfloat(pt2.y), -GLfloat(pt2.z));
	glEnd();
}

void Line2D(Vec2 pt1, Vec2 pt2, cv::Mat image) {
	cv::Point _pt1(int(pt1.x), int(pt1.y));
	cv::Point _pt2(int(pt2.x), int(pt2.y));
	cv::line(image, _pt1, _pt2, cv::Scalar(0, 0, 255));

}

void CVMat2EigenMatrix(Mat4d &input, Eigen::Matrix4d &output) {
	for (int i = 0; i < 4; i++)
	{
		output(0, i) = input.at<double>(0, i);
		output(1, i) = input.at<double>(1, i);
		output(2, i) = input.at<double>(2, i);
		output(3, i) = input.at<double>(3, i);
	}

}

/////////////////////////////////////////