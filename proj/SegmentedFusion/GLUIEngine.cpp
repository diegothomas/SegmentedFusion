#include "GLUIEngine.h"

using namespace std;


GLUIEngine* GLUIEngine::instance;



GLUIEngine::GLUIEngine()
{
	//sceneManager = new SceneManager();
}


GLUIEngine::~GLUIEngine()
{
	//delete sceneManager;
}


void GLUIEngine::computePos(float deltaMove, float deltaStrap) {
	GLUIEngine *uiEngine = GLUIEngine::Instance();

	uiEngine->x += deltaMove * uiEngine->lx * 0.1f + deltaStrap * uiEngine->lxStrap * 0.1f;
	uiEngine->y += deltaMove * uiEngine->ly * 0.1f + deltaStrap * uiEngine->lyStrap * 0.1f;
	uiEngine->z += deltaMove * uiEngine->lz * 0.1f + deltaStrap * uiEngine->lzStrap * 0.1f;

	//uiEngine->deltaMove = 0; uiEngine->deltaStrap = 0;
}

void GLUIEngine::glutDisplayFunction()
{
	GLUIEngine *uiEngine = GLUIEngine::Instance();

	if (uiEngine->deltaMove || uiEngine->deltaStrap)
		computePos(uiEngine->deltaMove, uiEngine->deltaStrap);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glLoadMatrixf(uiEngine->intrinsics);				// Set up camera intrinsics

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set the camera
	gluLookAt(uiEngine->x, uiEngine->y, uiEngine->z,
		uiEngine->x + uiEngine->lx, uiEngine->y + uiEngine->ly, uiEngine->z + uiEngine->lz,
		0.0f, 1.0f, 0.0f);


	glViewport(0, 0, 2*cDepthWidth, 2 * cDepthHeight);
	uiEngine->sceneManager->Draw();


	glutSwapBuffers();

}

void GLUIEngine::glutIdleFunction()
{
	GLUIEngine *uiEngine = GLUIEngine::Instance();
	uiEngine->sceneManager->ProcessFrame();
	glutPostRedisplay();
}

void GLUIEngine::glutKeyUpFunction(unsigned char key, int x, int y)
{
	// TODO : add manasger of InfiniTAM?
	GLUIEngine *uiEngine = GLUIEngine::Instance();
	switch (key)
	{
	case 'm':
		cout << "next frame\r";
		uiEngine->sceneManager->ProcessNextFrame();
		break;
	
	//control camera pos & pose 
	//////////////////////////
	case 'r': {
		cout << "Reset camera angle" << endl;

		uiEngine->anglex = 0.0f;
		uiEngine->angley = 0.0f;

		uiEngine->lx = 0.0f;
		uiEngine->ly = 0.0f;
		uiEngine->lz = -1.0f;

		uiEngine->x = 0.0f;
		uiEngine->y = 0.0f;
		uiEngine->z = 0.0f;

		break;
	}
	case 'u': {
		cout << "Upper camera angle" << endl;
		uiEngine->anglex = 0.0f;
		uiEngine->angley = M_PI * 0.50000001f;
		uiEngine->x = 0.0f;
		uiEngine->y = 4.0f;
		uiEngine->z = -0.0f;
		uiEngine->setCameraPoseFunction();
		break;
	}
	case 'y': {
		cout << "Left camera angle" << endl;
		uiEngine->anglex = -M_PI * 0.5f;
		uiEngine->angley = 0.0f;
		uiEngine->x = 5.0f;
		uiEngine->y = 0.0f;
		uiEngine->z = -0.0f;
		uiEngine->setCameraPoseFunction();
		break;
	}
	case 'i': {
		cout << "Right camera angle" << endl;
		uiEngine->anglex = M_PI * 0.5f;
		uiEngine->angley = 0.0f;
		uiEngine->x = -5.0f;
		uiEngine->y = 0.0f;
		uiEngine->z = -0.0f;
		uiEngine->setCameraPoseFunction();
		break;
	}
	case 'j': {
		cout << "Back camera angle" << endl;
		uiEngine->anglex = M_PI;
		uiEngine->angley = 0.0f;
		uiEngine->x = 0.0f;
		uiEngine->y = 0.0f;
		uiEngine->z = -8.0f;
		uiEngine->setCameraPoseFunction();
		break;
	}
	////////////////////////////////////

	case 't':
		uiEngine->sceneManager->ProcessContFrame();
		break;
	case 'c':
		uiEngine->sceneManager->ProcessCapture();
		break;
	case 'b':
		uiEngine->sceneManager->ProcessReset();
		break;
	case 'k':
		uiEngine->sceneManager->ProcessKINECT(); break;
	case 'e':
	case 27: // esc key
		printf("exiting ...\n");
		exit(0);
		break;
	default:
		break;
	}


}

