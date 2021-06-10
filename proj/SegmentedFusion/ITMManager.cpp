#include "ITMManager.h"



MyITMNamespase::ITMManager::ITMManager()
{
	/////////////InfiniTAM////////////////////////


	internalSettings = new ITMLibSettings();
	//if(!GPU_MODE)internalSettings->deviceType = internalSettings->DEVICE_CPU;

	ITMLib::ITMRGBDCalib ITMCalib;
	ITMCalib.intrinsics_d.SetFrom(cDepthWidth, cDepthHeight, Calib[0], Calib[1], Calib[2], Calib[3]);

	//mainEngine = new MyITMEngine(internalSettings, ITMCalib, ORUtils::Vector2<int>(cDepthHeight, cDepthWidth), ORUtils::Vector2<int>(cDepthHeight, cDepthWidth));
	mainEngine = new ITMBasicEngine<ITMVoxel, ITMVoxelIndex>(internalSettings, ITMCalib, ORUtils::Vector2<int>(cDepthHeight, cDepthWidth), ORUtils::Vector2<int>(cDepthHeight, cDepthWidth));
	//////////////////////////////////////

	depthImage = new ITMShortImage(Vector2i(cDepthHeight, cDepthWidth), true, true);
	colorImage = new ITMUChar4Image(Vector2i(cDepthHeight, cDepthWidth), true, true);

	outImageType[0] = ITMMainEngine::InfiniTAM_IMAGE_SCENERAYCAST;
	outImageType[1] = ITMMainEngine::InfiniTAM_IMAGE_ORIGINAL_DEPTH;

}


MyITMNamespase::ITMManager::~ITMManager()
{
	//delete internalSettings;
	//delete mainEngine;
	//delete depthImage;
	//delete colorImage;
}

void MyITMNamespase::ITMManager::SetImages(Mat color, Mat depth) {
	setDepthImage(depthImage, depth.clone());
	setImage(colorImage, color.clone());
}

int MyITMNamespase::ITMManager::Running(){
	mainEngine->ProcessFrame(colorImage, depthImage);

	if (!GPU_MODE) {
		if (mesh != NULL)delete mesh;
		mesh = mainEngine->GetMesh();

		if (cpu_triangles != NULL)delete cpu_triangles;
		cpu_triangles = new ORUtils::MemoryBlock<ITMLib::ITMMesh::Triangle>(noMaxTriangles, MEMORYDEVICE_CPU);
		cpu_triangles->SetFrom(mesh->triangles, ORUtils::MemoryBlock<ITMLib::ITMMesh::Triangle>::CUDA_TO_CPU);
		triangleArray = cpu_triangles->GetData(MEMORYDEVICE_CPU);
	}
	return 0;
}

void MyITMNamespase::ITMManager::Draw()
{
	if (!GPU_MODE)
	{
		if (mesh != NULL) {
			//glMatrixMode(GL_MODELVIEW);
			glColor3f(1.0, 1.0, 1.0);
			glBegin(GL_TRIANGLES);
			for (uint i = 0; i < mesh->noTotalTriangles; i++)
			{
				glVertex3f(triangleArray[i].p0.y, -triangleArray[i].p0.x, -triangleArray[i].p0.z);
				glVertex3f(triangleArray[i].p1.y, -triangleArray[i].p1.x, -triangleArray[i].p1.z);
				glVertex3f(triangleArray[i].p2.y, -triangleArray[i].p2.x, -triangleArray[i].p2.z);
			}
			glEnd();
		}
	}
	else {
		showImgs = new ITMUChar4Image(Vector2i(cDepthHeight, cDepthWidth), true, true);
		showImgs->ChangeDims(mainEngine->GetView()->depth->noDims);
		mainEngine->GetImage(showImgs, outImageType[0], &freeviewPose, &freeviewIntrinsics);

		glColor3f(1.0f, 1.0f, 1.0f);
		glEnable(GL_TEXTURE_2D);
		Vector4f winReg(0.0f, 0.0, 1.0f, 1.0f);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		{
			glLoadIdentity();
			glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, showImgs->noDims.x, showImgs->noDims.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, showImgs->GetData(MEMORYDEVICE_CPU));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glBegin(GL_QUADS); {
					glTexCoord2f(0, 1); glVertex2f(2-cameranumber - winReg[2], winReg[3]);
					glTexCoord2f(1, 1); glVertex2f(2-cameranumber - winReg[2], winReg[1]);
					glTexCoord2f(1, 0); glVertex2f(2-cameranumber - winReg[0], winReg[1]);
					glTexCoord2f(0, 0); glVertex2f(2-cameranumber - winReg[0], winReg[3]);
				}
				glEnd();
				glDisable(GL_TEXTURE_2D);
			}
			glPopMatrix();
		}
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		delete showImgs;

	}
	
}

