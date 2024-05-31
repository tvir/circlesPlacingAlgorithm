#include "Algorithm.hpp"

#include <algorithm>
#include <iterator>

namespace algo {
    std::unique_ptr<Algorithm> createDefaultAlgorithm() {
        return std::make_unique<GridBasedAlgorithm>();
    }

    std::optional<objects::ResultData> GridBasedAlgorithm::calculate(const objects::Scene& scene) {
        initGrid(scene.getZone(), scene.getExclusionAreas());

        auto layouts = grid->calculateAllowedAreas(GridCalculationMode::HORIZONTAL, LayoutAlignment::WIDTH_LESS);

        bool success = fillLayouts(layouts, scene.getCircles());
        if (!success)
            return std::nullopt;

        relaxCircleDistribution(layouts);
        recalculateCirclesPositions(layouts);

        std::vector<objects::PositionedCircle> results;
        for (auto& l : layouts) {
            std::move(l.circles.begin(), l.circles.end(), std::back_inserter(results));
        }
        return objects::ResultData{ results };
    }

    void GridBasedAlgorithm::initGrid(const objects::Rectangle& zone, const std::vector<objects::Rectangle>& exclusion_areas) {
        grid = std::make_unique<AreasGrid>(zone, exclusion_areas);
    }

    bool GridBasedAlgorithm::fillLayouts(std::vector<AreaLayout>& layouts, std::vector<objects::Circle> circles) {
        std::sort(layouts.begin(), layouts.end(), [](auto& a, auto& b) {return a.width > b.width; });
        std::sort(circles.begin(), circles.end(), [](auto& a, auto& b) {return a.outRad() > b.outRad(); });

        double min_rad = circles.back().inRad();
        for (auto& c : circles)
            min_rad = std::min(min_rad, c.inRad());

        auto discard_it = std::upper_bound(layouts.begin(), layouts.end(), min_rad * 2,
            [](double min_width, auto el) {
                return min_width > el.width;
            });
        if (discard_it < layouts.end())
            layouts.erase(discard_it, layouts.end());

        size_t traversal_ind = 0;
        for (auto& c : circles) {
            if (!placeCircle(layouts, c, traversal_ind))
                return false;

            auto& last_layout = layouts[traversal_ind];
            if (last_layout.width - last_layout.filled_width < min_rad &&
                last_layout.height - last_layout.filled_height < min_rad)
                last_layout.filled = true;

            traversal_ind++;
        }
        return true;
    }

    bool GridBasedAlgorithm::placeCircle(std::vector<AreaLayout>& layouts, objects::Circle& circle, size_t& start) {
        for (size_t j = 0; j < layouts.size(); ++j) {
            size_t i = (start + j) % layouts.size();
            auto& cur_layout = layouts[i];

            if (!cur_layout.filled) {
                auto x_pos = findMinWidth(cur_layout, circle.inRad(), circle.outRad());
                auto y_pos = findMinHeight(cur_layout, circle.inRad(), circle.outRad());

                if (x_pos && y_pos) {
                    cur_layout.circles.push_back({ circle, {x_pos.value(), y_pos.value()} });
                    cur_layout.filled_height = y_pos.value() + circle.outRad();
                    cur_layout.filled_width = std::max(cur_layout.filled_width, x_pos.value() + circle.outRad());
                    start = i;
                    return true;
                }

                if (cur_layout.width - cur_layout.filled_width > circle.outRad() * 2) {
                    auto new_layout = splitLayout(cur_layout, circle.outRad() * 2);
                    y_pos = findMinHeight(new_layout, circle.inRad(), circle.outRad());
                    if (y_pos) {
                        new_layout.circles.push_back({ circle, {circle.outRad(), y_pos.value()} });
                        new_layout.filled_width = new_layout.width;
                        new_layout.filled_height = y_pos.value() + circle.outRad();
                        layouts.push_back(new_layout);
                        start = i;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    std::optional<double> GridBasedAlgorithm::findMinWidth(const AreaLayout& layout, double inRad, double outRad) {
        double min_width = layout.leaning_allowed[LEFT] ? inRad : outRad;
        double needed_width = min_width + (layout.leaning_allowed[RIGHT] ? inRad : outRad);

        if (layout.width < needed_width)
            return std::nullopt;

        return min_width;
    }

    std::optional<double> GridBasedAlgorithm::findMinHeight(const AreaLayout& layout, double inRad, double outRad) {
        double min_height = layout.filled_height + outRad;
        if (layout.circles.empty())
            min_height = layout.leaning_allowed[BOTTOM] ? inRad : outRad;

        double needed_height = min_height + (layout.leaning_allowed[TOP] ? inRad : outRad);
        if (layout.height < needed_height)
            return std::nullopt;

        return min_height;
    }

    AreaLayout GridBasedAlgorithm::splitLayout(AreaLayout& layout, double split_width) {
        objects::Point min_point{ layout.min_point };
        if (layout.inverted)
            min_point.y += layout.width - split_width;
        else
            min_point.x += layout.width - split_width;

        AreaLayout new_layout{ min_point, split_width, layout.height };
        new_layout.inverted = layout.inverted;
        new_layout.leaning_allowed = layout.leaning_allowed;
        new_layout.leaning_allowed[LEFT] = false;

        layout.width -= split_width;
        layout.leaning_allowed[RIGHT] = false;

        return new_layout;
    }

    void GridBasedAlgorithm::relaxCircleDistribution(std::vector<AreaLayout>& layouts) {
        for (auto& layout : layouts) {
            if (!layout.circles.empty()) {
                double x_offset = std::max(layout.width - layout.filled_width, 0.0);
                double y_offset = (layout.height - layout.filled_height) / layout.circles.size();
                for (size_t i = 0; i < layout.circles.size(); ++i) {
                    layout.circles[i].position.y += y_offset * (i + 1);
                    if (layout.width > layout.circles[i].outRad() * 2)
                        layout.circles[i].position.x = layout.width / 2;
                    else
                        layout.circles[i].position.x += x_offset;
                }
            }
        }
    }

    void GridBasedAlgorithm::recalculateCirclesPositions(std::vector<AreaLayout>& layouts) {
        for (auto& l : layouts) {
            for (auto& c : l.circles) {
                double x = l.min_point.x + (l.inverted ? c.position.y : c.position.x);
                double y = l.min_point.y + (l.inverted ? c.position.x : c.position.y);
                c.position = { x, y };
            }
        }
    }

}
