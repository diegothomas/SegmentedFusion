
#include "RGBDUI.h"
#include "RGBDMnager.h"
#include "ITMManager.h"

//GLuint window;
/*** Camera variables for OpenGL ***/

GLuint window;
/*** Camera variables for OpenGL ***/
GLfloat intrinsics[16] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
float Znear = 0.05f;
float Zfar = 10.0f;
GLfloat light_pos[] = { 0.0, 0.0, 2.0, 0.0 }; //{ 1.0, 1.0, 0.0, 0.0 };
GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };

// angnamespaceotation for the camera direction
float anglex = 0.0f;
float angley = 0.0f;
// actual vector representing the camera's direction
float lx = 0.0f, ly = 0.0f, lz = -1.0f;
float lxStrap = -1.0f, lyStrap = 0.0f, lzStrap = 0.0f;
// XZ position of the camera
float x = 0.0f, y = 0.0f, z = 0.1f; //0.15f;//
float deltaAnglex = 0.0f;
float deltaAngley = 0.0f;
float deltaMove = 0;
float deltaStrap = 0;
int xOrigin = -1;
int yOrigin = -1;



bool Running = false;
bool start = false;
bool stop = false;
float my_count;
float fps;
bool first;
int anim_indx = 0;
bool save_img = false;
bool inverted = false;

clock_t current_time;
clock_t last_time;
clock_t starttime, endtime;


static const float scale_rotation = 0.005f;
static const float scale_translation = 0.0025f;

//static char*			FILENAME = "D:\\dataset\\Data_segmentedfusion/dataset_3cameras/camera_2/";
//static char*			FILENAME = "D:\\dataset\\Data_segmentedfusion/projected_images/walk/";
static char*			FILENAME = "D:\\dataset\\Data_segmentedfusion/dataset35/";

RGBDMnager *RGBD = NULL;
unique_ptr<MyITMNamespase::ITMManager> ITMMng;
cl_context context; cl_device_id device;

void computePos(float deltaMove, float deltaStrap);
void setCameraPoseFunction();
bool FrameFunction(int check) {
	starttime = clock();
	check = RGBD->Load();
	endtime = clock();
	starttime = clock();

	if (check == 3) {
		cout << "Load error" << endl;
		return false;
	}
	else if (check == 2) {
		cout << "No skeleton" << endl;
	}
	if (!Running) {
		RGBD->Init();
	}
	else {
		if (RGBD->Running()) {
			cout << "Running error" << endl;
			return false;
		}
	}
	endtime = clock();
	cout << "RGBD->Running : " << (endtime - starttime) << endl;
	Running = true;

	starttime = clock();
	Mat depth, color;
	RGBD->getBackImages(depth,color);
	ITMMng->SetImages(color, depth);
	ITMMng->Running();
	return true;
}
void ResetMovingCamera() {
	xOrigin = -1;
	yOrigin = -1;
	deltaAnglex = 0.0f;
	deltaAngley = 0.0f;
	deltaMove = 0;
	deltaStrap = 0;

	lxStrap = -1.0;
	lyStrap = 0.0;
	lzStrap = 0.0;
}

