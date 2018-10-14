#include "pch.h"
#include "homographytest.h"
#include "sfm.h"

int main(int argc, const char* argv[])
{
	int test_id = 2;
	bool show_result = true;

	switch (test_id)
	{
	case 1:
		for (auto& object_filename : std::filesystem::directory_iterator("Data\\src"))
		{
			for (auto& scene_filename : std::filesystem::directory_iterator("Data\\dst"))
			{
				ProcessForHomography(object_filename.path().filename().stem().string(), scene_filename.path().filename().stem().string(), show_result);
			}
		}
		break;
	case 2:
		for (auto& pair_directory : std::filesystem::directory_iterator("Data\\sfm"))
		{
			std::vector<std::filesystem::path> image_pairs;
			for (auto& image_filename : std::filesystem::directory_iterator(pair_directory))
			{
				image_pairs.push_back(image_filename.path());
			}
			ProcessSFM(image_pairs[0], image_pairs[1], show_result);

			if (show_result)
			{
				cv::waitKey();
				cv::destroyAllWindows();
			}
		}
		break;

	default:
		break;
	}
	

	return 0;
}