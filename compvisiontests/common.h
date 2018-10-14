#pragma once

#include "pch.h"

typedef std::vector<cv::DMatch> MatchNeighbours;
typedef std::vector<MatchNeighbours> Matches;
typedef std::vector<cv::KeyPoint> KeyPoints;

const int NUM_NEAREST_NEIGHBOURS = 2;
const double THRESHOLD_DISTANCE = 0.75;
const int MAX_MATCHES = 20;


void WriteKeyPointOutput(std::string dir_name, 
	std::string output_name,
	cv::InputArray image,
	const KeyPoints& keyPoints,
	bool showImage = false);


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
	bool showImage = false);