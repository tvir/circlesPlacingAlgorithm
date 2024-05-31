#pragma once

#include <vector>
#include <memory>
#include <optional>

#include "AreasGrid.hpp"
#include "objects.hpp"

namespace algo{ 
    class Algorithm {
    public:
        virtual std::optional<objects::ResultData> calculate(const objects::Scene& scene) = 0;
        virtual ~Algorithm() = default;
    };

    std::unique_ptr<Algorithm> createDefaultAlgorithm();

	class GridBasedAlgorithm : public Algorithm {
    public:
        std::optional<objects::ResultData> calculate(const objects::Scene& scene) override;

    private:
        std::unique_ptr<AreasGrid> grid;

        void initGrid(const objects::Rectangle& zone, const std::vector<objects::Rectangle>& exclusion_areas);

        bool fillLayouts(std::vector<AreaLayout>& layouts, std::vector<objects::Circle> circles);
        bool placeCircle(std::vector<AreaLayout>& layouts, objects::Circle& circle, size_t& start);

        std::optional<double> findMinWidth(const AreaLayout& layout, double inRad, double outRad);
        std::optional<double> findMinHeight(const AreaLayout& layout, double inRad, double outRad);

        AreaLayout splitLayout(AreaLayout& layout, double split_width);
                 
        void relaxCircleDistribution(std::vector<AreaLayout>& layouts);
        void recalculateCirclesPositions(std::vector<AreaLayout>& layouts);
	};
}