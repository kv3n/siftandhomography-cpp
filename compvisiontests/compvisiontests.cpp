// compvisionhw.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <string>
#include <filesystem>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

typedef std::vector<cv::DMatch> MatchNeighbours;
typedef std::vector<MatchNeighbours> Matches;
typedef std::vector<cv::KeyPoint> KeyPoints;

const int NUM_NEAREST_NEIGHBOURS = 5;
const double THRESHOLD_DISTANCE = 0.75;
const int MAX_MATCHES = 20;


void AddPatchedTextToImage(cv::Mat& image, std::string text)
{
	using namespace cv;

	int baseline = 0;
	Size text_size = getTextSize(text, cv::FONT_HERSHEY_PLAIN, 1, 1, &baseline);
	rectangle(image, Rect(image.cols - text_size.width - 20, 0, text_size.width + 20, text_size.height * 2), cv::Scalar(0, 0, 0, 0), FILLED);
	putText(image, text, Point(image.cols - text_size.width - 5, text_size.height + baseline), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255, 0));
}


void WriteKeyPointOutput(std::string output_name, 
						 cv::InputArray image, 
						 const KeyPoints& keyPoints, 
						 bool showImage = false)
{
	cv::Mat output;
	cv::drawKeypoints(image, keyPoints, output, cv::Scalar_<double>::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	std::string num_features = "Features Found: " + std::to_string(keyPoints.size());
	AddPatchedTextToImage(output, num_features);
	

	cv::imwrite("Output\\" + output_name + ".jpg", output);	
	

	if (showImage)
	{
		cv::imshow(output_name, output);
	}
}


void WriteMatchesOutput(std::string output_name, 
						cv::InputArray sourceImage, 
						const KeyPoints& sourceKeypoints, 
						cv::InputArray destinationImage, 
						const KeyPoints& destinationKeypoints, 
						const MatchNeighbours& filteredMatches, 
						unsigned int goodMatchesFound,
						unsigned int totalMatchesFound,
						bool showImage = false)
{
	cv::Mat output;
	cv::drawMatches(sourceImage, sourceKeypoints, destinationImage, destinationKeypoints, filteredMatches, output);
	std::string num_matches = "Good Matches Found: " + std::to_string(goodMatchesFound) + " / " + std::to_string(totalMatchesFound);
	AddPatchedTextToImage(output, num_matches);
	cv::imwrite("Output\\" + output_name + ".jpg", output);

	if (showImage)
	{
		cv::imshow(output_name, output);
	}
}


void Process(std::string object_name, std::string scene_name, bool show_result = false)
{
	const cv::Mat object = cv::imread("Data\\src\\" + object_name + ".jpg");
	const cv::Mat scene = cv::imread("Data\\dst\\" + scene_name + ".jpg");


	// Fetch feature points
	cv::Ptr<cv::xfeatures2d::SIFT> detector = cv::xfeatures2d::SIFT::create();

	std::vector<cv::KeyPoint> object_keypoints;
	cv::Mat object_descriptors;
	detector->detectAndCompute(object, cv::noArray(), object_keypoints, object_descriptors);

	std::vector<cv::KeyPoint> scene_keypoints;
	cv::Mat scene_descriptors;
	detector->detectAndCompute(scene, cv::noArray(), scene_keypoints, scene_descriptors);


	// Match points in destination
	cv::Ptr<cv::BFMatcher> matcher = cv::BFMatcher::create();

	Matches matches;
	matcher->knnMatch(object_descriptors, scene_descriptors, matches, NUM_NEAREST_NEIGHBOURS);


	// Filter the matches
	MatchNeighbours good_matches;
	for (Matches::iterator neighbourItr = matches.begin(); neighbourItr != matches.end(); neighbourItr++)
	{
		MatchNeighbours neighbour = *neighbourItr;

		// 0 is the closest neigbour to match_matrix[match_id]'s query idx
		// Retain the match only if it is closer by 60% of the next nearest neighbour.
		if (neighbour.size() > 0 && neighbour.size() <= NUM_NEAREST_NEIGHBOURS)
		{
			if (neighbour[0].distance < THRESHOLD_DISTANCE * neighbour[1].distance)
			{
				good_matches.push_back(neighbour[0]);
			}
		}
	}
	
	// Sort the matches based on distance
	std::sort(good_matches.begin(), good_matches.end(), 
	[](cv::DMatch match1, cv::DMatch match2)
	{
		return match1.distance < match2.distance;
	});
	unsigned int goodMatchCount = good_matches.size();
	good_matches.resize(MAX_MATCHES);

	// Get output
	WriteKeyPointOutput(object_name, object, object_keypoints, show_result);
	WriteKeyPointOutput(scene_name, scene, scene_keypoints, show_result);
	WriteMatchesOutput(object_name + "_" + scene_name, object, object_keypoints, scene, scene_keypoints, good_matches, goodMatchCount, matches.size(), show_result);

	if (show_result)
	{
		cv::waitKey();
	}	
}


int main(int argc, const char* argv[])
{
	for (auto& object_filename : std::filesystem::directory_iterator("Data\\src"))
		for (auto& scene_filename : std::filesystem::directory_iterator("Data\\dst"))
			Process(object_filename.path().filename().stem().string(), scene_filename.path().filename().stem().string());

	return 0;
}