void reshape(int width_in, int height_in) {
	glViewport(0, 0, width_in, height_in);
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
														// Set up camera intrinsics
	glLoadMatrixf(intrinsics);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void computePos(float deltaMove, float deltaStrap) {

	x += deltaMove * lx * 0.1f + deltaStrap * lxStrap * 0.1f;
	y += deltaMove * ly * 0.1f + deltaStrap * lyStrap * 0.1f;
	z += deltaMove * lz * 0.1f + deltaStrap * lzStrap * 0.1f;
}

void saveimg(int x, int y, int id, char *path) {
	float *image = new float[3 * cDepthWidth*cDepthHeight];
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadBuffer(GL_FRONT);
	glReadPixels(x, y, cDepthWidth, cDepthHeight, GL_RGB, GL_FLOAT, image);

	cv::Mat imagetest(cDepthHeight, cDepthWidth, CV_8UC3);
	for (int i = 0; i < cDepthHeight; i++) {
		for (int j = 0; j < cDepthWidth; j++) {
			imagetest.at<cv::Vec3b>(cDepthHeight - 1 - i, j)[2] = unsigned char(255.0*image[3 * (i*cDepthWidth + j)]);
			imagetest.at<cv::Vec3b>(cDepthHeight - 1 - i, j)[1] = unsigned char(255.0*image[3 * (i*cDepthWidth + j) + 1]);
			imagetest.at<cv::Vec3b>(cDepthHeight - 1 - i, j)[0] = unsigned char(255.0*image[3 * (i*cDepthWidth + j) + 2]);
		}
	}

	char filename[100];
	sprintf_s(filename, "%s%d.png", path, id);
	cv::imwrite(filename, imagetest);

	delete[] image;
	image = 0;
}

void dispylay_RGBD(void) {
	RGBD->Draw();
}

void glutDisplayFunction() {
	ITMMng->Draw();
}

void display_Model() {
	//glutDisplayFunction();
	RGBD->Draw2ndScreen();

	return;
}

void display() {
	if (!Running) {
		glutSwapBuffers();
		glutPostRedisplay();
		return;
	}

	if (deltaMove || deltaStrap)
		computePos(deltaMove, deltaStrap);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
														// Set up camera intrinsics
	glLoadMatrixf(intrinsics);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set the camera
	gluLookAt(x, y, z,
		x + lx, y + ly, z + lz,
		0.0f, 1.0f, 0.0f);

	glEnable(GL_LIGHTING);
	GLfloat ambientLightq[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLightq[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLightq);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLightq);
	glEnable(GL_LIGHT0);

	glViewport(0, 0, cDepthWidth, cDepthHeight);
	dispylay_RGBD();


	glViewport(0, cDepthHeight, cDepthWidth, cDepthHeight);
	display_Model();

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	glutSwapBuffers();
	glutPostRedisplay();

}

void setCameraPoseFunction()
{

	lx = sin(anglex + deltaAnglex);
	ly = cos(anglex + deltaAnglex) * sin(-(angley + deltaAngley));
	lz = -cos(anglex + deltaAnglex) * cos(-(angley + deltaAngley));

	lxStrap = -cos(anglex + deltaAnglex);
	lyStrap = sin(anglex + deltaAnglex) * sin(-(angley + deltaAngley));
	lzStrap = -sin(anglex + deltaAnglex) * cos(-(angley + deltaAngley));

	deltaAnglex = 0.0f;
	deltaAngley = 0.0f;
}

void keyboard(unsigned char key, int _x, int _y) {
	//using namespace MyITMNamespace;
	int check = 0;
	bool shoulDelete = false;
	switch (key) {
	case 'c':
		cout << "Reset" << endl;
		RGBD->Reset();
		Running = false;
		break;
	case 'q':
		cout << "save the mesh ...";
		//mainEngine->SaveSceneToMesh("mesh.stl");
		RGBD->SavePly();
		cout << "done" << endl;
		break;
	case 'b':
		start = !start;
		if (start) {
			cout << "run mode" << endl;
		}
		else {
			cout << "one mode" << endl;
			break;
		}
		//break;
	case 's':
		FrameFunction(check);
		break;

	case 'f':
		ITMMng->KeyboardFunction(key);
		//if (freeviewActive)
		//{
		//	outImageType[0] = ITMMainEngine::InfiniTAM_IMAGE_SCENERAYCAST;
		//	outImageType[1] = ITMMainEngine::InfiniTAM_IMAGE_ORIGINAL_DEPTH;

		//	freeviewActive = false;
		//}
		//else
		//{
		//	outImageType[0] = ITMMainEngine::InfiniTAM_IMAGE_FREECAMERA_SHADED;
		//	outImageType[1] = ITMMainEngine::InfiniTAM_IMAGE_SCENERAYCAST;

		//	freeviewPose.SetFrom(mainEngine->GetTrackingState()->pose_d);
		//	if (mainEngine->GetView() != NULL) {
		//		freeviewIntrinsics = mainEngine->GetView()->calib.intrinsics_d;
		//		showImgs->ChangeDims(mainEngine->GetView()->depth->noDims);
		//	}
		//	freeviewActive = true;
		//}

		break;


		//control camera pos & pose 
	case 'r': {
		cout << "Reset camera angle" << endl;
		ResetMovingCamera();

		anglex = 0.0f;
		angley = 0.0f;

		lx = 0.0f;
		ly = 0.0f;
		lz = -1.0f;

		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		break;
	}
	case 'u': {
		cout << "Upper camera angle" << endl;
		ResetMovingCamera();
		anglex = 0.0f;
		angley = M_PI * 0.50000001f;
		x = 0.0f;
		y = 4.0f;
		z = -0.0f;
		setCameraPoseFunction();
		break;
	}
	case 'y': {
		cout << "Left camera angle" << endl;
		ResetMovingCamera();
		anglex = -M_PI * 0.5f;
		angley = 0.0f;
		x = 5.0f;
		y = 0.0f;
		z = -0.0f;
		setCameraPoseFunction();
		break;
	}
	case 'i': {
		cout << "Right camera angle" << endl;
		ResetMovingCamera();
		anglex = M_PI * 0.5f;
		angley = 0.0f;
		x = -5.0f;
		y = 0.0f;
		z = -0.0f;
		setCameraPoseFunction();
		break;
	}
	case 'j': {
		cout << "Back camera angle" << endl;
		ResetMovingCamera();
		anglex = M_PI;
		angley = 0.0f;
		x = 0.0f;
		y = 0.0f;
		z = -8.0f;
		setCameraPoseFunction();
		break;
	}

	case 27 /* Esc */:
		exit(1);
		break;
	}
}

void keyUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'a':
		break;
	default:
		break;
	}
}

