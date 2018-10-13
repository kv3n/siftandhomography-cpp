#include "pch.h"
#include "sfm.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

void ProcessSFM(std::string image_1, std::string image_2, bool show_result)
{
	using namespace cv;

	const Mat image1 = imread(image_1);
	const Mat image2 = imread(image_2);

	imshow("SFMA", image1);
	imshow("SFMB", image2);

	waitKey(0);
}