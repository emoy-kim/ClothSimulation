#pragma once

#include "opencv2/opencv.hpp"

#ifdef _DEBUG
#pragma comment(lib,"opencv_calib3dd.lib")
#pragma comment(lib,"opencv_cored.lib")
#pragma comment(lib,"opencv_dnnd.lib")
#pragma comment(lib,"opencv_features2dd.lib")
#pragma comment(lib,"opencv_flannd.lib")
#pragma comment(lib,"opencv_highguid.lib")
#pragma comment(lib,"opencv_imgcodecsd.lib")
#pragma comment(lib,"opencv_imgprocd.lib")
#pragma comment(lib,"opencv_mld.lib")
#pragma comment(lib,"opencv_objdetectd.lib")
#pragma comment(lib,"opencv_photod.lib")
#pragma comment(lib,"opencv_stitchingd.lib")
#pragma comment(lib,"opencv_videod.lib")
#pragma comment(lib,"opencv_videoiod.lib")

#else
#pragma comment(lib,"opencv_calib3d.lib")
#pragma comment(lib,"opencv_core.lib")
#pragma comment(lib,"opencv_dnn.lib")
#pragma comment(lib,"opencv_features2d.lib")
#pragma comment(lib,"opencv_flann.lib")
#pragma comment(lib,"opencv_highgui.lib")
#pragma comment(lib,"opencv_imgcodecs.lib")
#pragma comment(lib,"opencv_imgproc.lib")
#pragma comment(lib,"opencv_ml.lib")
#pragma comment(lib,"opencv_objdetect.lib")
#pragma comment(lib,"opencv_photo.lib")
#pragma comment(lib,"opencv_stitching.lib")
#pragma comment(lib,"opencv_video.lib")
#pragma comment(lib,"opencv_videoio.lib")
#endif