#pragma once

#define _USE_MATH_DEFINES //Use Math Defines
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glut32.lib")

#include "main.h"
#include "DirectoryConfig.h"

//GL
//#include"opengl\glew-2.1.0\include\GL\glew.h"
//#include"opengl\glut-3.7.6-bin\glut.h"
#include <gl\glew.h>
#include <GL\glut.h>

//CL
#define __CL_ENABLE_EXCEPTIONS
#include<CL/cl.hpp>
#include<CL\cl_gl.h>


//Kinect 
#include "Kinect.h"

//#include "ITMHeader.h"


/*
correspondance between number and body parts and color
background should have : color = [0, 0, 0] = #000000     black                 label = 0
armLeft[0] = forearmL      color = [0, 0, 255] = #0000ff     blue                  label = 1
armLeft[1] = upperarmL     color = [200, 200, 255] = #ffc8ff     very light blue       label = 2
armRight[0] = forearmR      color = [0, 255, 0] = #00ff00     green                 label = 3
armRight[1] = upperarmR    color = [200, 255, 200] = #c8ffc8     very light green      label = 4
legRight[0] = thighR       color = [255, 0, 255] = #ff00ff     purple                label = 5
legRight[1] = calfR        color = [255, 180, 255] = #ffb4ff     pink                  label = 6
legLeft[0] = thighL        color = [255, 255, 0] = #ffff00     yellow                label = 7
legLeft[1] = calfL         color = [255, 255, 180] = #ffffb4     very light yellow     label = 8
head = headB               color = [255, 0, 0] = #ff0000     red                   label = 9
body = body                color = [255, 255, 255] = #ffffff     white                 label = 10
handRight = right hand     color = [0, 191, 255] = #00bfff     turquoise             label = 11
handLeft = left hand       color = [0, 100, 0] = #006400     dark green            label = 12
footRight = right foot     color = [199, 21, 133] = #c715ff     dark purple           label = 13
footLeft = left foot       color = [255, 165, 0] = #ffa500     orange                label = 14

/**/

//#define VMAP_KER 0
//#define NMAP_KER 1
//#define BUMP_KER 2
//#define NMAPBUMP_KER 3
//#define VMAPBUMP_KER 4
//#define DENSEBS_KER 5
//#define BSSYSTEM_KER 6
//#define REDUCE_KER 7
//#define DATAPROC_KER 8
//#define GICP_KER 9
//#define REDUCEGICP_KER 10
//#define JACOBI_KER 11
//#define SYSTEMPR_KER 12
//#define SOLVEPR_KER 13
//#define SYSTEMPRB_KER 14
//#define REDUCE1_KER 15
//#define REDUCE2_KER 16
//#define PSEUDOINV_KER 17
//#define ATC_KER 18
//#define REDSOLVE_KER 19
//#define BILATERAL_KER 20
//#define MEDIANFILTER_KER 21

// TODO: reference additional headers your program requires here

extern int        cDepthWidth;
extern int        cDepthHeight;
extern float		NII;
extern int        cColorWidth;
extern int        cColorHeight;


static const int		FRAME_BUFFER_SIZE = 100;

extern float Calib[11];
const string srcOpenCL =  strData + "../src/OpenCL Kernels/";

#define divUp(x,y) (x%y) ? ((x+y-1)/y) : (x/y)

inline void checkErr(cl_int err, const char * name) {
	if (err != CL_SUCCESS) {
		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
		//exit(EXIT_FAILURE);
	}
}

struct Vec2
{
	double x;
	double y;

	Vec2() = default;

	constexpr Vec2(double _x, double _y)
		: x(_x)
		, y(_y) {}

	Vec2(Vec2i p)
		: x(p(0))
		, y(p(1)){}
	
	double length() const
	{
		return std::sqrt(lengthSquare());
	}

	constexpr double lengthSquare() const
	{
		return dot(*this);
	}

	constexpr double dot(const Vec2& other) const
	{
		return x * other.x + y * other.y;
	}

	double distanceFrom(const Vec2& other) const
	{
		return (other - *this).length();
	}

	Vec2 normalized() const
	{
		if (length() == 0)return *this;
		else return *this / length();
	}

	constexpr bool isZero() const
	{
		return x == 0.0 && y == 0.0;
	}

	constexpr Vec2 operator +() const
	{
		return *this;
	}

	constexpr Vec2 operator -() const
	{
		return{ -x, -y };
	}

	constexpr Vec2 operator +(const Vec2& other) const
	{
		return{ x + other.x, y + other.y };
	}

	constexpr Vec2 operator -(const Vec2& other) const
	{
		return{ x - other.x, y - other.y };
	}

	constexpr Vec2 operator *(double s) const
	{
		return{ x * s, y * s };
	}

	constexpr Vec2 operator /(double s) const
	{
		return{ x / s, y / s };
	}

	Vec2& operator +=(const Vec2& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	Vec2& operator -=(const Vec2& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vec2& operator *=(double s)
	{
		x *= s;
		y *= s;
		return *this;
	}

	Vec2& operator /=(double s)
	{
		x /= s;
		y /= s;
		return *this;
	}

	bool operator ==(const Vec2& other) {
		if (x == other.x && y == other.y) return true;
		else return false;
	}
};

template <class Char>
inline std::basic_ostream<Char>& operator <<(std::basic_ostream<Char>& os, const Vec2& v)
{
	return os << Char('(') << v.x << Char(',') << v.y << Char(')');
}

template <class Char>
inline std::basic_istream<Char>& operator >> (std::basic_istream<Char>& is, Vec2& v)
{
	Char unused;
	return is >> unused >> v.x >> unused >> v.y >> unused;
}

struct Vec3
{
	double x;
	double y;
	double z;

	Vec3() = default;

	constexpr Vec3(double _x, double _y, double _z)
		: x(_x)
		, y(_y) 
		, z(_z){}

	Vec3(Vec3d p)
		: x(p(0))
		, y(p(1))
		, z(p(2)) {}

	double length() const
	{
		return std::sqrt(lengthSquare());
	}

	constexpr double lengthSquare() const
	{
		return dot(*this);
	}

	constexpr double dot(const Vec3& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	double distanceFrom(const Vec3& other) const
	{
		return (other - *this).length();
	}

	Vec3 cross(const Vec3& other)const {
		double a = this->y*other.z - this->z*other.y;
		double b = this->z*other.x - this->x*other.z;
		double c = this->x*other.y - this->y*other.x;
		return Vec3(a, b, c);
	}

	Vec3 normalized() const
	{
		if (length() == 0.0)return *this;
		else return *this / length();
	}

	constexpr bool isZero() const
	{
		return x == 0.0 && y == 0.0 && z==0.0;
	}

	constexpr Vec3 operator +() const
	{
		return *this;
	}

	constexpr Vec3 operator -() const
	{
		return{ -x, -y , -z };
	}

	constexpr Vec3 operator +(const Vec3& other) const
	{
		return{ x + other.x, y + other.y ,z + other.z };
	}

	constexpr Vec3 operator -(const Vec3& other) const
	{
		return{ x - other.x, y - other.y , z - other.z };
	}

	constexpr Vec3 operator *(double s) const
	{
		return{ x * s, y * s , z*s };
	}

	constexpr Vec3 operator /(double s) const
	{
		return{ x / s, y / s , z / s };
	}

	Vec3& operator +=(const Vec3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vec3& operator -=(const Vec3& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	Vec3& operator *=(double s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	Vec3& operator /=(double s)
	{
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}

	bool operator ==(const Vec3& other) {
		if (x == other.x && y == other.y && z == other.z) return true;
		else return false;
	}
};

