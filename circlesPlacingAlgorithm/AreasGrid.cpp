#include "AreasGrid.hpp"

#include <algorithm>
#include <ostream>

namespace algo {
    AreasGrid::AreasGrid(const objects::Rectangle& zone, const std::vector<objects::Rectangle>& exclusion_areas) {
        fillCoordsValues(zone, exclusion_areas);
        fillGrid(exclusion_areas);
    }

    void AreasGrid::fillCoordsValues(const objects::Rectangle& zone, const std::vector<objects::Rectangle>& exclusion_areas) {
        x_values = { zone.minPoint().x, zone.maxPoint().x };
        y_values = { zone.minPoint().y, zone.maxPoint().y };

        for (auto& area : exclusion_areas) {
            x_values.insert(x_values.end(), { area.minPoint().x, area.maxPoint().x });
            y_values.insert(y_values.end(), { area.minPoint().y, area.maxPoint().y });
        }

        std::sort(x_values.begin(), x_values.end());
        std::sort(y_values.begin(), y_values.end());
        x_values.erase(std::unique(x_values.begin(), x_values.end()), x_values.end());
        y_values.erase(std::unique(y_values.begin(), y_values.end()), y_values.end());
    }

    void AreasGrid::fillGrid(const std::vector<objects::Rectangle>& exclusion_areas) {
        auto xSize = x_values.size() - 1;
        auto ySize = y_values.size() - 1;
        grid = std::vector<bool>(ySize * xSize, true);
        for (auto& area : exclusion_areas) {
            auto xMinIt = std::lower_bound(x_values.begin(), x_values.end(), area.minPoint().x);
            auto xMaxIt = std::lower_bound(xMinIt, x_values.end(), area.maxPoint().x);
            auto yMinIt = std::lower_bound(y_values.begin(), y_values.end(), area.minPoint().y);
            auto yMaxIt = std::lower_bound(yMinIt, y_values.end(), area.maxPoint().y);

            auto xMin = std::distance(x_values.begin(), xMinIt);
            auto xMax = std::distance(x_values.begin(), xMaxIt);
            auto yMin = std::distance(y_values.begin(), yMinIt);
            auto yMax = std::distance(y_values.begin(), yMaxIt);

            for (auto i = yMin; i < yMax; ++i) {
                std::fill(grid.begin() + xSize * i + xMin, grid.begin() + xSize * i + xMax, false);
            }
        }
    }

    std::vector<AreaLayout> AreasGrid::calculateAllowedAreas(GridCalculationMode mode, LayoutAlignment align) {
        if (mode == GridCalculationMode::HORIZONTAL) {
            auto layouts = calculateHorizontalAllowedAreas();
            for (auto& l : layouts)
                l.applyAlignment(align);
            return layouts;
        }
    }

    std::vector<AreaLayout> AreasGrid::calculateHorizontalAllowedAreas() {
        std::vector<AreaLayout> result;
        auto xSize = x_values.size() - 1;
        auto ySize = y_values.size() - 1;

        for (size_t i = 0; i < ySize; ++i) {
            for (size_t j = 0; j < xSize; ++j) {
                auto beginInd = i * xSize + j;
                if (grid[beginInd]) {
                    auto j0 = j;
                    auto rowBeginInd = i * xSize;
                    auto rowEndInd = (i + 1) * xSize;
                    auto endInd = std::adjacent_find(grid.begin() + beginInd, grid.begin() + rowEndInd, std::not_equal_to<bool>());
                    if (endInd == grid.begin() + rowEndInd)
                        endInd--;
                    j = std::distance(grid.begin() + rowBeginInd, endInd);

                    AreaLayout layout = createAreaLayout(j0, j+1, i, i+1);
                    layout.leaning_allowed[LEFT] = true;
                    layout.leaning_allowed[RIGHT] = true;
                    layout.leaning_allowed[BOTTOM] = (i == 0);
                    layout.leaning_allowed[TOP] = (i == ySize - 1);
                    result.push_back(layout);
                }
            }
        }
        return result;
    }

    AreaLayout AreasGrid::createAreaLayout(size_t x0, size_t x, size_t y0, size_t y) {
        objects::Point min_point = { x_values[x0], y_values[y0] };
        double w = x_values[x] - x_values[x0];
        double h = y_values[y] - y_values[y0];

        return AreaLayout{min_point, w, h};
    }

    std::ostream& operator<< (std::ostream& out, const AreasGrid& a) {
        out << "Grid: \n";
        out << "x_values: ";
        std::for_each(a.x_values.begin(), a.x_values.end(), [&out](auto el) {out << el << " "; });
        out << "\ny_values: ";
        std::for_each(a.y_values.begin(), a.y_values.end(), [&out](auto el) {out << el << " "; });
        out << "\n";
        auto xSize = a.x_values.size() - 1;
        auto ySize = a.y_values.size() - 1;
        for (size_t i = ySize - 1; i >= 0; --i) {
            for (size_t j = 0; j < xSize; ++j)
                out << a.grid[i * xSize + j] << " ";
            out << "\n";
        }
        return out;
    }
}
