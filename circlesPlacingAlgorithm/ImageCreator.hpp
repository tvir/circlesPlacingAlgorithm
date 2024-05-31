#pragma once
#include <string>

#include "objects.hpp"


namespace imagecreator {

    void createResultingImage(const objects::Scene& scene, const objects::ResultData& results, const std::string& path);

}