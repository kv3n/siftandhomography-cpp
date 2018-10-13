#include "pch.h"
#include "homographytest.h"
#include "sfm.h"
#include <filesystem>

int main(int argc, const char* argv[])
{
	int test_id = 2;

	switch (test_id)
	{
	case 1:
		for (auto& object_filename : std::filesystem::directory_iterator("Data\\src"))
		{
			for (auto& scene_filename : std::filesystem::directory_iterator("Data\\dst"))
			{
				ProcessForHomography(object_filename.path().filename().stem().string(), scene_filename.path().filename().stem().string(), true);
			}
		}
		break;
	case 2:
		for (auto& pair_directory : std::filesystem::directory_iterator("Data\\sfm"))
		{
			std::vector<std::string> image_pairs;
			for (auto& image_filename : std::filesystem::directory_iterator(pair_directory))
			{
				image_pairs.push_back(image_filename.path().string());
			}
			ProcessSFM(image_pairs[0], image_pairs[1], true);
		}
		break;

	default:
		break;
	}
	

	return 0;
}