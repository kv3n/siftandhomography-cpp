// compvisionhw.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

int main(int argc, const char* argv[])
{
	const cv::Mat source = cv::imread("Data\\src_1.jpg"); // Query Image
	const cv::Mat destination = cv::imread("Data\\dst_1.jpg"); // Train Image
	

	// Fetch feature points
	cv::Ptr<cv::xfeatures2d::SIFT> detector = cv::xfeatures2d::SIFT::create();

	std::vector<cv::KeyPoint> source_keypoints;
	detector->detect(source, source_keypoints);
	
	std::vector<cv::KeyPoint> destination_keypoints;
	detector->detect(destination, destination_keypoints);

	
	// Match points in destination
	cv::BFMatcher bfMatcher = cv::BFMatcher();


	cv::Mat output;
	cv::drawKeypoints(source, source_keypoints, output, cv::Scalar_<double>::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	

	cv::imshow("Test", output);
	cv::waitKey();

	return 0;
}