void GLUIEngine::pressKey(int key, int xx, int yy) {
	GLUIEngine *uiEngine = GLUIEngine::Instance();

	switch (key) {
	case GLUT_KEY_UP: uiEngine->deltaMove = 0.5f / 3.0f/*(fps/30.0)*/; break;
	case GLUT_KEY_DOWN: uiEngine->deltaMove = -0.5f / 3.0f/*(fps/30.0)*/; break;
	case GLUT_KEY_LEFT: uiEngine->deltaStrap = 0.5f / 3.0f/*(fps/30.0)*/; break;
	case GLUT_KEY_RIGHT: uiEngine->deltaStrap = -0.5f / 3.0f/*(fps/30.0)*/; break;

	}
}

void GLUIEngine::releaseKey(int key, int x, int y) {
	GLUIEngine *uiEngine = GLUIEngine::Instance();

	switch (key) {
	case GLUT_KEY_LEFT:
	case GLUT_KEY_RIGHT:
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN: uiEngine->deltaMove = 0; uiEngine->deltaStrap = 0; break;

	}
}

void GLUIEngine::glutMouseButtonFunction(int button, int state, int x, int y)
{
	GLUIEngine *uiEngine = GLUIEngine::Instance();

	if (state == GLUT_DOWN)
	{
		switch (button)
		{
		case GLUT_LEFT_BUTTON: uiEngine->mouseState = 1; break;
		case GLUT_MIDDLE_BUTTON: uiEngine->mouseState = 3; break;
		case GLUT_RIGHT_BUTTON: uiEngine->mouseState = 2; break;
		default: break;
		}
		uiEngine->mouseLastClick[0] = x;
		uiEngine->mouseLastClick[1] = y;

		glutSetCursor(GLUT_CURSOR_NONE);
	}
	else if (state == GLUT_UP && !uiEngine->mouseWarped)
	{
		uiEngine->mouseState = 0;
		glutSetCursor(GLUT_CURSOR_INHERIT);
	}
	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {
		// when the button is released
		if (state == GLUT_UP) {
			uiEngine->anglex += uiEngine->deltaAnglex;
			uiEngine->angley += uiEngine->deltaAngley;
			uiEngine->xOrigin = -1;
			uiEngine->yOrigin = -1;
		}
		else {// state = GLUT_DOWN
			uiEngine->xOrigin = x;
			uiEngine->yOrigin = y;
		}

	}
}

