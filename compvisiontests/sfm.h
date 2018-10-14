#pragma once

#include <string>
#include <filesystem>

void ProcessSFM(std::filesystem::path image1_path, std::filesystem::path image2_path, bool show_result = false);