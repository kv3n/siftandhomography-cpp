#include "pch.h"
#include "common.h"

void AddPatchedTextToImage(cv::Mat& image, std::string text)
{
	using namespace cv;

	int baseline = 0;
	Size text_size = getTextSize(text, cv::FONT_HERSHEY_PLAIN, 1, 1, &baseline);
	rectangle(image, Rect(image.cols - text_size.width - 20, 0, text_size.width + 20, text_size.height * 2), cv::Scalar(0, 0, 0, 0), FILLED);
	putText(image, text, Point(image.cols - text_size.width - 5, text_size.height + baseline), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255, 0));
}


void WriteKeyPointOutput(std::string dir_name,
	std::string output_name,
	cv::InputArray image,
	const KeyPoints& keyPoints,
	bool showImage)
{
	cv::Mat output;
	cv::drawKeypoints(image, keyPoints, output, cv::Scalar_<double>::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	std::string num_features = "Features Found: " + std::to_string(keyPoints.size());
	AddPatchedTextToImage(output, num_features);


	cv::imwrite("Output\\" + dir_name + "\\" + output_name + ".jpg", output);


	if (showImage)
	{
		cv::imshow(output_name, output);
	}
}


void WriteMatchesOutput(std::string dir_name,
	std::string output_name,
	cv::InputArray sourceImage,
	const KeyPoints& sourceKeypoints,
	cv::InputArray destinationImage,
	const KeyPoints& destinationKeypoints,
	const MatchNeighbours& filteredMatches,
	unsigned int goodMatchesFound,
	unsigned int totalMatchesFound,
	std::vector<cv::Point2f> boxInScene,
	cv::Mat MatchMask,
	bool showImage)
{
	// Keep interim pre-homography output for display
	MatchNeighbours displayedTop20Matches(filteredMatches.begin(), filteredMatches.begin() + MAX_MATCHES);


	cv::Mat preHomographyOutput;
	cv::drawMatches(sourceImage, sourceKeypoints, destinationImage, destinationKeypoints, displayedTop20Matches, preHomographyOutput);
	std::string num_matches = "Good Matches Found: " + std::to_string(goodMatchesFound) + " / " + std::to_string(totalMatchesFound);
	AddPatchedTextToImage(preHomographyOutput, num_matches);
	cv::imwrite("Output\\" + dir_name + "\\" + output_name + "_pre.jpg", preHomographyOutput);



	cv::Mat output;

	if (!MatchMask.empty())
	{
		// Filtering results post-homography from the homographyMask
		MatchNeighbours maskedTop20Matches;
		for (unsigned int matchIndex = 0; matchIndex < filteredMatches.size() && maskedTop20Matches.size() < MAX_MATCHES; matchIndex++)
		{
			unsigned int element = (unsigned int)MatchMask.at<unsigned char>(matchIndex, 0);
			if (1 == element)
			{
				maskedTop20Matches.push_back(filteredMatches[matchIndex]);
			}
		}
		cv::drawMatches(sourceImage, sourceKeypoints, destinationImage, destinationKeypoints, maskedTop20Matches, output);
	}	
	else
	{
		destinationImage.copyTo(output);
	}

	// The box in our matches is offset by the object image. Hence the need for this shift.
	if (!boxInScene.empty())
	{
		std::vector<cv::Point2i> boxInSceneOffset;
		for (unsigned int i = 0; i < boxInScene.size(); i++)
		{
			boxInSceneOffset.push_back(cv::Point2i((int)(boxInScene[i].x + (float)sourceImage.getMat().cols), (int)boxInScene[i].y));
		}
		cv::polylines(output, boxInSceneOffset, true, cv::Scalar(0, 0, 255, 0), 2);

		cv::imwrite("Output\\" + output_name + ".jpg", output);
	}	

	if (showImage)
	{
		if (MatchMask.empty())
		{
			cv::imshow(output_name, preHomographyOutput);
		}
		else
		{
			cv::imshow(output_name, output);
		}		
	}
}