void GLUIEngine::glutMouseMoveFunction(int x, int y)
{
	GLUIEngine *uiEngine = GLUIEngine::Instance();
	// this will only be true when the left button is down
	if (uiEngine->xOrigin >= 0 || uiEngine->yOrigin >= 0) {

		// update deltaAngle
		uiEngine->deltaAnglex = (x - uiEngine->xOrigin) * 0.001f;
		uiEngine->deltaAngley = (y - uiEngine->yOrigin) * 0.001f;

		// update camera's direction
		uiEngine->lx = sin(uiEngine->anglex + uiEngine->deltaAnglex);
		uiEngine->ly = cos(uiEngine->anglex + uiEngine->deltaAnglex) * sin(-(uiEngine->angley + uiEngine->deltaAngley));
		uiEngine->lz = -cos(uiEngine->anglex + uiEngine->deltaAnglex) * cos(-(uiEngine->angley + uiEngine->deltaAngley));

		// update camera's direction
		uiEngine->lxStrap = -cos(uiEngine->anglex + uiEngine->deltaAnglex);
		uiEngine->lyStrap = sin(uiEngine->anglex + uiEngine->deltaAnglex) * sin(-(uiEngine->angley + uiEngine->deltaAngley));
		uiEngine->lzStrap = -sin(uiEngine->anglex + uiEngine->deltaAnglex) * cos(-(uiEngine->angley + uiEngine->deltaAngley));

		
	}

}

void GLUIEngine::glutMouseWheelFunction(int button, int dir, int x, int y)
{
	GLUIEngine *uiEngine = GLUIEngine::Instance();

}

void GLUIEngine::setCameraPoseFunction()
{
	GLUIEngine *uiEngine = GLUIEngine::Instance();

	uiEngine->lx = sin(uiEngine->anglex + uiEngine->deltaAnglex);
	uiEngine->ly = cos(uiEngine->anglex + uiEngine->deltaAnglex) * sin(-(uiEngine->angley + uiEngine->deltaAngley));
	uiEngine->lz = -cos(uiEngine->anglex + uiEngine->deltaAnglex) * cos(-(uiEngine->angley + uiEngine->deltaAngley));

	uiEngine->lxStrap = -cos(uiEngine->anglex + uiEngine->deltaAnglex);
	uiEngine->lyStrap = sin(uiEngine->anglex + uiEngine->deltaAnglex) * sin(-(uiEngine->angley + uiEngine->deltaAngley));
	uiEngine->lzStrap = -sin(uiEngine->anglex + uiEngine->deltaAnglex) * cos(-(uiEngine->angley + uiEngine->deltaAngley));

	uiEngine->deltaAnglex = 0.0f;
	uiEngine->deltaAngley = 0.0f;
}

void GLUIEngine::Initialise(int & argc, char** argv)
{
	char *inGlut = "Bouh";
	glutInit(&argc, &inGlut);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	glutInitWindowSize(2.1*cDepthWidth,  2.1*cDepthHeight);

	glutCreateWindow("OpenGL UI Engine");
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
														//glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
														// enable color tracking
	glEnable(GL_COLOR_MATERIAL);
	// set material properties which will be assigned by glColor
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_LIGHTING);
	GLfloat ambientLightq[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLightq[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLightq);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLightq);
	glEnable(GL_LIGHT0);

	intrinsics[0] = 2.0 * Calib[0] / cDepthWidth;
	intrinsics[5] = 2.0 * Calib[1] / cDepthHeight;
	intrinsics[10] = -(Zfar + Znear) / (Zfar - Znear);
	intrinsics[11] = -1.0;
	intrinsics[14] = -2.0*(Zfar * Znear) / (Zfar - Znear);



	glutDisplayFunc(GLUIEngine::glutDisplayFunction);
	glutKeyboardUpFunc(GLUIEngine::glutKeyUpFunction);
	glutMouseFunc(GLUIEngine::glutMouseButtonFunction);
	glutMotionFunc(GLUIEngine::glutMouseMoveFunction);
	glutIdleFunc(GLUIEngine::glutIdleFunction);
	glutSpecialFunc(GLUIEngine::pressKey);
	glutSpecialUpFunc(GLUIEngine::releaseKey);

}

void GLUIEngine::Shutdown()
{
	exit(1);
}

void GLUIEngine::Run(cl_context context, cl_device_id device)
{
	sceneManager = make_unique<SceneManager>(context,device);

	glutMainLoop();
}

void GLUIEngine::ProcessFrame()
{
}
