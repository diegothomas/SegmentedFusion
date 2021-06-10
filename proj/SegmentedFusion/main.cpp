//  include

#include "GLUIEngine.h"
#include "RGBDUI.h"


///////////////////////////////////////

//static char*			FILENAME = "D:\\dataset\\Data_SegmentedFusion\\dataset08/";
static char*			FILENAME = "D:\\dataset\\Data_SegmentedFusion/projected_images/walk/";
//static char*			FILENAME = "D:\\dataset\\Data_SegmentedFusion\\niidataset02/";

//float		NII = 2.0;
//int			cDepthWidth = 640;
//int			cDepthHeight = 480;
//int        cColorWidth = 640;
//int        cColorHeight = 480;
//float Calib[11] = { 580.8857f, 583.317f, cDepthWidth / 2.0, cDepthHeight / 2.0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 8000.0f }; // Kinect data
//																															   

//float Calib[11] = { 357.324, 362.123, 250.123, 217.526, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 690.0 }; // silvia
float Calib[11] = { 357.324, 362.123, 250.123, 217.526, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1000.0 }; // silvia
int       cDepthWidth = 512;
int		cDepthHeight = 424;
int        cColorWidth = 1920;
int        cColorHeight = 1080;
float		NII = 1.0;



cl_context _context;
cl_device_id _device;
typedef CL_API_ENTRY cl_int(CL_API_CALL *P1)(const cl_context_properties *properties, cl_gl_context_info param_name, size_t param_value_size, void *param_value, size_t *param_value_size_ret);

CL_API_ENTRY cl_int(CL_API_CALL *myclGetGLContextInfoKHR) (const cl_context_properties *properties, cl_gl_context_info param_name, size_t param_value_size, void *param_value, size_t *param_value_size_ret) = NULL;


int Init() {
	//////////////////////////////////////////////////////////////
	//////////////////////////OpenCL//////////////////////////
	//////////////////////////////////////////////////////////////
	cl_device_id device_id = NULL;
	cl_platform_id platform_id = NULL;
	cl_context context;

	/*** Initialise OpenCL and OpenGL interoperability***/
	// Get platforms.
	cl_uint lNbPlatformId = 0;
	clGetPlatformIDs(0, 0, &lNbPlatformId);


	if (lNbPlatformId == 0)
	{
		std::cerr << "Unable to find an OpenCL platform." << std::endl;
		return -1;
	}

	// Loop on all platforms.
	std::vector< cl_platform_id > platformList(lNbPlatformId);
	clGetPlatformIDs(lNbPlatformId, platformList.data(), 0);

	std::cerr << "Platform number is: " << lNbPlatformId << std::endl;
	char platformVendor[10240];
	clGetPlatformInfo(platformList[0], (cl_platform_info)CL_PLATFORM_VENDOR, 10240, platformVendor, NULL);
	std::cerr << "Platform is by: " << platformVendor << "\n";

	//myclGetGLContextInfoKHR = (P1)clGetExtensionFunctionAddressForPlatform(platformList[0], "clGetGLContextInfoKHR");

	cl_uint lNbDeviceId = 0;
	clGetDeviceIDs(platformList[0], CL_DEVICE_TYPE_GPU, 0, 0, &lNbDeviceId);

	if (lNbDeviceId == 0)
	{
		return -1;
	}

	std::vector< cl_device_id > lDeviceIds(lNbDeviceId);
	clGetDeviceIDs(platformList[0], CL_DEVICE_TYPE_GPU, lNbDeviceId, lDeviceIds.data(), 0);


	//Additional attributes to OpenCL context creation
	//which associate an OpenGL context with the OpenCL context 
	cl_context_properties props[] =
	{
		//OpenCL platform
		CL_CONTEXT_PLATFORM, (cl_context_properties)platformList[0],
		//OpenGL context
		CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
		//HDC used to create the OpenGL context
		CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
		0
	};
	cl_int lError = CL_SUCCESS;
	_context = clCreateContext(props, 1, &lDeviceIds[0], 0, 0, &lError);
	context = _context;
	_device = lDeviceIds[0];
	//context = clCreateContext(props, 1, &lDeviceIds[0], 0, 0, &lError);
	if (lError != CL_SUCCESS)
	{
		cout << "device unsupported" << endl;
	}
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
//int main(int argc, char** argv)
{
	cout << "Program Start" << endl;
	bool RGBDM = false;
	if (!RGBDM) {
		GLUIEngine::Instance()->Initialise(argc, argv);
		if (Init() != 0)return 1;
		GLUIEngine::Instance()->Run(_context, _device);
		GLUIEngine::Instance()->Shutdown();
	}
	else {
		RGBInitialize(argc, argv);
		if (Init() != 0)return 1;
		RGBFunction(_context, _device);
	}
	cout << "end" << endl;
	return 0;
}