void MyITMNamespase::ITMManager::KeyboardFunction(unsigned char key)
{
	if (freeviewActive)
	{
		outImageType[0] = ITMMainEngine::InfiniTAM_IMAGE_SCENERAYCAST;
		outImageType[1] = ITMMainEngine::InfiniTAM_IMAGE_ORIGINAL_DEPTH;

		freeviewActive = false;
	}
	else
	{
		outImageType[0] = ITMMainEngine::InfiniTAM_IMAGE_FREECAMERA_SHADED;
		outImageType[1] = ITMMainEngine::InfiniTAM_IMAGE_SCENERAYCAST;

		freeviewPose.SetFrom(mainEngine->GetTrackingState()->pose_d);
		if (mainEngine->GetView() != NULL) {
			freeviewIntrinsics = mainEngine->GetView()->calib.intrinsics_d;
			showImgs->ChangeDims(mainEngine->GetView()->depth->noDims);
		}
		freeviewActive = true;
	}
}


bool setImage(ITMUChar4Image *image, const cv::Mat &input) {
	Vector2i newSize(input.cols, input.rows);
	image->ChangeDims(newSize);
	Vector4u *data_ptr = image->GetData(MEMORYDEVICE_CPU);

	for (int i = 0; i < input.rows; ++i) {
		for (int j = 0; j < input.cols; ++j) {
			int idx = i * input.cols + j;
			// Convert from OpenCV's standard BGR format to RGB.
			cv::Vec3b col = input.at<cv::Vec3b>(i, j);
			data_ptr[idx].b = col[0];
			data_ptr[idx].g = col[1];
			data_ptr[idx].r = col[2];
			data_ptr[idx].a = 255u;
		}
	}

	return true;
}

bool setDepthImage(ITMShortImage *image, const cv::Mat &input) {
	for (int j = 0; j < cDepthHeight; j++)
	{
		for (int i = 0; i < cDepthWidth; i++)
		{
			//image->GetData(MEMORYDEVICE_CPU)[i*cDepthHeight + j] = static_cast<short>(input.at<unsigned short>(j, i)) / 16.0;
			image->GetData(MEMORYDEVICE_CPU)[i*cDepthHeight + j] = static_cast<short>(input.at<unsigned short>(j, i) / 8.0);
			//image->GetData(MEMORYDEVICE_CPU)[i*cDepthHeight + j] = static_cast<short>(input.at<unsigned short>(j, i) );
		}
	}
	return true;
}

void ITMD2Cv(const ITMShortImage &itm, cv::Mat1s *out_mat) {
	for (int j = 0; j < cDepthHeight; j++)
	{
		for (int i = 0; i < cDepthWidth; ++i)
		{
			out_mat->at<short>(j, i) = itm.GetData(MEMORYDEVICE_CPU)[i*cDepthHeight + j];
		}
	}
}

void ITMRGB2Cv(const ITMUChar4Image &itm, cv::Mat3b *out_mat) {
	const Vector4u *itm_data = itm.GetData(MemoryDeviceType::MEMORYDEVICE_CPU);
	for (int i = 0; i < itm.noDims[1]; ++i) {
		for (int j = 0; j < itm.noDims[0]; ++j) {
			out_mat->at<cv::Vec3b>(j, i) = cv::Vec3b(
				itm_data[i * itm.noDims[0] + j].b,
				itm_data[i * itm.noDims[0] + j].g,
				itm_data[i * itm.noDims[0] + j].r
			);
		}
	}
}