void pressKey(int key, int xx, int yy) {

	switch (key) {
	case GLUT_KEY_UP: deltaMove = 0.5f / 3.0f/*(fps/30.0)*/; break;
	case GLUT_KEY_DOWN: deltaMove = -0.5f / 3.0f/*(fps/30.0)*/; break;
	case GLUT_KEY_LEFT: deltaStrap = 0.5f / 3.0f/*(fps/30.0)*/; break;
	case GLUT_KEY_RIGHT: deltaStrap = -0.5f / 3.0f/*(fps/30.0)*/; break;

	}
}

void releaseKey(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_LEFT:
	case GLUT_KEY_RIGHT:
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN: deltaMove = 0; deltaStrap = 0; break;

	}
}

void mouseMove(int x, int y) {

	// this will only be true when the left button is down
	if (xOrigin >= 0 || yOrigin >= 0) {
		// update deltaAngle
		deltaAnglex = (x - xOrigin) * 0.001f;
		deltaAngley = (y - yOrigin) * 0.001f;

		// update camera's direction
		lx = sin(anglex + deltaAnglex);
		ly = cos(anglex + deltaAnglex) * sin(-(angley + deltaAngley));
		lz = -cos(anglex + deltaAnglex) * cos(-(angley + deltaAngley));

		// update camera's direction
		lxStrap = -cos(anglex + deltaAnglex);
		lyStrap = sin(anglex + deltaAnglex) * sin(-(angley + deltaAngley));
		lzStrap = -sin(anglex + deltaAnglex) * cos(-(angley + deltaAngley));

		//deltaAnglex = 0.0f;
		//deltaAngley = 0.0f;
		//if (!freeviewActive)return;
		//// update freeview
		//Vector3f axis((float)-deltaAngley, (float)-deltaAnglex, 0.0f);
		//float angle = scale_rotation * sqrt((float)(deltaAnglex * deltaAnglex + deltaAngley*deltaAngley));
		//Matrix3f rot = createRotation(axis, angle);
		//freeviewPose.SetRT(rot * freeviewPose.GetR(), rot * freeviewPose.GetT());
		//freeviewPose.Coerce();

	}
}

