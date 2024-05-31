#pragma once

#include <iosfwd>
#include <vector>

#include "objects.hpp"
#include "AreaLayout.hpp"


namespace algo {
	enum class GridCalculationMode{
		HORIZONTAL
	};

	class AreasGrid {
    public:
		AreasGrid(const objects::Rectangle& zone, const std::vector<objects::Rectangle>& exclusion_areas);
		std::vector<AreaLayout> calculateAllowedAreas(GridCalculationMode mode, LayoutAlignment align = LayoutAlignment::NO_ALIGH);

    private:
        std::vector<bool> grid;
        std::vector<double> x_values;
        std::vector<double> y_values;

		void fillCoordsValues(const objects::Rectangle& zone, const std::vector<objects::Rectangle>& exclusion_areas);
		void fillGrid(const std::vector<objects::Rectangle>& exclusion_areas);
		std::vector<AreaLayout> calculateHorizontalAllowedAreas();
		AreaLayout createAreaLayout(size_t x0, size_t x, size_t y0, size_t y);


		friend std::ostream& operator<< (std::ostream& out, const AreasGrid& a);
	};

}