#pragma once

#include <iosfwd>
#include <array>

#include "objects.hpp"

namespace algo {
	enum RectangleSides {
		LEFT = 0, RIGHT = 1, BOTTOM = 2, TOP = 3
	};

	enum class LayoutAlignment {
		WIDTH_LESS, HEIGHT_LESS, NO_ALIGH
	};

	class AreasGrid;
	class GridBasedAlgorithm;

	class AreaLayout {
	private:
		AreaLayout(objects::Point point, double w, double h) : min_point{ point }, width{ w }, height{ h } {}
		void applyAlignment(LayoutAlignment align);

		objects::Point min_point{};
		double width{};
		double height{};
		double filled_width{};
		double filled_height{};
		bool inverted{};
		bool filled{};
		std::array<bool, 4> leaning_allowed{};
		std::vector<objects::PositionedCircle> circles;

		friend AreasGrid;
		friend GridBasedAlgorithm;
		friend std::ostream& operator<< (std::ostream& out, const AreaLayout& l);
	};
}