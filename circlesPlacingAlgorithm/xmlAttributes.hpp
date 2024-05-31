#pragma once
#include <array>
#include <string>

namespace xmlAttributes {
    const char zonePath[] = "data/placement_zone/rect";
    const char alarmsPath[] = "data/placement_zone/alarms/alarms";
    const char circlesPath[] = "data/circles/circle";
    const char minPoint[] = "min_point";
    const char maxPoint[] = "max_point";
    const char x[] = "x";
    const char y[] = "y";
    const char id[] = "id";
    const char inRad[] = "inner_rad";
    const char outRad[] = "outter_rad";

    std::array<const char const*, 3> resultStr {"data", "circles", "circle"};
}