#include "pch.h"
#include "common.h"
#include "sfm.h"

void ProcessSFM(std::filesystem::path image1_path, std::filesystem::path image2_path, bool show_result)
{
	using namespace cv;

	const Mat image1 = imread(image1_path.string());
	const Mat image2 = imread(image2_path.string());

	// Break out image names from filepath
	std::string image1_name = image1_path.filename().stem().string();
	std::string image2_name = image2_path.filename().stem().string();

	// Fetch feature points
	Ptr<xfeatures2d::SIFT> detector = xfeatures2d::SIFT::create();

	KeyPoints image1_keypoints;
	Mat image1_descriptors;
	detector->detectAndCompute(image1, noArray(), image1_keypoints, image1_descriptors);

	KeyPoints image2_keypoints;
	Mat image2_descriptors;
	detector->detectAndCompute(image2, noArray(), image2_keypoints, image2_descriptors);

	// DO BRUTE FORCE MATCHING
	// Match points in destination
	Ptr<FlannBasedMatcher> matcher = makePtr<FlannBasedMatcher>(makePtr<flann::KDTreeIndexParams>(5), makePtr<flann::SearchParams>(50));
	Matches matches;
	matcher->knnMatch(image1_descriptors, image2_descriptors, matches, NUM_NEAREST_NEIGHBOURS);

	// Filter the matches
	MatchNeighbours good_matches;
	for (Matches::iterator matchItr = matches.begin(); matchItr != matches.end(); matchItr++)
	{
		MatchNeighbours neighbours = *matchItr;

		// 0 is the closest neigbour to match_matrix[match_id]'s query idx
		// Retain the match only if it is closer by 60% of the next nearest neighbour.
		if (neighbours.size() > 0 && neighbours.size() <= NUM_NEAREST_NEIGHBOURS)
		{
			if (neighbours[0].distance < THRESHOLD_DISTANCE * neighbours[1].distance)
			{
				good_matches.push_back(neighbours[0]);
			}
		}
	}

	// Sort the matches based on distance
	std::sort(good_matches.begin(), good_matches.end(),
		[](DMatch match1, DMatch match2)
	{
		return match1.distance < match2.distance;
	});

	// Draw Output
	std::vector<Point2f> object_box_outline;
	Mat essentialMatrixMask;
	

	// Separate out the image 1 and 2 points from good matches
	std::vector<Point2f> image1_points, image2_points;
	for (MatchNeighbours::iterator neighbourItr = good_matches.begin(); neighbourItr != good_matches.end(); neighbourItr++)
	{
		DMatch hoodMatch = *neighbourItr;
		image1_points.push_back(image1_keypoints[hoodMatch.queryIdx].pt);
		image2_points.push_back(image2_keypoints[hoodMatch.trainIdx].pt);
	}

	// Intrinsic Matrix
	Mat intrinsicMatrix = Mat::eye(3, 3, CV_64F);
	intrinsicMatrix.at<double>(0, 0) = 518.86; intrinsicMatrix.at<double>(0, 1) = 000.00; intrinsicMatrix.at<double>(0, 2) = 285.58;
	intrinsicMatrix.at<double>(1, 0) = 000.00; intrinsicMatrix.at<double>(1, 1) = 519.47; intrinsicMatrix.at<double>(1, 2) = 213.74;
	intrinsicMatrix.at<double>(2, 0) = 000.00; intrinsicMatrix.at<double>(2, 1) = 000.00; intrinsicMatrix.at<double>(2, 2) = 001.00;
	

	Mat essentialMatrix = findEssentialMat(image1_points, image2_points, intrinsicMatrix, RANSAC, 0.998999, 1.0, essentialMatrixMask);
	std::cout << essentialMatrix <<std::endl;
	unsigned int numInliers = (int)sum(essentialMatrixMask)[0];
	unsigned int numOutliers = image1_points.size() - numInliers;

	std::cout << "SFM Stats: \n\t Inliers: " << numInliers << "\n\t Outliers: " << numOutliers << std::endl << std::endl;


	WriteMatchesOutput("sfm",
		image1_name + "_" + image2_name,
		image1, image1_keypoints,
		image2, image2_keypoints,
		good_matches, good_matches.size(), matches.size(),
		object_box_outline, essentialMatrixMask,
		show_result);

	Mat rotation, translation;
	recoverPose(essentialMatrix, image1_points, image2_points, rotation, translation);

	std::cout << "Rotation: \n" << rotation << "\nTranslation: \n" << translation<<std::endl<<std::endl;

	// Calculate the Full-Projection Matrix
	Mat leftCameraM;
	hconcat(Mat::eye(3, 3, CV_64F), Mat::zeros(3, 1, CV_64F), leftCameraM);
	leftCameraM = intrinsicMatrix * leftCameraM;

	Mat rightCameraM;
	hconcat(rotation, translation, rightCameraM);
	rightCameraM = intrinsicMatrix * rightCameraM;


	// Undistort Consistent Points
	std::vector<Point2f> image1_consistent_points, image2_consistent_points;
	for (int i = 0; i < image1_points.size(); i++)
	{
		unsigned int element = (unsigned int)essentialMatrixMask.at<unsigned char>(i, 0);
		if (1 == element)
		{
			image1_consistent_points.push_back(image1_points[i]);
			image2_consistent_points.push_back(image2_points[i]);
		}
	}

	std::vector<Point2f> image1_undistorted_points, image2_undistorted_points;
	undistortPoints(image1_consistent_points, image1_undistorted_points, intrinsicMatrix, noArray());
	undistortPoints(image2_consistent_points, image2_undistorted_points, intrinsicMatrix, noArray());


	// Traingulate points to get final image points
	Mat triangulatedPoints;
	triangulatePoints(leftCameraM, rightCameraM, image1_undistorted_points, image2_undistorted_points, triangulatedPoints);	
	transpose(triangulatedPoints, triangulatedPoints);	

	std::vector<Point3f> pointCloud;	
	convertPointsFromHomogeneous(triangulatedPoints, pointCloud);


	// Print out points for python
	std::ofstream myfile;
	myfile.open("Output\\sfm\\points_" + image1_name + "_" + image2_name + ".txt");
	for (std::vector<Point3f>::iterator pointCloudItr = pointCloud.begin(); pointCloudItr != pointCloud.end(); pointCloudItr++)
	{
		Point3f point = *pointCloudItr;
		myfile << point.x <<", " << point.y <<", "<<point.z <<std::endl;
	}
	myfile.close();
}