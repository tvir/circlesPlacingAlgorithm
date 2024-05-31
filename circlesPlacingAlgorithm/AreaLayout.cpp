#include "AreaLayout.hpp"

#include <ostream>
#include <algorithm>

namespace algo {

    void AreaLayout::applyAlignment(LayoutAlignment align) {
        bool new_invert{};
        if (align == LayoutAlignment::HEIGHT_LESS) 
            new_invert = (height > width);

        if (align == LayoutAlignment::WIDTH_LESS)
            new_invert = (width > height);

        if (new_invert) {
            std::swap(width, height);
            std::swap(leaning_allowed[LEFT], leaning_allowed[BOTTOM]);
            std::swap(leaning_allowed[RIGHT], leaning_allowed[TOP]);
            inverted = !inverted;
        }
    }

    std::ostream& operator<< (std::ostream& out, const AreaLayout& l) {
        out << l.min_point << " w: " << l.filled_width << "/" << l.width << " h: " << l.filled_height << "/" << l.height << "\n";
        out << "inverted: " << l.inverted << " filled: " << l.filled;
        out << " LRBT: " << l.leaning_allowed[LEFT] << " " << l.leaning_allowed[RIGHT] << " " <<
            l.leaning_allowed[BOTTOM] << " " << l.leaning_allowed[TOP] << "\n";
        if (!l.circles.empty()) {
            out << "Circles: \n";
            for (auto& c : l.circles)
                out << c << "\n";
        }
        return out;
    }
}