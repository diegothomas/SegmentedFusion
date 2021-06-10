#pragma once

#pragma comment(lib, "ITMLib.lib")
#pragma comment(lib, "FernRelocLib.lib")
#pragma comment(lib, "InfiniTAM.lib")
#pragma comment(lib, "InfiniTAM_cli.lib")
#pragma comment(lib, "InputSource.lib")
#pragma comment(lib, "MiniSlamGraphLib.lib")
#pragma comment(lib, "ORUtils.lib")

#include "Header.h"
#include "ITMLib/ITMLibDefines.h"
#include "ITMLib/Core/ITMBasicEngine.h"
#include "ITMLib/Core/ITMBasicSurfelEngine.h"
#include "ITMLib/Core/ITMMultiEngine.h"

#include "Functions.h"


namespace MyITMNamespase {
	using namespace ITMLib;
	class ITMManager
	{
	public:
		ITMManager();
		~ITMManager();

		void SetImages(Mat color, Mat depth);

		int Running();

		void Draw();

		void KeyboardFunction(unsigned char key);

		int cameranumber;
		//GPU Mode
		bool GPU_MODE = true;
	private:
		ITMBasicEngine<ITMVoxel, ITMVoxelIndex> *mainEngine = NULL;
		ITMMeshingEngine<ITMVoxel, ITMVoxelIndex> *meshingEngine = NULL;
		ITMUChar4Image *colorImage = NULL;
		ITMShortImage *depthImage = NULL;

		ITMTrackingState::TrackingResult trackerResult;
		ITMLibSettings *internalSettings = NULL;
		ITMMesh *mesh = NULL;
		ITMMesh::Triangle *triangleArray = NULL;
		ORUtils::MemoryBlock<ITMLib::ITMMesh::Triangle> *cpu_triangles = NULL;

		int idim = 0;
		bool color = true;
		bool bump = true;
		bool quad = true;
		bool ready_to_bump = false;
		cv::Mat im1, im2;
		cv::Mat1s im1s;
		cv::Mat3b im23b;
		ITMLib::ITMIntrinsics freeviewIntrinsics;
		ITMUChar4Image* showImgs;


		//freeview mode
		bool freeviewActive = false;
		ITMLib::ITMMainEngine::GetImageType outImageType[3];
		ORUtils::SE3Pose freeviewPose;
		static const uint noMaxTriangles = SDF_LOCAL_BLOCK_NUM * 32;

	};

}

bool setImage(ITMUChar4Image * image, const cv::Mat & input);

bool setDepthImage(ITMShortImage * image, const cv::Mat & input);