void mouseButton(int button, int state, int x, int y) {

	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {
		// when the button is released
		if (state == GLUT_UP) {
			anglex += deltaAnglex;
			angley += deltaAngley;
			xOrigin = -1;
			yOrigin = -1;

		}
		else {// state = GLUT_DOWN
			xOrigin = x;
			yOrigin = y;
		}

	}
}

void Right_menu(int val)
{
	switch (val)
	{
	case 0:
		//KinectLive();
		cout << "case 0" << endl;
		break;
	case 1:
		//KinectOffLine();
		cout << "case 1" << endl;

		break;
	case 2:
		//KinectV2Live();
		cout << "case 2" << endl;
		break;
	case 3:
		//KinectV2OffLine();
		cout << "case 3" << endl;
		break;
	case 4:
		break;
	default:
		break;
	}
	//Running = true;
}


void Idle() {
	if (Running && start) {
		starttime = clock();
		if (RGBD->Load() != 3) {
			endtime = clock();
			cout << "RGBD->Load : " << (endtime - starttime) << endl;
			starttime = clock();

			RGBD->Running();

			endtime = clock();
			cout << "RGBD->Running : " << (endtime - starttime) << endl;

			Mat depth, color;
			RGBD->getBackImages(depth, color);
			ITMMng->SetImages(color, depth);
			ITMMng->Running();
		}
		else Running = false;
	}
	glutPostRedisplay(); 
}

int RGBFunction(cl_context _context, cl_device_id _device)
{
	////////////OpenCL////////////////////////
	context = _context;
	device = _device;
	RGBD = new RGBDMnager(context, device);
	RGBD->SetParam(Calib, FILENAME, 0);

	glutMainLoop();
	return 0;
}

int RGBInitialize(int argc, _TCHAR * argv[])
{
	///////////////InfiniTAM////////////////////////
	ITMMng = make_unique<MyITMNamespase::ITMManager>(MyITMNamespase::ITMManager());

	//////////OpenGL////////////////////////

	char *inGlut = "Bouh";
	glutInit(&argc, &inGlut);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(cDepthWidth, 2 * cDepthHeight); // (3 * cDepthWidth, 2 * cDepthHeight);

	glutCreateWindow("fusion");
	GLenum err = glewInit();
	if (err == GLEW_OK) {
		cout << "GLEW OK : Glew Ver. " << glewGetString(GLEW_VERSION) << endl;
	}
	else {
		cout << "GLEW Error : " << glewGetErrorString(err) << endl;
	}
	/*** Initialize OpenGL ***/
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glClearColor(1.0f, 1.0f, 1.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
														// enable color tracking
	glEnable(GL_COLOR_MATERIAL);
	// set material properties which will be assigned by glColor
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	//glEnable(GL_DEPTH_TEST);

	intrinsics[0] = 2.0 * Calib[0] / cDepthWidth;
	intrinsics[5] = 2.0 * Calib[1] / cDepthHeight;
	intrinsics[10] = -(Zfar + Znear) / (Zfar - Znear);
	intrinsics[11] = -1.0;
	intrinsics[14] = -2.0*(Zfar * Znear) / (Zfar - Znear);

	int menu_general = glutCreateMenu(Right_menu);
	glutAddMenuEntry("Kinect V1 live", 0);
	glutAddMenuEntry("Kinect V1 off-line", 1);
	glutAddMenuEntry("Kinect V2 live", 2);
	glutAddMenuEntry("Kinect V2 off-line", 3);
	glutAddMenuEntry("Play back", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	//glutReshapeFunc(reshape);
	glutDisplayFunc(display);

	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyUp);

	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey);

	//here are the two new functions
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);

	glutIdleFunc(Idle);
	return 0;
}
